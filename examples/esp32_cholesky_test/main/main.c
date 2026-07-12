/**
 * @file main.c
 * @brief Self-contained ESP32-S3 test/bench harness for numx's
 * numx_cholesky_decompose.
 *
 * Mirrors the 6 test cases in tests/test_linalg.c without depending on
 * Unity, so this project links src/linalg.c directly and needs nothing
 * beyond the ESP-IDF freertos/esp_timer components. Results and benchmark
 * numbers are printed over UART.
 */

#include <stdio.h>
#include <stdint.h>
#include "esp_timer.h"
#include "numx/linalg.h"

static int g_pass = 0;
static int g_fail = 0;

#define CHK(cond, label) do { \
    if (cond) { g_pass++; } \
    else { g_fail++; printf("  FAIL: %s\n", (label)); } \
} while (0)

#define CHK_STATUS(exp, got, label) CHK((exp) == (got), label)

static int approx(numx_real_t a, numx_real_t b, numx_real_t tol)
{
    numx_real_t d = a - b;
    if (d < 0)
        d = -d;
    return d <= tol;
}

#define TOL ((numx_real_t)1e-3)

/* L1 */
static void test_cholesky_decompose_3x3_success(void)
{
    numx_size_t n = 3;
    const numx_real_t A[9] = {
         4.0f,  12.0f, -16.0f,
        12.0f,  37.0f, -43.0f,
       -16.0f, -43.0f,  98.0f
    };
    numx_real_t L[9];

    CHK_STATUS(NUMX_OK, numx_cholesky_decompose(A, n, L), "3x3_success status");

    CHK(approx(L[0*3 + 0],  2.0f, TOL), "3x3_success L00");
    CHK(approx(L[0*3 + 1],  0.0f, TOL), "3x3_success L01");
    CHK(approx(L[0*3 + 2],  0.0f, TOL), "3x3_success L02");

    CHK(approx(L[1*3 + 0],  6.0f, TOL), "3x3_success L10");
    CHK(approx(L[1*3 + 1],  1.0f, TOL), "3x3_success L11");
    CHK(approx(L[1*3 + 2],  0.0f, TOL), "3x3_success L12");

    CHK(approx(L[2*3 + 0], -8.0f, TOL), "3x3_success L20");
    CHK(approx(L[2*3 + 1],  5.0f, TOL), "3x3_success L21");
    CHK(approx(L[2*3 + 2],  3.0f, TOL), "3x3_success L22");
}

/* L2 */
static void test_cholesky_decompose_residual_reconstruction(void)
{
    numx_size_t n = 3;
    const numx_real_t A[9] = {
         4.0f,  12.0f, -16.0f,
        12.0f,  37.0f, -43.0f,
       -16.0f, -43.0f,  98.0f
    };
    numx_real_t L[9], LT[9], LLT[9];
    numx_size_t i;

    numx_cholesky_decompose(A, n, L);
    numx_mat_transpose(L, n, n, LT);
    numx_mat_mul(L, n, n, LT, n, n, LLT);

    for (i = 0; i < n * n; i++)
        CHK(approx(A[i], LLT[i], TOL), "residual_reconstruction LLT != A");
}

/* L3 */
static void test_cholesky_decompose_non_spd_fail(void)
{
    numx_size_t n = 3;
    const numx_real_t A[9] = {
        0.0f, 1.0f, 2.0f,
        1.0f, 5.0f, 6.0f,
        2.0f, 6.0f, 9.0f
    };
    numx_real_t L[9];

    CHK_STATUS(NUMX_ERR_SINGULAR, numx_cholesky_decompose(A, n, L), "non_spd_fail");
}

/* L4 */
static void test_cholesky_decompose_null_A(void)
{
    numx_real_t L[4];
    CHK_STATUS(NUMX_ERR_NULL_PTR, numx_cholesky_decompose(NULL, 2, L), "null_A");
}

static void test_cholesky_decompose_null_L(void)
{
    numx_real_t A[4] = {4.0f, 0.0f, 0.0f, 4.0f};
    CHK_STATUS(NUMX_ERR_NULL_PTR, numx_cholesky_decompose(A, 2, NULL), "null_L");
}

static void test_cholesky_decompose_invalid_dim(void)
{
    numx_real_t L[4];
    numx_real_t A[4] = {4.0f, 0.0f, 0.0f, 4.0f};
    CHK_STATUS(NUMX_ERR_INVALID_ARG, numx_cholesky_decompose(A, 0, L), "invalid_dim");
}

static void run_all_tests(void)
{
    test_cholesky_decompose_3x3_success();
    test_cholesky_decompose_residual_reconstruction();
    test_cholesky_decompose_non_spd_fail();
    test_cholesky_decompose_null_A();
    test_cholesky_decompose_null_L();
    test_cholesky_decompose_invalid_dim();
}

/* ── Benchmark ─────────────────────────────────────────────────────── */

#define BENCH_N 100000

static void run_benchmarks(void)
{
    numx_size_t n = 3;
    const numx_real_t A[9] = {
         4.0f,  12.0f, -16.0f,
        12.0f,  37.0f, -43.0f,
       -16.0f, -43.0f,  98.0f
    };
    numx_real_t L[9];
    int i;
    int64_t t0, t1, us, ns_per_call;

    printf("\n| %-24s | N      | Total          | Per call   |\n", "Function");
    printf("|--------------------------|--------|----------------|------------|\n");

    t0 = esp_timer_get_time();
    for (i = 0; i < BENCH_N; i++)
        numx_cholesky_decompose(A, n, L);
    t1 = esp_timer_get_time();
    us = t1 - t0;
    ns_per_call = (us * 1000LL) / BENCH_N;
    printf("| %-24s | %6d | %10lld us | %10lld ns |\n",
           "cholesky_decompose 3x3", BENCH_N, (long long)us, (long long)ns_per_call);
}

void app_main(void)
{
    printf("=== numx cholesky_decompose self-contained test/bench harness (ESP32-S3) ===\n");
    run_all_tests();
    printf("\nRESULTS: %d PASS / %d FAIL / %d TOTAL\n", g_pass, g_fail, g_pass + g_fail);

    run_benchmarks();
    printf("\n=== done ===\n");
}
