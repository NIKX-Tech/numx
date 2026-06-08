/**
 * @file test_interpolate.c
 * @brief Interpolation tests — float64 / x64 host build.
 *        numx_real_t = double  (compiled with -DNUMX_USE_DOUBLE)
 */

#include "unity.h"
#include "numx/numx_types.h"
#include "numx/interpolate.h"

#define TOL      1e-8
#define TOL_CHEB 1e-8

static numx_real_t f_quad(numx_real_t x)  { return x*x; }
static numx_real_t f_linear(numx_real_t x){ return x; }
static numx_real_t f_const(numx_real_t x) { (void)x; return (numx_real_t)3.0; }

void test_linear_midpoint_known(void)
{
    numx_real_t xs[] = {0.0,1.0,2.0};
    numx_real_t ys[] = {0.0,1.0,4.0};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_interp_linear(xs, ys, 3, 0.5, &r));
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 0.5, r);
}

void test_linear_second_interval(void)
{
    numx_real_t xs[] = {0.0,1.0,2.0};
    numx_real_t ys[] = {0.0,1.0,4.0};
    numx_real_t r;
    numx_interp_linear(xs, ys, 3, 1.5, &r);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 2.5, r);
}

void test_linear_at_knots(void)
{
    numx_real_t xs[] = {0.0,1.0,3.0,6.0};
    numx_real_t ys[] = {1.0,3.0,2.0,5.0};
    numx_real_t r; numx_size_t i;
    for (i = 0; i < 4; i++)
    {
        numx_interp_linear(xs, ys, 4, xs[i], &r);
        TEST_ASSERT_DOUBLE_WITHIN(TOL, ys[i], r);
    }
}

void test_linear_clamp_below(void)
{
    numx_real_t xs[] = {1.0,2.0,3.0};
    numx_real_t ys[] = {10.0,20.0,30.0};
    numx_real_t r;
    numx_interp_linear(xs, ys, 3, 0.0, &r);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 10.0, r);
}

void test_linear_clamp_above(void)
{
    numx_real_t xs[] = {1.0,2.0,3.0};
    numx_real_t ys[] = {10.0,20.0,30.0};
    numx_real_t r;
    numx_interp_linear(xs, ys, 3, 5.0, &r);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 30.0, r);
}

void test_linear_n2(void)
{
    numx_real_t xs[] = {0.0,4.0}, ys[] = {0.0,8.0}, r;
    numx_interp_linear(xs, ys, 2, 2.0, &r);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 4.0, r);
}

void test_linear_null(void)
{
    numx_real_t xs[] = {0.0,1.0}, ys[] = {0.0,1.0}, r;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_interp_linear(NULL, ys, 2, 0.5, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_interp_linear(xs, NULL, 2, 0.5, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_interp_linear(xs, ys, 2, 0.5, NULL));
}

void test_linear_n_lt_2(void)
{
    numx_real_t xs[] = {1.0}, ys[] = {1.0}, r;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_interp_linear(xs, ys, 1, 1.0, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_interp_linear(xs, ys, 0, 1.0, &r));
}

void test_spline_linear_data_exact(void)
{
    numx_real_t xs[] = {0.0,1.0,2.0,3.0};
    numx_real_t ys[] = {0.0,1.0,2.0,3.0};
    numx_real_t m[4], r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_interp_spline_precompute(xs, ys, 4, m));
    TEST_ASSERT_EQUAL(NUMX_OK, numx_interp_spline_eval(xs, ys, m, 4, 0.5, &r));
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 0.5, r);
    TEST_ASSERT_EQUAL(NUMX_OK, numx_interp_spline_eval(xs, ys, m, 4, 1.7, &r));
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 1.7, r);
}

void test_spline_cubic_oneshot_midpoint(void)
{
    numx_real_t xs[] = {0.0,1.0,2.0,3.0};
    numx_real_t ys[] = {0.0,1.0,4.0,9.0};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_interp_spline_cubic(xs, ys, 4, 0.5, &r));
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 0.35, r);
}

void test_spline_at_knots(void)
{
    numx_real_t xs[] = {0.0,1.0,2.0,3.0,4.0};
    numx_real_t ys[] = {1.0,3.0,2.0,5.0,4.0};
    numx_real_t m[5], r; numx_size_t i;
    numx_interp_spline_precompute(xs, ys, 5, m);
    for (i = 0; i < 5; i++)
    {
        numx_interp_spline_eval(xs, ys, m, 5, xs[i], &r);
        TEST_ASSERT_DOUBLE_WITHIN(TOL, ys[i], r);
    }
}

void test_spline_clamp_below(void)
{
    numx_real_t xs[] = {1.0,2.0,3.0}, ys[] = {1.0,4.0,9.0}, r;
    numx_interp_spline_cubic(xs, ys, 3, 0.0, &r);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 1.0, r);
}

void test_spline_clamp_above(void)
{
    numx_real_t xs[] = {1.0,2.0,3.0}, ys[] = {1.0,4.0,9.0}, r;
    numx_interp_spline_cubic(xs, ys, 3, 5.0, &r);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 9.0, r);
}

void test_spline_n2(void)
{
    numx_real_t xs[] = {0.0,2.0}, ys[] = {0.0,4.0}, r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_interp_spline_cubic(xs, ys, 2, 1.0, &r));
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 2.0, r);
}

void test_spline_precompute_null(void)
{
    numx_real_t xs[] = {0.0,1.0}, ys[] = {0.0,1.0}, m[2];
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_interp_spline_precompute(NULL, ys, 2, m));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_interp_spline_precompute(xs, NULL, 2, m));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_interp_spline_precompute(xs, ys, 2, NULL));
}

void test_spline_precompute_n_lt_2(void)
{
    numx_real_t xs[] = {1.0}, ys[] = {1.0}, m[1];
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_interp_spline_precompute(xs, ys, 1, m));
}

void test_cheb_constant(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_interp_chebyshev(f_const, 4, 0.0, 1.0, 0.5, &r));
    TEST_ASSERT_DOUBLE_WITHIN(TOL_CHEB, 3.0, r);
}

void test_cheb_linear_exact(void)
{
    numx_real_t r;
    numx_interp_chebyshev(f_linear, 4, 0.0, 2.0, 1.0, &r);
    TEST_ASSERT_DOUBLE_WITHIN(TOL_CHEB, 1.0, r);
}

void test_cheb_quadratic_exact(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_interp_chebyshev(f_quad, 4, -1.0, 1.0, 0.5, &r));
    TEST_ASSERT_DOUBLE_WITHIN(TOL_CHEB, 0.25, r);
}

void test_cheb_quadratic_at_zero(void)
{
    numx_real_t r;
    numx_interp_chebyshev(f_quad, 4, -1.0, 1.0, 0.0, &r);
    TEST_ASSERT_DOUBLE_WITHIN(TOL_CHEB, 0.0, r);
}

void test_cheb_n2_gives_linear_interp(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_interp_chebyshev(f_quad, 2, -1.0, 1.0, 0.0, &r));
    TEST_ASSERT_DOUBLE_WITHIN(TOL_CHEB, 0.5, r);
}

void test_cheb_null(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
                      numx_interp_chebyshev(NULL, 4, 0.0, 1.0, 0.5, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
                      numx_interp_chebyshev(f_quad, 4, 0.0, 1.0, 0.5, NULL));
}

void test_cheb_b_le_a(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_interp_chebyshev(f_quad, 4, 1.0, 0.0, 0.5, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_interp_chebyshev(f_quad, 4, 1.0, 1.0, 0.5, &r));
}

void test_cheb_n_lt_2(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_interp_chebyshev(f_quad, 1, 0.0, 1.0, 0.5, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_interp_chebyshev(f_quad, 0, 0.0, 1.0, 0.5, &r));
}

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
