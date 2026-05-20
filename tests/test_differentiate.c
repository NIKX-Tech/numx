/**
 * @file test_differentiate.c
 * @brief Unit tests for numx numerical differentiation module.
 *
 * Test levels:
 *   L1 - Known-answer tests: verified against analytical truth.
 *   L2 - Property tests: Richardson more accurate than central for smooth f.
 *   L3 - Edge cases: constant function, linear function (exact for all methods).
 *   L4 - Error handling: null pointers, h <= 0.
 */

#include "unity.h"
#include "numx/numx_types.h"
#include "numx/differentiate.h"

/* float32 round-off dominates at small h; forward O(h) error with h=1e-3
 * and degree-3 f gives ~6e-3 truncation.  Central/Richardson are limited
 * to ~5e-4 by cancellation noise at float32 precision. */
#define TOL_FWD 1e-2f
#define TOL_CTR 1e-3f
#define TOL_RICH 1e-3f

static const numx_real_t H = 1e-3f;

/* ── Test functions ────────────────────────────────────────────────── */

/* f(x) = x^2,  f'(x) = 2x */
static numx_real_t f_quad(numx_real_t x) { return x * x; }

/* f(x) = x^3,  f'(x) = 3x^2 */
static numx_real_t f_cubic(numx_real_t x) { return x * x * x; }

/* f(x) = 1  (constant),  f'(x) = 0 */
static numx_real_t f_const(numx_real_t x)
{
    (void)x;
    return (numx_real_t)1.0;
}

/* f(x) = x  (linear),  f'(x) = 1 */
static numx_real_t f_linear(numx_real_t x) { return x; }

/* ════════════════════════════════════════════════════════════════════
 *  numx_diff_forward
 * ════════════════════════════════════════════════════════════════════ */

/* L1 */
void test_fwd_quadratic_at_3(void)
{
    /* f'(3) = 6 */
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_diff_forward(f_quad, 3.0f, H, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL_FWD, 6.0f, r);
}

void test_fwd_cubic_at_2(void)
{
    /* f'(2) = 12 */
    numx_real_t r;
    numx_diff_forward(f_cubic, 2.0f, H, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL_FWD, 12.0f, r);
}

/* L2 — constant and linear are exact regardless of h */
void test_fwd_constant_is_zero(void)
{
    numx_real_t r;
    numx_diff_forward(f_const, 5.0f, H, &r);
    TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.0f, r);
}

void test_fwd_linear_is_one(void)
{
    numx_real_t r;
    numx_diff_forward(f_linear, 5.0f, H, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL_FWD, 1.0f, r);
}

/* L4 */
void test_fwd_null(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_diff_forward(NULL, 1.0f, H, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_diff_forward(f_quad, 1.0f, H, NULL));
}

void test_fwd_nonpositive_h(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_diff_forward(f_quad, 1.0f, 0.0f, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_diff_forward(f_quad, 1.0f, -1.0f, &r));
}

/* ════════════════════════════════════════════════════════════════════
 *  numx_diff_central
 * ════════════════════════════════════════════════════════════════════ */

/* L1 */
void test_central_quadratic_at_3(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_diff_central(f_quad, 3.0f, H, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL_CTR, 6.0f, r);
}

void test_central_quadratic_at_neg2(void)
{
    /* f'(-2) = -4 */
    numx_real_t r;
    numx_diff_central(f_quad, -2.0f, H, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL_CTR, -4.0f, r);
}

void test_central_cubic_at_2(void)
{
    numx_real_t r;
    numx_diff_central(f_cubic, 2.0f, H, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL_CTR, 12.0f, r);
}

/* L2 */
void test_central_constant_is_zero(void)
{
    numx_real_t r;
    numx_diff_central(f_const, 5.0f, H, &r);
    TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.0f, r);
}

void test_central_linear_exact(void)
{
    /* Use x near 1 to avoid catastrophic cancellation in float32. */
    numx_real_t r;
    numx_diff_central(f_linear, 1.0f, H, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL_CTR, 1.0f, r);
}

/* L4 */
void test_central_null(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_diff_central(NULL, 1.0f, H, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_diff_central(f_quad, 1.0f, H, NULL));
}

void test_central_nonpositive_h(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_diff_central(f_quad, 1.0f, 0.0f, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_diff_central(f_quad, 1.0f, -1e-3f, &r));
}

/* ════════════════════════════════════════════════════════════════════
 *  numx_diff_richardson
 * ════════════════════════════════════════════════════════════════════ */

/* L1 */
void test_richardson_quadratic_at_3(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_diff_richardson(f_quad, 3.0f, H, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL_RICH, 6.0f, r);
}

void test_richardson_cubic_at_2(void)
{
    numx_real_t r;
    numx_diff_richardson(f_cubic, 2.0f, H, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL_RICH, 12.0f, r);
}

/* L2 — Richardson beats central when truncation error dominates (large h). */
void test_richardson_more_accurate_than_central(void)
{
    /* With h=0.1, central O(h^2) error ~ 0.01 dominates over float noise.
     * Richardson cancels the O(h^2) term, giving a much smaller error. */
    numx_real_t rc, rr;
    numx_real_t h_large = 0.1f;
    numx_diff_central(f_cubic, 2.0f, h_large, &rc);
    numx_diff_richardson(f_cubic, 2.0f, h_large, &rr);
    numx_real_t err_c = rc - 12.0f;
    if (err_c < 0.0f)
        err_c = -err_c;
    numx_real_t err_r = rr - 12.0f;
    if (err_r < 0.0f)
        err_r = -err_r;
    TEST_ASSERT_TRUE(err_r < err_c);
}

void test_richardson_constant_is_zero(void)
{
    numx_real_t r;
    numx_diff_richardson(f_const, 5.0f, H, &r);
    TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.0f, r);
}

/* L4 */
void test_richardson_null(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_diff_richardson(NULL, 1.0f, H, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_diff_richardson(f_quad, 1.0f, H, NULL));
}

void test_richardson_nonpositive_h(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_diff_richardson(f_quad, 1.0f, 0.0f, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_diff_richardson(f_quad, 1.0f, -1e-3f, &r));
}

/* ════════════════════════════════════════════════════════════════════
 *  Suite entry point
 * ════════════════════════════════════════════════════════════════════ */

void numx_test_differentiate(void)
{
    RUN_TEST(test_fwd_quadratic_at_3);
    RUN_TEST(test_fwd_cubic_at_2);
    RUN_TEST(test_fwd_constant_is_zero);
    RUN_TEST(test_fwd_linear_is_one);
    RUN_TEST(test_fwd_null);
    RUN_TEST(test_fwd_nonpositive_h);

    RUN_TEST(test_central_quadratic_at_3);
    RUN_TEST(test_central_quadratic_at_neg2);
    RUN_TEST(test_central_cubic_at_2);
    RUN_TEST(test_central_constant_is_zero);
    RUN_TEST(test_central_linear_exact);
    RUN_TEST(test_central_null);
    RUN_TEST(test_central_nonpositive_h);

    RUN_TEST(test_richardson_quadratic_at_3);
    RUN_TEST(test_richardson_cubic_at_2);
    RUN_TEST(test_richardson_more_accurate_than_central);
    RUN_TEST(test_richardson_constant_is_zero);
    RUN_TEST(test_richardson_null);
    RUN_TEST(test_richardson_nonpositive_h);
}
