/**
 * @file test_interpolate.c
 * @brief Unit tests for numx interpolation module.
 *
 * Test levels:
 *   L1 - Known-answer tests: verified against analytical truth.
 *   L2 - Property tests: interpolant passes through data points.
 *   L3 - Edge cases: x at boundary, n=2 (linear segment).
 *   L4 - Error handling: null pointers, n < 2, b <= a for Chebyshev.
 */

#include "unity.h"
#include "numx/numx_types.h"
#include "numx/interpolate.h"

#define TOL 1e-4f
#define TOL_CHEB 1e-4f

/* ── Helper for Chebyshev tests ────────────────────────────────────── */

static numx_real_t f_quad(numx_real_t x) { return x * x; }
static numx_real_t f_linear(numx_real_t x) { return x; }
static numx_real_t f_const(numx_real_t x)
{
    (void)x;
    return (numx_real_t)3.0;
}

/* ════════════════════════════════════════════════════════════════════
 *  numx_interp_linear
 * ════════════════════════════════════════════════════════════════════ */

/* L1 */
void test_linear_midpoint_known(void)
{
    numx_real_t xs[] = {0.0f, 1.0f, 2.0f};
    numx_real_t ys[] = {0.0f, 1.0f, 4.0f};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_interp_linear(xs, ys, 3, 0.5f, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 0.5f, r); /* linear between 0 and 1 */
}

void test_linear_second_interval(void)
{
    numx_real_t xs[] = {0.0f, 1.0f, 2.0f};
    numx_real_t ys[] = {0.0f, 1.0f, 4.0f};
    numx_real_t r;
    numx_interp_linear(xs, ys, 3, 1.5f, &r);
    /* linear between ys[1]=1 and ys[2]=4: 1 + 0.5*(4-1) = 2.5 */
    TEST_ASSERT_FLOAT_WITHIN(TOL, 2.5f, r);
}

/* L2 — interpolant reproduces data at knot points */
void test_linear_at_knots(void)
{
    numx_real_t xs[] = {0.0f, 1.0f, 3.0f, 6.0f};
    numx_real_t ys[] = {1.0f, 3.0f, 2.0f, 5.0f};
    numx_real_t r;
    numx_size_t i;
    for (i = 0; i < 4; i++)
    {
        numx_interp_linear(xs, ys, 4, xs[i], &r);
        TEST_ASSERT_FLOAT_WITHIN(TOL, ys[i], r);
    }
}

/* L3 */
void test_linear_clamp_below(void)
{
    numx_real_t xs[] = {1.0f, 2.0f, 3.0f};
    numx_real_t ys[] = {10.0f, 20.0f, 30.0f};
    numx_real_t r;
    numx_interp_linear(xs, ys, 3, 0.0f, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 10.0f, r);
}

void test_linear_clamp_above(void)
{
    numx_real_t xs[] = {1.0f, 2.0f, 3.0f};
    numx_real_t ys[] = {10.0f, 20.0f, 30.0f};
    numx_real_t r;
    numx_interp_linear(xs, ys, 3, 5.0f, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 30.0f, r);
}

void test_linear_n2(void)
{
    numx_real_t xs[] = {0.0f, 4.0f};
    numx_real_t ys[] = {0.0f, 8.0f};
    numx_real_t r;
    numx_interp_linear(xs, ys, 2, 2.0f, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 4.0f, r);
}

/* L4 */
void test_linear_null(void)
{
    numx_real_t xs[] = {0.0f, 1.0f};
    numx_real_t ys[] = {0.0f, 1.0f};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_interp_linear(NULL, ys, 2, 0.5f, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_interp_linear(xs, NULL, 2, 0.5f, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_interp_linear(xs, ys, 2, 0.5f, NULL));
}

void test_linear_n_lt_2(void)
{
    numx_real_t xs[] = {1.0f};
    numx_real_t ys[] = {1.0f};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_interp_linear(xs, ys, 1, 1.0f, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_interp_linear(xs, ys, 0, 1.0f, &r));
}

/* ════════════════════════════════════════════════════════════════════
 *  numx_interp_spline_precompute / eval  (and one-shot cubic)
 * ════════════════════════════════════════════════════════════════════ */

/* L1 */
void test_spline_linear_data_exact(void)
{
    /* For linear y = x, natural spline second derivatives are all 0 => exact */
    numx_real_t xs[] = {0.0f, 1.0f, 2.0f, 3.0f};
    numx_real_t ys[] = {0.0f, 1.0f, 2.0f, 3.0f};
    numx_real_t m[4], r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_interp_spline_precompute(xs, ys, 4, m));
    TEST_ASSERT_EQUAL(NUMX_OK, numx_interp_spline_eval(xs, ys, m, 4, 0.5f, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 0.5f, r);
    TEST_ASSERT_EQUAL(NUMX_OK, numx_interp_spline_eval(xs, ys, m, 4, 1.7f, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 1.7f, r);
}

void test_spline_cubic_oneshot_midpoint(void)
{
    /* Natural cubic spline on y=x^2 data; result at x=0.5 is 0.35 (not 0.25 —
     * splines don't reproduce x^2 exactly between knots with natural BC). */
    numx_real_t xs[] = {0.0f, 1.0f, 2.0f, 3.0f};
    numx_real_t ys[] = {0.0f, 1.0f, 4.0f, 9.0f};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_interp_spline_cubic(xs, ys, 4, 0.5f, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 0.35f, r);
}

/* L2 — spline passes through each data point exactly */
void test_spline_at_knots(void)
{
    numx_real_t xs[] = {0.0f, 1.0f, 2.0f, 3.0f, 4.0f};
    numx_real_t ys[] = {1.0f, 3.0f, 2.0f, 5.0f, 4.0f};
    numx_real_t m[5], r;
    numx_size_t i;
    numx_interp_spline_precompute(xs, ys, 5, m);
    for (i = 0; i < 5; i++)
    {
        numx_interp_spline_eval(xs, ys, m, 5, xs[i], &r);
        TEST_ASSERT_FLOAT_WITHIN(TOL, ys[i], r);
    }
}

/* L3 */
void test_spline_clamp_below(void)
{
    numx_real_t xs[] = {1.0f, 2.0f, 3.0f};
    numx_real_t ys[] = {1.0f, 4.0f, 9.0f};
    numx_real_t r;
    numx_interp_spline_cubic(xs, ys, 3, 0.0f, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 1.0f, r);
}

void test_spline_clamp_above(void)
{
    numx_real_t xs[] = {1.0f, 2.0f, 3.0f};
    numx_real_t ys[] = {1.0f, 4.0f, 9.0f};
    numx_real_t r;
    numx_interp_spline_cubic(xs, ys, 3, 5.0f, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 9.0f, r);
}

void test_spline_n2(void)
{
    /* n=2: no interior moments, degenerate to linear segment */
    numx_real_t xs[] = {0.0f, 2.0f};
    numx_real_t ys[] = {0.0f, 4.0f};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_interp_spline_cubic(xs, ys, 2, 1.0f, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 2.0f, r);
}

/* L4 */
void test_spline_precompute_null(void)
{
    numx_real_t xs[] = {0.0f, 1.0f};
    numx_real_t ys[] = {0.0f, 1.0f};
    numx_real_t m[2];
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_interp_spline_precompute(NULL, ys, 2, m));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_interp_spline_precompute(xs, NULL, 2, m));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_interp_spline_precompute(xs, ys, 2, NULL));
}

void test_spline_precompute_n_lt_2(void)
{
    numx_real_t xs[] = {1.0f};
    numx_real_t ys[] = {1.0f};
    numx_real_t m[1];
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_interp_spline_precompute(xs, ys, 1, m));
}

/* ════════════════════════════════════════════════════════════════════
 *  numx_interp_chebyshev
 * ════════════════════════════════════════════════════════════════════ */

/* L1 */
void test_cheb_constant(void)
{
    /* constant f = 3 should be reproduced exactly for any n */
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_interp_chebyshev(f_const, 4, 0.0f, 1.0f, 0.5f, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL_CHEB, 3.0f, r);
}

void test_cheb_linear_exact(void)
{
    /* f(x) = x, degree 1 < 4, should be exact */
    numx_real_t r;
    numx_interp_chebyshev(f_linear, 4, 0.0f, 2.0f, 1.0f, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL_CHEB, 1.0f, r);
}

void test_cheb_quadratic_exact(void)
{
    /* f(x) = x^2, degree 2 < 4 nodes, exact */
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_interp_chebyshev(f_quad, 4, -1.0f, 1.0f, 0.5f, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL_CHEB, 0.25f, r);
}

void test_cheb_quadratic_at_zero(void)
{
    numx_real_t r;
    numx_interp_chebyshev(f_quad, 4, -1.0f, 1.0f, 0.0f, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL_CHEB, 0.0f, r);
}

/* L3 */
void test_cheb_n2_gives_linear_interp(void)
{
    /* n=2 on [-1,1] with f=x^2: nodes at ±0.707, fvals both 0.5.
     * The barycentric interpolant is the constant function 0.5. */
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_interp_chebyshev(f_quad, 2, -1.0f, 1.0f, 0.0f, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL_CHEB, 0.5f, r);
}

/* L4 */
void test_cheb_null(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
                      numx_interp_chebyshev(NULL, 4, 0.0f, 1.0f, 0.5f, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
                      numx_interp_chebyshev(f_quad, 4, 0.0f, 1.0f, 0.5f, NULL));
}

void test_cheb_b_le_a(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_interp_chebyshev(f_quad, 4, 1.0f, 0.0f, 0.5f, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_interp_chebyshev(f_quad, 4, 1.0f, 1.0f, 0.5f, &r));
}

void test_cheb_n_lt_2(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_interp_chebyshev(f_quad, 1, 0.0f, 1.0f, 0.5f, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_interp_chebyshev(f_quad, 0, 0.0f, 1.0f, 0.5f, &r));
}

/* ════════════════════════════════════════════════════════════════════
 *  Suite entry point
 * ════════════════════════════════════════════════════════════════════ */

void numx_test_interpolate(void)
{
    RUN_TEST(test_linear_midpoint_known);
    RUN_TEST(test_linear_second_interval);
    RUN_TEST(test_linear_at_knots);
    RUN_TEST(test_linear_clamp_below);
    RUN_TEST(test_linear_clamp_above);
    RUN_TEST(test_linear_n2);
    RUN_TEST(test_linear_null);
    RUN_TEST(test_linear_n_lt_2);

    RUN_TEST(test_spline_linear_data_exact);
    RUN_TEST(test_spline_cubic_oneshot_midpoint);
    RUN_TEST(test_spline_at_knots);
    RUN_TEST(test_spline_clamp_below);
    RUN_TEST(test_spline_clamp_above);
    RUN_TEST(test_spline_n2);
    RUN_TEST(test_spline_precompute_null);
    RUN_TEST(test_spline_precompute_n_lt_2);

    RUN_TEST(test_cheb_constant);
    RUN_TEST(test_cheb_linear_exact);
    RUN_TEST(test_cheb_quadratic_exact);
    RUN_TEST(test_cheb_quadratic_at_zero);
    RUN_TEST(test_cheb_n2_gives_linear_interp);
    RUN_TEST(test_cheb_null);
    RUN_TEST(test_cheb_b_le_a);
    RUN_TEST(test_cheb_n_lt_2);
}
