/**
 * @file test_ntt.c
 * @brief Tests for numx NTT functions.
 *
 * Ring: Z_3329[x]/(x^256 + 1), omega = 17.
 * All coefficient values are in [0, q-1] where q = 3329.
 *
 * Polynomial encoding: f[k] is the coefficient of x^k; f[0] is the constant.
 * Multiplication in the ring wraps x^256 = -1, so x^k * x^j contributes
 * +f[k]*g[j] at index (k+j) mod 256 if k+j < 256, else -f[k]*g[j].
 */

#include "unity.h"
#include "numx/ntt.h"

/* q = 3329 */
#define NTT_Q 3329

/* ── Helpers ───────────────────────────────────────────────────────── */

/* Naive polynomial multiplication in Z_3329[x]/(x^256 + 1). */
static void poly_mul_ref(
    const numx_q15_t *a,
    const numx_q15_t *b,
    numx_q15_t       *out
)
{
    int32_t acc[256] = {0};
    int i, j;
    for (i = 0; i < 256; i++)
    {
        if (a[i] == 0) continue;
        for (j = 0; j < 256; j++)
        {
            int idx = (i + j) % 256;
            int sign = (i + j >= 256) ? -1 : 1;
            acc[idx] = (acc[idx] + (int32_t)sign * a[i] * b[j]) % NTT_Q;
            if (acc[idx] < 0) acc[idx] += NTT_Q;
        }
    }
    for (i = 0; i < 256; i++) out[i] = (numx_q15_t)acc[i];
}

/* Simple LCG for deterministic test vectors (no stdlib rand). */
static uint32_t priv_lcg(uint32_t *s)
{
    *s = *s * 1664525u + 1013904223u;
    return *s;
}

/* Fill polynomial with pseudorandom coefficients in [0, q-1]. */
static void rand_poly(numx_q15_t *f, uint32_t *seed)
{
    int i;
    for (i = 0; i < 256; i++)
        f[i] = (numx_q15_t)(priv_lcg(seed) % (uint32_t)NTT_Q);
}

/* ════════════════════════════════════════════════════════════════════
 *  Forward NTT
 * ════════════════════════════════════════════════════════════════════ */

/* L1 */
void test_ntt_forward_delta(void)
{
    /* NTT(1, 0, ..., 0) = [1, 0, 1, 0, ..., 1, 0]:
     * constant polynomial 1 evaluates to (1, 0) in every degree-2 ring. */
    numx_q15_t f[256] = {0};
    int i;
    f[0] = 1;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ntt_forward(f));
    for (i = 0; i < 256; i++)
    {
        if (i % 2 == 0)
            TEST_ASSERT_EQUAL_INT16(1, f[i]);
        else
            TEST_ASSERT_EQUAL_INT16(0, f[i]);
    }
}

void test_ntt_forward_zero(void)
{
    /* NTT(0, 0, ..., 0) = all zeros. */
    numx_q15_t f[256] = {0};
    int i;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ntt_forward(f));
    for (i = 0; i < 256; i++)
        TEST_ASSERT_EQUAL_INT16(0, f[i]);
}

/* L4 */
void test_ntt_forward_null(void)
{
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_ntt_forward(NULL));
}

/* ════════════════════════════════════════════════════════════════════
 *  Inverse NTT
 * ════════════════════════════════════════════════════════════════════ */

/* L1 */
void test_ntt_inverse_roundtrip_delta(void)
{
    /* INTT(NTT(delta)) = delta. */
    numx_q15_t f[256] = {0};
    f[0] = 1;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ntt_forward(f));
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ntt_inverse(f));
    TEST_ASSERT_EQUAL_INT16(1, f[0]);
    TEST_ASSERT_EQUAL_INT16(0, f[1]);
    TEST_ASSERT_EQUAL_INT16(0, f[2]);
    TEST_ASSERT_EQUAL_INT16(0, f[255]);
}

void test_ntt_inverse_roundtrip_x(void)
{
    /* INTT(NTT(x)) = x. */
    numx_q15_t f[256] = {0};
    f[1] = 1;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ntt_forward(f));
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ntt_inverse(f));
    TEST_ASSERT_EQUAL_INT16(0, f[0]);
    TEST_ASSERT_EQUAL_INT16(1, f[1]);
    TEST_ASSERT_EQUAL_INT16(0, f[2]);
}

void test_ntt_inverse_roundtrip_x2(void)
{
    /* INTT(NTT(x^2)) = x^2. */
    numx_q15_t f[256] = {0};
    f[2] = 1;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ntt_forward(f));
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ntt_inverse(f));
    TEST_ASSERT_EQUAL_INT16(0, f[0]);
    TEST_ASSERT_EQUAL_INT16(0, f[1]);
    TEST_ASSERT_EQUAL_INT16(1, f[2]);
    TEST_ASSERT_EQUAL_INT16(0, f[3]);
}

void test_ntt_inverse_roundtrip_full(void)
{
    /* INTT(NTT(f)) = f for all monomials x^k, k = 0..255. */
    numx_q15_t f[256];
    int k, i;
    for (k = 0; k < 256; k++)
    {
        for (i = 0; i < 256; i++) f[i] = 0;
        f[k] = 1;
        TEST_ASSERT_EQUAL(NUMX_OK, numx_ntt_forward(f));
        TEST_ASSERT_EQUAL(NUMX_OK, numx_ntt_inverse(f));
        TEST_ASSERT_EQUAL_INT16_MESSAGE(1, f[k], "monomial round-trip failed");
        TEST_ASSERT_EQUAL_INT16_MESSAGE(0, f[(k + 1) % 256], "leak into adjacent");
    }
}

/* L4 */
void test_ntt_inverse_null(void)
{
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_ntt_inverse(NULL));
}

/* ════════════════════════════════════════════════════════════════════
 *  Pointwise multiplication
 * ════════════════════════════════════════════════════════════════════ */

/* L1 */
void test_ntt_pointwise_mul_identity(void)
{
    /* NTT(delta) * NTT(f) after INTT should equal f. */
    numx_q15_t delta[256] = {0};
    numx_q15_t f[256]     = {0};
    numx_q15_t f_ntt[256];
    numx_q15_t out[256];
    int i;

    delta[0] = 1;
    f[0] = 7; f[1] = 3; f[50] = 100; f[255] = 1;

    for (i = 0; i < 256; i++) f_ntt[i] = f[i];

    numx_ntt_forward(delta);
    numx_ntt_forward(f_ntt);
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ntt_pointwise_mul(delta, f_ntt, out));
    numx_ntt_inverse(out);
    for (i = 0; i < 256; i++)
        TEST_ASSERT_EQUAL_INT16_MESSAGE(f[i], out[i], "pointwise_mul identity failed");
}

void test_ntt_pointwise_mul_known(void)
{
    /* (1+x) * (1+x) = 1 + 2x + x^2 via NTT pointwise. */
    numx_q15_t a[256] = {0};
    numx_q15_t b[256] = {0};
    numx_q15_t out[256];

    a[0] = 1; a[1] = 1;
    b[0] = 1; b[1] = 1;
    numx_ntt_forward(a);
    numx_ntt_forward(b);
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ntt_pointwise_mul(a, b, out));
    numx_ntt_inverse(out);
    TEST_ASSERT_EQUAL_INT16(1, out[0]);
    TEST_ASSERT_EQUAL_INT16(2, out[1]);
    TEST_ASSERT_EQUAL_INT16(1, out[2]);
    TEST_ASSERT_EQUAL_INT16(0, out[3]);
}

/* L4 */
void test_ntt_pointwise_mul_null(void)
{
    numx_q15_t a[256] = {0}, b[256] = {0}, out[256];
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_ntt_pointwise_mul(NULL, b, out));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_ntt_pointwise_mul(a, NULL, out));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_ntt_pointwise_mul(a, b, NULL));
}

/* ════════════════════════════════════════════════════════════════════
 *  Polynomial multiplication (numx_ntt_polymul)
 * ════════════════════════════════════════════════════════════════════ */

/* L1 */
void test_ntt_polymul_delta_identity(void)
{
    /* delta * f = f for any f. */
    numx_q15_t delta[256] = {0};
    numx_q15_t f[256]     = {0};
    numx_q15_t out[256];
    int i;

    delta[0] = 1;
    f[0] = 1; f[1] = 2; f[2] = 3; f[100] = 42; f[255] = 7;

    TEST_ASSERT_EQUAL(NUMX_OK, numx_ntt_polymul(delta, f, out));
    for (i = 0; i < 256; i++)
        TEST_ASSERT_EQUAL_INT16_MESSAGE(f[i], out[i], "delta * f != f");
}

void test_ntt_polymul_x_times_x(void)
{
    /* x * x = x^2. */
    numx_q15_t a[256] = {0};
    numx_q15_t b[256] = {0};
    numx_q15_t out[256];

    a[1] = 1;
    b[1] = 1;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ntt_polymul(a, b, out));
    TEST_ASSERT_EQUAL_INT16(0, out[0]);
    TEST_ASSERT_EQUAL_INT16(0, out[1]);
    TEST_ASSERT_EQUAL_INT16(1, out[2]);
}

void test_ntt_polymul_wrap_negacyclic(void)
{
    /* x^128 * x^128 = x^256 = -1 in Z_q[x]/(x^256+1).
     * -1 mod 3329 = 3328.                                */
    numx_q15_t a[256] = {0};
    numx_q15_t b[256] = {0};
    numx_q15_t out[256];
    int i;

    a[128] = 1;
    b[128] = 1;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ntt_polymul(a, b, out));
    TEST_ASSERT_EQUAL_INT16(NTT_Q - 1, out[0]);   /* -1 mod q */
    for (i = 1; i < 256; i++)
        TEST_ASSERT_EQUAL_INT16_MESSAGE(0, out[i], "non-zero tail after wrap");
}

void test_ntt_polymul_x255_times_x(void)
{
    /* x^255 * x = x^256 = -1 mod (x^256+1). */
    numx_q15_t a[256] = {0};
    numx_q15_t b[256] = {0};
    numx_q15_t out[256];

    a[255] = 1;
    b[1]   = 1;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ntt_polymul(a, b, out));
    TEST_ASSERT_EQUAL_INT16(NTT_Q - 1, out[0]);
    TEST_ASSERT_EQUAL_INT16(0, out[1]);
    TEST_ASSERT_EQUAL_INT16(0, out[255]);
}

/* L2 */
void test_ntt_polymul_commutativity(void)
{
    /* a*b = b*a. */
    numx_q15_t a[256] = {0};
    numx_q15_t b[256] = {0};
    numx_q15_t ab[256], ba[256];
    int i;

    a[0] = 1; a[1] = 2; a[2] = 3;
    b[0] = 5; b[1] = 1; b[3] = 7;

    TEST_ASSERT_EQUAL(NUMX_OK, numx_ntt_polymul(a, b, ab));
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ntt_polymul(b, a, ba));
    for (i = 0; i < 256; i++)
        TEST_ASSERT_EQUAL_INT16_MESSAGE(ab[i], ba[i], "commutativity violated");
}

void test_ntt_polymul_known_linear(void)
{
    /* (1 + x) * (1 + x) = 1 + 2x + x^2. */
    numx_q15_t a[256] = {0};
    numx_q15_t b[256] = {0};
    numx_q15_t out[256];

    a[0] = 1; a[1] = 1;
    b[0] = 1; b[1] = 1;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ntt_polymul(a, b, out));
    TEST_ASSERT_EQUAL_INT16(1, out[0]);
    TEST_ASSERT_EQUAL_INT16(2, out[1]);
    TEST_ASSERT_EQUAL_INT16(1, out[2]);
    TEST_ASSERT_EQUAL_INT16(0, out[3]);
}

void test_ntt_polymul_random_vs_ref(void)
{
    /* NTT-based polymul matches naive reference for 8 random pairs. */
    uint32_t seed = 0xDEADBEEFu;
    numx_q15_t a[256], b[256], ntt_out[256], ref_out[256];
    int trial, i;

    for (trial = 0; trial < 8; trial++)
    {
        rand_poly(a, &seed);
        rand_poly(b, &seed);
        TEST_ASSERT_EQUAL(NUMX_OK, numx_ntt_polymul(a, b, ntt_out));
        poly_mul_ref(a, b, ref_out);
        for (i = 0; i < 256; i++)
            TEST_ASSERT_EQUAL_INT16_MESSAGE(ref_out[i], ntt_out[i],
                                            "NTT polymul != reference");
    }
}

/* L4 */
void test_ntt_polymul_null(void)
{
    numx_q15_t a[256] = {0}, b[256] = {0}, out[256];
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_ntt_polymul(NULL, b, out));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_ntt_polymul(a, NULL, out));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_ntt_polymul(a, b, NULL));
}

/* ════════════════════════════════════════════════════════════════════
 *  Reduce
 * ════════════════════════════════════════════════════════════════════ */

/* L1 */
void test_ntt_reduce_noop_in_range(void)
{
    /* Values already in [0, q-1] are unchanged. */
    numx_q15_t f[256];
    int i;
    for (i = 0; i < 256; i++) f[i] = (numx_q15_t)(i % NTT_Q);
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ntt_reduce(f));
    for (i = 0; i < 256; i++)
        TEST_ASSERT_EQUAL_INT16((numx_q15_t)(i % NTT_Q), f[i]);
}

void test_ntt_reduce_boundary(void)
{
    /* 3329 reduces to 0; 3328 stays 3328; 0 stays 0. */
    numx_q15_t f[256] = {0};
    f[0] = 3329 % NTT_Q;  /* 0 */
    f[1] = 3328;
    f[2] = 0;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ntt_reduce(f));
    TEST_ASSERT_EQUAL_INT16(0,    f[0]);
    TEST_ASSERT_EQUAL_INT16(3328, f[1]);
    TEST_ASSERT_EQUAL_INT16(0,    f[2]);
}

/* L4 */
void test_ntt_reduce_null(void)
{
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_ntt_reduce(NULL));
}

/* ════════════════════════════════════════════════════════════════════
 *  Polynomial addition / subtraction
 * ════════════════════════════════════════════════════════════════════ */

/* L1 */
void test_ntt_poly_add_basic(void)
{
    /* (1 + 2x) + (3 + x) = 4 + 3x */
    numx_q15_t a[256] = {0};
    numx_q15_t b[256] = {0};
    numx_q15_t out[256];

    a[0] = 1; a[1] = 2;
    b[0] = 3; b[1] = 1;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ntt_poly_add(a, b, out));
    TEST_ASSERT_EQUAL_INT16(4, out[0]);
    TEST_ASSERT_EQUAL_INT16(3, out[1]);
    TEST_ASSERT_EQUAL_INT16(0, out[2]);
}

void test_ntt_poly_add_wrap(void)
{
    /* (q-1) + 1 = 0 mod q */
    numx_q15_t a[256] = {0};
    numx_q15_t b[256] = {0};
    numx_q15_t out[256];

    a[0] = NTT_Q - 1;
    b[0] = 1;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ntt_poly_add(a, b, out));
    TEST_ASSERT_EQUAL_INT16(0, out[0]);
}

/* L4 */
void test_ntt_poly_add_null(void)
{
    numx_q15_t a[256] = {0}, b[256] = {0}, out[256];
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_ntt_poly_add(NULL, b, out));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_ntt_poly_add(a, NULL, out));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_ntt_poly_add(a, b, NULL));
}

/* L1 */
void test_ntt_poly_sub_basic(void)
{
    /* (5 + 3x) - (2 + x) = 3 + 2x */
    numx_q15_t a[256] = {0};
    numx_q15_t b[256] = {0};
    numx_q15_t out[256];

    a[0] = 5; a[1] = 3;
    b[0] = 2; b[1] = 1;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ntt_poly_sub(a, b, out));
    TEST_ASSERT_EQUAL_INT16(3, out[0]);
    TEST_ASSERT_EQUAL_INT16(2, out[1]);
    TEST_ASSERT_EQUAL_INT16(0, out[2]);
}

void test_ntt_poly_sub_wrap(void)
{
    /* 0 - 1 = q-1 mod q */
    numx_q15_t a[256] = {0};
    numx_q15_t b[256] = {0};
    numx_q15_t out[256];

    a[0] = 0;
    b[0] = 1;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ntt_poly_sub(a, b, out));
    TEST_ASSERT_EQUAL_INT16(NTT_Q - 1, out[0]);
}

void test_ntt_poly_add_sub_inverse(void)
{
    /* (a + b) - b = a */
    numx_q15_t a[256] = {0};
    numx_q15_t b[256] = {0};
    numx_q15_t sum[256], result[256];
    int i;

    a[0] = 100; a[1] = 200; a[100] = 3000; a[255] = 1;
    b[0] = 999; b[1] =  42; b[100] =  500; b[200] = 7;

    TEST_ASSERT_EQUAL(NUMX_OK, numx_ntt_poly_add(a, b, sum));
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ntt_poly_sub(sum, b, result));
    for (i = 0; i < 256; i++)
        TEST_ASSERT_EQUAL_INT16_MESSAGE(a[i], result[i], "(a+b)-b != a");
}

/* L4 */
void test_ntt_poly_sub_null(void)
{
    numx_q15_t a[256] = {0}, b[256] = {0}, out[256];
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_ntt_poly_sub(NULL, b, out));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_ntt_poly_sub(a, NULL, out));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_ntt_poly_sub(a, b, NULL));
}

/* ════════════════════════════════════════════════════════════════════
 *  Suite entry point
 * ════════════════════════════════════════════════════════════════════ */

void numx_test_ntt(void)
{
    /* Forward NTT */
    RUN_TEST(test_ntt_forward_delta);
    RUN_TEST(test_ntt_forward_zero);
    RUN_TEST(test_ntt_forward_null);

    /* Inverse NTT */
    RUN_TEST(test_ntt_inverse_roundtrip_delta);
    RUN_TEST(test_ntt_inverse_roundtrip_x);
    RUN_TEST(test_ntt_inverse_roundtrip_x2);
    RUN_TEST(test_ntt_inverse_roundtrip_full);
    RUN_TEST(test_ntt_inverse_null);

    /* Pointwise multiplication */
    RUN_TEST(test_ntt_pointwise_mul_identity);
    RUN_TEST(test_ntt_pointwise_mul_known);
    RUN_TEST(test_ntt_pointwise_mul_null);

    /* Polynomial multiplication */
    RUN_TEST(test_ntt_polymul_delta_identity);
    RUN_TEST(test_ntt_polymul_x_times_x);
    RUN_TEST(test_ntt_polymul_wrap_negacyclic);
    RUN_TEST(test_ntt_polymul_x255_times_x);
    RUN_TEST(test_ntt_polymul_commutativity);
    RUN_TEST(test_ntt_polymul_known_linear);
    RUN_TEST(test_ntt_polymul_random_vs_ref);
    RUN_TEST(test_ntt_polymul_null);

    /* Reduce */
    RUN_TEST(test_ntt_reduce_noop_in_range);
    RUN_TEST(test_ntt_reduce_boundary);
    RUN_TEST(test_ntt_reduce_null);

    /* Polynomial addition / subtraction */
    RUN_TEST(test_ntt_poly_add_basic);
    RUN_TEST(test_ntt_poly_add_wrap);
    RUN_TEST(test_ntt_poly_add_null);
    RUN_TEST(test_ntt_poly_sub_basic);
    RUN_TEST(test_ntt_poly_sub_wrap);
    RUN_TEST(test_ntt_poly_add_sub_inverse);
    RUN_TEST(test_ntt_poly_sub_null);
}
