/**
 * @file main.c
 * @brief Self-contained ESP32-S3 test/bench harness for numx's NTT module.
 *
 * Mirrors the 29 test cases in tests/test_ntt.c (Ring: Z_3329[x]/(x^256+1),
 * omega = 17) without depending on Unity, so this project links src/ntt.c
 * directly and needs nothing beyond the ESP-IDF freertos/esp_timer
 * components. Results and benchmark numbers are printed over UART.
 */

#include <stdio.h>
#include <stdint.h>
#include "esp_timer.h"
#include "numx/ntt.h"

#define NTT_Q 3329

static int g_pass = 0;
static int g_fail = 0;

#define CHK(cond, label) do { \
    if (cond) { g_pass++; } \
    else { g_fail++; printf("  FAIL: %s\n", (label)); } \
} while (0)

#define CHK_STATUS(exp, got, label) CHK((exp) == (got), label)
#define CHK_I16(exp, got, label) CHK((int16_t)(exp) == (int16_t)(got), label)

/* ── Helpers (mirrors tests/test_ntt.c) ───────────────────────────── */

static void poly_mul_ref(const numx_q15_t *a, const numx_q15_t *b, numx_q15_t *out)
{
    int32_t acc[256] = {0};
    int i, j;
    for (i = 0; i < 256; i++)
    {
        if (a[i] == 0)
            continue;
        for (j = 0; j < 256; j++)
        {
            int idx = (i + j) % 256;
            int sign = (i + j >= 256) ? -1 : 1;
            acc[idx] = (acc[idx] + (int32_t)sign * a[i] * b[j]) % NTT_Q;
            if (acc[idx] < 0)
                acc[idx] += NTT_Q;
        }
    }
    for (i = 0; i < 256; i++)
        out[i] = (numx_q15_t)acc[i];
}

static uint32_t priv_lcg(uint32_t *s)
{
    *s = *s * 1664525u + 1013904223u;
    return *s;
}

static void rand_poly(numx_q15_t *f, uint32_t *seed)
{
    int i;
    for (i = 0; i < 256; i++)
        f[i] = (numx_q15_t)(priv_lcg(seed) % (uint32_t)NTT_Q);
}

/* ── Forward NTT ───────────────────────────────────────────────────── */

static void test_ntt_forward_delta(void)
{
    numx_q15_t f[256] = {0};
    int i;
    f[0] = 1;
    CHK_STATUS(NUMX_OK, numx_ntt_forward(f), "forward_delta status");
    for (i = 0; i < 256; i++)
        CHK_I16(i % 2 == 0 ? 1 : 0, f[i], "forward_delta value");
}

static void test_ntt_forward_zero(void)
{
    numx_q15_t f[256] = {0};
    int i;
    CHK_STATUS(NUMX_OK, numx_ntt_forward(f), "forward_zero status");
    for (i = 0; i < 256; i++)
        CHK_I16(0, f[i], "forward_zero value");
}

static void test_ntt_forward_null(void)
{
    CHK_STATUS(NUMX_ERR_NULL_PTR, numx_ntt_forward(NULL), "forward_null");
}

/* ── Inverse NTT ───────────────────────────────────────────────────── */

static void test_ntt_inverse_roundtrip_delta(void)
{
    numx_q15_t f[256] = {0};
    f[0] = 1;
    CHK_STATUS(NUMX_OK, numx_ntt_forward(f), "inv_rt_delta fwd status");
    CHK_STATUS(NUMX_OK, numx_ntt_inverse(f), "inv_rt_delta inv status");
    CHK_I16(1, f[0], "inv_rt_delta f0");
    CHK_I16(0, f[1], "inv_rt_delta f1");
    CHK_I16(0, f[2], "inv_rt_delta f2");
    CHK_I16(0, f[255], "inv_rt_delta f255");
}

static void test_ntt_inverse_roundtrip_x(void)
{
    numx_q15_t f[256] = {0};
    f[1] = 1;
    CHK_STATUS(NUMX_OK, numx_ntt_forward(f), "inv_rt_x fwd status");
    CHK_STATUS(NUMX_OK, numx_ntt_inverse(f), "inv_rt_x inv status");
    CHK_I16(0, f[0], "inv_rt_x f0");
    CHK_I16(1, f[1], "inv_rt_x f1");
    CHK_I16(0, f[2], "inv_rt_x f2");
}

static void test_ntt_inverse_roundtrip_x2(void)
{
    numx_q15_t f[256] = {0};
    f[2] = 1;
    CHK_STATUS(NUMX_OK, numx_ntt_forward(f), "inv_rt_x2 fwd status");
    CHK_STATUS(NUMX_OK, numx_ntt_inverse(f), "inv_rt_x2 inv status");
    CHK_I16(0, f[0], "inv_rt_x2 f0");
    CHK_I16(0, f[1], "inv_rt_x2 f1");
    CHK_I16(1, f[2], "inv_rt_x2 f2");
    CHK_I16(0, f[3], "inv_rt_x2 f3");
}

static void test_ntt_inverse_roundtrip_full(void)
{
    numx_q15_t f[256];
    int k, i;
    for (k = 0; k < 256; k++)
    {
        for (i = 0; i < 256; i++)
            f[i] = 0;
        f[k] = 1;
        CHK_STATUS(NUMX_OK, numx_ntt_forward(f), "inv_rt_full fwd status");
        CHK_STATUS(NUMX_OK, numx_ntt_inverse(f), "inv_rt_full inv status");
        CHK_I16(1, f[k], "inv_rt_full monomial round-trip");
        CHK_I16(0, f[(k + 1) % 256], "inv_rt_full leak into adjacent");
    }
}

static void test_ntt_inverse_null(void)
{
    CHK_STATUS(NUMX_ERR_NULL_PTR, numx_ntt_inverse(NULL), "inverse_null");
}

/* ── Pointwise multiplication ─────────────────────────────────────── */

static void test_ntt_pointwise_mul_identity(void)
{
    numx_q15_t delta[256] = {0};
    numx_q15_t f[256] = {0};
    numx_q15_t f_ntt[256];
    numx_q15_t out[256];
    int i;

    delta[0] = 1;
    f[0] = 7;
    f[1] = 3;
    f[50] = 100;
    f[255] = 1;
    for (i = 0; i < 256; i++)
        f_ntt[i] = f[i];

    numx_ntt_forward(delta);
    numx_ntt_forward(f_ntt);
    CHK_STATUS(NUMX_OK, numx_ntt_pointwise_mul(delta, f_ntt, out), "pw_mul_identity status");
    numx_ntt_inverse(out);
    for (i = 0; i < 256; i++)
        CHK_I16(f[i], out[i], "pw_mul_identity value");
}

static void test_ntt_pointwise_mul_known(void)
{
    numx_q15_t a[256] = {0};
    numx_q15_t b[256] = {0};
    numx_q15_t out[256];

    a[0] = 1;
    a[1] = 1;
    b[0] = 1;
    b[1] = 1;
    numx_ntt_forward(a);
    numx_ntt_forward(b);
    CHK_STATUS(NUMX_OK, numx_ntt_pointwise_mul(a, b, out), "pw_mul_known status");
    numx_ntt_inverse(out);
    CHK_I16(1, out[0], "pw_mul_known o0");
    CHK_I16(2, out[1], "pw_mul_known o1");
    CHK_I16(1, out[2], "pw_mul_known o2");
    CHK_I16(0, out[3], "pw_mul_known o3");
}

static void test_ntt_pointwise_mul_null(void)
{
    numx_q15_t a[256] = {0}, b[256] = {0}, out[256];
    CHK_STATUS(NUMX_ERR_NULL_PTR, numx_ntt_pointwise_mul(NULL, b, out), "pw_mul_null a");
    CHK_STATUS(NUMX_ERR_NULL_PTR, numx_ntt_pointwise_mul(a, NULL, out), "pw_mul_null b");
    CHK_STATUS(NUMX_ERR_NULL_PTR, numx_ntt_pointwise_mul(a, b, NULL), "pw_mul_null out");
}

/* ── Polynomial multiplication ────────────────────────────────────── */

static void test_ntt_polymul_delta_identity(void)
{
    numx_q15_t delta[256] = {0};
    numx_q15_t f[256] = {0};
    numx_q15_t out[256];
    int i;

    delta[0] = 1;
    f[0] = 1;
    f[1] = 2;
    f[2] = 3;
    f[100] = 42;
    f[255] = 7;

    CHK_STATUS(NUMX_OK, numx_ntt_polymul(delta, f, out), "polymul_delta_identity status");
    for (i = 0; i < 256; i++)
        CHK_I16(f[i], out[i], "polymul_delta_identity value");
}

static void test_ntt_polymul_x_times_x(void)
{
    numx_q15_t a[256] = {0};
    numx_q15_t b[256] = {0};
    numx_q15_t out[256];

    a[1] = 1;
    b[1] = 1;
    CHK_STATUS(NUMX_OK, numx_ntt_polymul(a, b, out), "polymul_x_times_x status");
    CHK_I16(0, out[0], "polymul_x_times_x o0");
    CHK_I16(0, out[1], "polymul_x_times_x o1");
    CHK_I16(1, out[2], "polymul_x_times_x o2");
}

static void test_ntt_polymul_wrap_negacyclic(void)
{
    numx_q15_t a[256] = {0};
    numx_q15_t b[256] = {0};
    numx_q15_t out[256];
    int i;

    a[128] = 1;
    b[128] = 1;
    CHK_STATUS(NUMX_OK, numx_ntt_polymul(a, b, out), "polymul_wrap status");
    CHK_I16(NTT_Q - 1, out[0], "polymul_wrap o0");
    for (i = 1; i < 256; i++)
        CHK_I16(0, out[i], "polymul_wrap non-zero tail");
}

static void test_ntt_polymul_x255_times_x(void)
{
    numx_q15_t a[256] = {0};
    numx_q15_t b[256] = {0};
    numx_q15_t out[256];

    a[255] = 1;
    b[1] = 1;
    CHK_STATUS(NUMX_OK, numx_ntt_polymul(a, b, out), "polymul_x255_times_x status");
    CHK_I16(NTT_Q - 1, out[0], "polymul_x255_times_x o0");
    CHK_I16(0, out[1], "polymul_x255_times_x o1");
    CHK_I16(0, out[255], "polymul_x255_times_x o255");
}

static void test_ntt_polymul_commutativity(void)
{
    numx_q15_t a[256] = {0};
    numx_q15_t b[256] = {0};
    numx_q15_t ab[256], ba[256];
    int i;

    a[0] = 1;
    a[1] = 2;
    a[2] = 3;
    b[0] = 5;
    b[1] = 1;
    b[3] = 7;

    CHK_STATUS(NUMX_OK, numx_ntt_polymul(a, b, ab), "polymul_commutativity ab status");
    CHK_STATUS(NUMX_OK, numx_ntt_polymul(b, a, ba), "polymul_commutativity ba status");
    for (i = 0; i < 256; i++)
        CHK_I16(ab[i], ba[i], "polymul_commutativity violated");
}

static void test_ntt_polymul_known_linear(void)
{
    numx_q15_t a[256] = {0};
    numx_q15_t b[256] = {0};
    numx_q15_t out[256];

    a[0] = 1;
    a[1] = 1;
    b[0] = 1;
    b[1] = 1;
    CHK_STATUS(NUMX_OK, numx_ntt_polymul(a, b, out), "polymul_known_linear status");
    CHK_I16(1, out[0], "polymul_known_linear o0");
    CHK_I16(2, out[1], "polymul_known_linear o1");
    CHK_I16(1, out[2], "polymul_known_linear o2");
    CHK_I16(0, out[3], "polymul_known_linear o3");
}

static void test_ntt_polymul_random_vs_ref(void)
{
    uint32_t seed = 0xDEADBEEFu;
    numx_q15_t a[256], b[256], ntt_out[256], ref_out[256];
    int trial, i;

    for (trial = 0; trial < 8; trial++)
    {
        rand_poly(a, &seed);
        rand_poly(b, &seed);
        CHK_STATUS(NUMX_OK, numx_ntt_polymul(a, b, ntt_out), "polymul_random_vs_ref status");
        poly_mul_ref(a, b, ref_out);
        for (i = 0; i < 256; i++)
            CHK_I16(ref_out[i], ntt_out[i], "polymul_random_vs_ref != reference");
    }
}

static void test_ntt_polymul_null(void)
{
    numx_q15_t a[256] = {0}, b[256] = {0}, out[256];
    CHK_STATUS(NUMX_ERR_NULL_PTR, numx_ntt_polymul(NULL, b, out), "polymul_null a");
    CHK_STATUS(NUMX_ERR_NULL_PTR, numx_ntt_polymul(a, NULL, out), "polymul_null b");
    CHK_STATUS(NUMX_ERR_NULL_PTR, numx_ntt_polymul(a, b, NULL), "polymul_null out");
}

/* ── Reduce ────────────────────────────────────────────────────────── */

static void test_ntt_reduce_noop_in_range(void)
{
    numx_q15_t f[256];
    int i;
    for (i = 0; i < 256; i++)
        f[i] = (numx_q15_t)(i % NTT_Q);
    CHK_STATUS(NUMX_OK, numx_ntt_reduce(f), "reduce_noop status");
    for (i = 0; i < 256; i++)
        CHK_I16((numx_q15_t)(i % NTT_Q), f[i], "reduce_noop value");
}

static void test_ntt_reduce_boundary(void)
{
    numx_q15_t f[256] = {0};
    f[0] = 3329 % NTT_Q;
    f[1] = 3328;
    f[2] = 0;
    CHK_STATUS(NUMX_OK, numx_ntt_reduce(f), "reduce_boundary status");
    CHK_I16(0, f[0], "reduce_boundary f0");
    CHK_I16(3328, f[1], "reduce_boundary f1");
    CHK_I16(0, f[2], "reduce_boundary f2");
}

static void test_ntt_reduce_null(void)
{
    CHK_STATUS(NUMX_ERR_NULL_PTR, numx_ntt_reduce(NULL), "reduce_null");
}

/* ── Polynomial addition / subtraction ────────────────────────────── */

static void test_ntt_poly_add_basic(void)
{
    numx_q15_t a[256] = {0};
    numx_q15_t b[256] = {0};
    numx_q15_t out[256];

    a[0] = 1;
    a[1] = 2;
    b[0] = 3;
    b[1] = 1;
    CHK_STATUS(NUMX_OK, numx_ntt_poly_add(a, b, out), "poly_add_basic status");
    CHK_I16(4, out[0], "poly_add_basic o0");
    CHK_I16(3, out[1], "poly_add_basic o1");
    CHK_I16(0, out[2], "poly_add_basic o2");
}

static void test_ntt_poly_add_wrap(void)
{
    numx_q15_t a[256] = {0};
    numx_q15_t b[256] = {0};
    numx_q15_t out[256];

    a[0] = NTT_Q - 1;
    b[0] = 1;
    CHK_STATUS(NUMX_OK, numx_ntt_poly_add(a, b, out), "poly_add_wrap status");
    CHK_I16(0, out[0], "poly_add_wrap o0");
}

static void test_ntt_poly_add_null(void)
{
    numx_q15_t a[256] = {0}, b[256] = {0}, out[256];
    CHK_STATUS(NUMX_ERR_NULL_PTR, numx_ntt_poly_add(NULL, b, out), "poly_add_null a");
    CHK_STATUS(NUMX_ERR_NULL_PTR, numx_ntt_poly_add(a, NULL, out), "poly_add_null b");
    CHK_STATUS(NUMX_ERR_NULL_PTR, numx_ntt_poly_add(a, b, NULL), "poly_add_null out");
}

static void test_ntt_poly_sub_basic(void)
{
    numx_q15_t a[256] = {0};
    numx_q15_t b[256] = {0};
    numx_q15_t out[256];

    a[0] = 5;
    a[1] = 3;
    b[0] = 2;
    b[1] = 1;
    CHK_STATUS(NUMX_OK, numx_ntt_poly_sub(a, b, out), "poly_sub_basic status");
    CHK_I16(3, out[0], "poly_sub_basic o0");
    CHK_I16(2, out[1], "poly_sub_basic o1");
    CHK_I16(0, out[2], "poly_sub_basic o2");
}

static void test_ntt_poly_sub_wrap(void)
{
    numx_q15_t a[256] = {0};
    numx_q15_t b[256] = {0};
    numx_q15_t out[256];

    a[0] = 0;
    b[0] = 1;
    CHK_STATUS(NUMX_OK, numx_ntt_poly_sub(a, b, out), "poly_sub_wrap status");
    CHK_I16(NTT_Q - 1, out[0], "poly_sub_wrap o0");
}

static void test_ntt_poly_add_sub_inverse(void)
{
    numx_q15_t a[256] = {0};
    numx_q15_t b[256] = {0};
    numx_q15_t sum[256], result[256];
    int i;

    a[0] = 100;
    a[1] = 200;
    a[100] = 3000;
    a[255] = 1;
    b[0] = 999;
    b[1] = 42;
    b[100] = 500;
    b[200] = 7;

    CHK_STATUS(NUMX_OK, numx_ntt_poly_add(a, b, sum), "poly_add_sub_inverse add status");
    CHK_STATUS(NUMX_OK, numx_ntt_poly_sub(sum, b, result), "poly_add_sub_inverse sub status");
    for (i = 0; i < 256; i++)
        CHK_I16(a[i], result[i], "poly_add_sub_inverse (a+b)-b != a");
}

static void test_ntt_poly_sub_null(void)
{
    numx_q15_t a[256] = {0}, b[256] = {0}, out[256];
    CHK_STATUS(NUMX_ERR_NULL_PTR, numx_ntt_poly_sub(NULL, b, out), "poly_sub_null a");
    CHK_STATUS(NUMX_ERR_NULL_PTR, numx_ntt_poly_sub(a, NULL, out), "poly_sub_null b");
    CHK_STATUS(NUMX_ERR_NULL_PTR, numx_ntt_poly_sub(a, b, NULL), "poly_sub_null out");
}

/* ── Suite entry point ────────────────────────────────────────────── */

static void run_all_tests(void)
{
    test_ntt_forward_delta();
    test_ntt_forward_zero();
    test_ntt_forward_null();

    test_ntt_inverse_roundtrip_delta();
    test_ntt_inverse_roundtrip_x();
    test_ntt_inverse_roundtrip_x2();
    test_ntt_inverse_roundtrip_full();
    test_ntt_inverse_null();

    test_ntt_pointwise_mul_identity();
    test_ntt_pointwise_mul_known();
    test_ntt_pointwise_mul_null();

    test_ntt_polymul_delta_identity();
    test_ntt_polymul_x_times_x();
    test_ntt_polymul_wrap_negacyclic();
    test_ntt_polymul_x255_times_x();
    test_ntt_polymul_commutativity();
    test_ntt_polymul_known_linear();
    test_ntt_polymul_random_vs_ref();
    test_ntt_polymul_null();

    test_ntt_reduce_noop_in_range();
    test_ntt_reduce_boundary();
    test_ntt_reduce_null();

    test_ntt_poly_add_basic();
    test_ntt_poly_add_wrap();
    test_ntt_poly_add_null();
    test_ntt_poly_sub_basic();
    test_ntt_poly_sub_wrap();
    test_ntt_poly_add_sub_inverse();
    test_ntt_poly_sub_null();
}

/* ── Benchmarks ────────────────────────────────────────────────────── */

#define BENCH_N 1000 /* watchdog-safe on ESP32-S3, see validation/results/ntt/ntt.md */

#define BENCH(lbl, n, body)                                          \
    do                                                               \
    {                                                                \
        int64_t _t0 = esp_timer_get_time();                          \
        for (int _i = 0; _i < (n); _i++)                             \
        {                                                            \
            body;                                                    \
        }                                                            \
        int64_t _t1 = esp_timer_get_time();                          \
        int64_t _us = _t1 - _t0;                                     \
        int64_t _ns_per_call = (_us * 1000LL) / (n);                 \
        printf("| %-20s | %6d | %10lld us | %10lld ns |\n",          \
               (lbl), (n), (long long)_us, (long long)_ns_per_call); \
    } while (0)

static void run_benchmarks(void)
{
    numx_q15_t a[256] = {0}, b[256] = {0}, out[256];
    int i;
    for (i = 0; i < 256; i++)
    {
        a[i] = (numx_q15_t)(i % NTT_Q);
        b[i] = (numx_q15_t)((255 - i) % NTT_Q);
    }

    printf("\n| %-20s | N      | Total          | Per call   |\n", "Function");
    printf("|----------------------|--------|----------------|------------|\n");

    BENCH("numx_ntt_forward", BENCH_N, numx_ntt_forward(a));
    BENCH("numx_ntt_inverse", BENCH_N, numx_ntt_inverse(a));
    BENCH("numx_ntt_polymul", BENCH_N, numx_ntt_polymul(a, b, out));
    BENCH("numx_ntt_poly_add", BENCH_N, numx_ntt_poly_add(a, b, out));
    BENCH("numx_ntt_poly_sub", BENCH_N, numx_ntt_poly_sub(a, b, out));
}

void app_main(void)
{
    printf("=== numx NTT self-contained test/bench harness (ESP32-S3) ===\n");
    run_all_tests();
    printf("\nRESULTS: %d PASS / %d FAIL / %d TOTAL\n", g_pass, g_fail, g_pass + g_fail);

    run_benchmarks();
    printf("\n=== done ===\n");
}
