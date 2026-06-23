/**
 * @file test_differentiate.c
 * @brief Differentiation tests — float32 / x86 host build.
 *        numx_real_t = float  (NUMX_USE_DOUBLE not set)
 *
 * float32 round-off dominates at small h; tolerances are loose.
 */

#include "unity.h"
#include "numx/numx_types.h"
#include "numx/differentiate.h"

#define TOL_FWD  (numx_real_t)1e-2
#define TOL_CTR  (numx_real_t)1e-3
#define TOL_RICH (numx_real_t)1e-3

static const numx_real_t H = (numx_real_t)1e-3;

static numx_real_t f_quad(numx_real_t x)  { return x*x; }
static numx_real_t f_cubic(numx_real_t x) { return x*x*x; }
static numx_real_t f_const(numx_real_t x) { (void)x; return (numx_real_t)1.0; }
static numx_real_t f_linear(numx_real_t x){ return x; }

void test_fwd_quadratic_at_3(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_diff_forward(f_quad, (numx_real_t)3.0, H, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL_FWD, (numx_real_t)6.0, r);
}

void test_fwd_cubic_at_2(void)
{
    numx_real_t r;
    numx_diff_forward(f_cubic, (numx_real_t)2.0, H, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL_FWD, (numx_real_t)12.0, r);
}

void test_fwd_constant_is_zero(void)
{
    numx_real_t r;
    numx_diff_forward(f_const, (numx_real_t)5.0, H, &r);
    TEST_ASSERT_FLOAT_WITHIN((numx_real_t)1e-6, (numx_real_t)0.0, r);
}

void test_fwd_linear_is_one(void)
{
    numx_real_t r;
    numx_diff_forward(f_linear, (numx_real_t)5.0, H, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL_FWD, (numx_real_t)1.0, r);
}

void test_fwd_null(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_diff_forward(NULL, (numx_real_t)1.0, H, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_diff_forward(f_quad, (numx_real_t)1.0, H, NULL));
}

void test_fwd_nonpositive_h(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_diff_forward(f_quad, (numx_real_t)1.0, (numx_real_t)0.0, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_diff_forward(f_quad, (numx_real_t)1.0, -(numx_real_t)1.0, &r));
}

void test_central_quadratic_at_3(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_diff_central(f_quad, (numx_real_t)3.0, H, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL_CTR, (numx_real_t)6.0, r);
}

void test_central_quadratic_at_neg2(void)
{
    numx_real_t r;
    numx_diff_central(f_quad, -(numx_real_t)2.0, H, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL_CTR, -(numx_real_t)4.0, r);
}

void test_central_cubic_at_2(void)
{
    numx_real_t r;
    numx_diff_central(f_cubic, (numx_real_t)2.0, H, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL_CTR, (numx_real_t)12.0, r);
}

void test_central_constant_is_zero(void)
{
    numx_real_t r;
    numx_diff_central(f_const, (numx_real_t)5.0, H, &r);
    TEST_ASSERT_FLOAT_WITHIN((numx_real_t)1e-6, (numx_real_t)0.0, r);
}

void test_central_linear_exact(void)
{
    numx_real_t r;
    numx_diff_central(f_linear, (numx_real_t)1.0, H, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL_CTR, (numx_real_t)1.0, r);
}

void test_central_null(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_diff_central(NULL, (numx_real_t)1.0, H, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_diff_central(f_quad, (numx_real_t)1.0, H, NULL));
}

void test_central_nonpositive_h(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_diff_central(f_quad, (numx_real_t)1.0, (numx_real_t)0.0, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_diff_central(f_quad, (numx_real_t)1.0, -(numx_real_t)1e-3, &r));
}

void test_richardson_quadratic_at_3(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_diff_richardson(f_quad, (numx_real_t)3.0, H, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL_RICH, (numx_real_t)6.0, r);
}

void test_richardson_cubic_at_2(void)
{
    numx_real_t r;
    numx_diff_richardson(f_cubic, (numx_real_t)2.0, H, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL_RICH, (numx_real_t)12.0, r);
}

void test_richardson_more_accurate_than_central(void)
{
    numx_real_t rc, rr;
    numx_real_t h_large = (numx_real_t)0.1;
    numx_diff_central(f_cubic, (numx_real_t)2.0, h_large, &rc);
    numx_diff_richardson(f_cubic, (numx_real_t)2.0, h_large, &rr);
    numx_real_t err_c = rc - (numx_real_t)12.0;
    if (err_c < (numx_real_t)0.0) err_c = -err_c;
    numx_real_t err_r = rr - (numx_real_t)12.0;
    if (err_r < (numx_real_t)0.0) err_r = -err_r;
    TEST_ASSERT_TRUE(err_r < err_c);
}

void test_richardson_constant_is_zero(void)
{
    numx_real_t r;
    numx_diff_richardson(f_const, (numx_real_t)5.0, H, &r);
    TEST_ASSERT_FLOAT_WITHIN((numx_real_t)1e-6, (numx_real_t)0.0, r);
}

void test_richardson_null(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_diff_richardson(NULL, (numx_real_t)1.0, H, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_diff_richardson(f_quad, (numx_real_t)1.0, H, NULL));
}

void test_richardson_nonpositive_h(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_diff_richardson(f_quad, (numx_real_t)1.0, (numx_real_t)0.0, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_diff_richardson(f_quad, (numx_real_t)1.0, -(numx_real_t)1e-3, &r));
}

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
