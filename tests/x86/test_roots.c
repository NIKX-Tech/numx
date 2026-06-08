/**
 * @file test_roots.c
 * @brief Root-finding tests — float32 / x86 host build.
 *        numx_real_t = float  (NUMX_USE_DOUBLE not set)
 */

#include "unity.h"
#include "numx/numx_types.h"
#include "numx/roots.h"

#define TOL      1e-5f
#define ROOT_TOL 1e-5f

static numx_real_t f_linear(numx_real_t x)   { return x - (numx_real_t)2.0; }
static numx_real_t df_linear(numx_real_t x)  { (void)x; return (numx_real_t)1.0; }
static numx_real_t f_quadratic(numx_real_t x) { return x*x - (numx_real_t)4.0; }
static numx_real_t df_quadratic(numx_real_t x){ return (numx_real_t)2.0*x; }
static numx_real_t f_cubic(numx_real_t x)     { return x*x*x - x; }
static numx_real_t df_cubic(numx_real_t x)    { return (numx_real_t)3.0*x*x - (numx_real_t)1.0; }

static numx_real_t f_double_root(numx_real_t x)
{ numx_real_t d = x-(numx_real_t)1.0; return d*d; }
static numx_real_t df_double_root(numx_real_t x)
{ return (numx_real_t)2.0*(x-(numx_real_t)1.0); }

void test_bisect_linear(void)
{
    numx_real_t root;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_root_bisect(f_linear, 0.0f, 5.0f, ROOT_TOL, &root));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 2.0f, root);
}

void test_bisect_quadratic_positive(void)
{
    numx_real_t root;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_root_bisect(f_quadratic, 0.0f, 5.0f, ROOT_TOL, &root));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 2.0f, root);
}

void test_bisect_quadratic_negative(void)
{
    numx_real_t root;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_root_bisect(f_quadratic, -5.0f, 0.0f, ROOT_TOL, &root));
    TEST_ASSERT_FLOAT_WITHIN(TOL, -2.0f, root);
}

void test_bisect_residual_near_zero(void)
{
    numx_real_t root, residual;
    numx_root_bisect(f_cubic, 0.5f, 2.0f, ROOT_TOL, &root);
    residual = f_cubic(root);
    if (residual < 0.0f) residual = -residual;
    TEST_ASSERT_TRUE(residual < 1e-3f);
}

void test_bisect_root_at_left_endpoint(void)
{
    numx_real_t root;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_root_bisect(f_linear, 2.0f, 5.0f, ROOT_TOL, &root));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 2.0f, root);
}

void test_bisect_root_at_right_endpoint(void)
{
    numx_real_t root;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_root_bisect(f_linear, 0.0f, 2.0f, ROOT_TOL, &root));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 2.0f, root);
}

void test_bisect_null_f(void)
{
    numx_real_t root;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_root_bisect(NULL, 0.0f, 1.0f, ROOT_TOL, &root));
}

void test_bisect_null_root(void)
{
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_root_bisect(f_linear, 0.0f, 5.0f, ROOT_TOL, NULL));
}

void test_bisect_no_bracket(void)
{
    numx_real_t root;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_root_bisect(f_quadratic, 3.0f, 5.0f, ROOT_TOL, &root));
}

void test_bisect_tol_zero(void)
{
    numx_real_t root;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_root_bisect(f_linear, 0.0f, 5.0f, 0.0f, &root));
}

void test_newton_linear(void)
{
    numx_real_t root;
    TEST_ASSERT_EQUAL(NUMX_OK,
                      numx_root_newton(f_linear, df_linear, 0.0f, ROOT_TOL, &root));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 2.0f, root);
}

void test_newton_quadratic(void)
{
    numx_real_t root;
    TEST_ASSERT_EQUAL(NUMX_OK,
                      numx_root_newton(f_quadratic, df_quadratic, 3.0f, ROOT_TOL, &root));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 2.0f, root);
}

void test_newton_residual_near_zero(void)
{
    numx_real_t root, res;
    numx_root_newton(f_cubic, df_cubic, 2.0f, ROOT_TOL, &root);
    res = f_cubic(root);
    if (res < 0.0f) res = -res;
    TEST_ASSERT_TRUE(res < 1e-3f);
}

void test_newton_already_at_root(void)
{
    numx_real_t root;
    TEST_ASSERT_EQUAL(NUMX_OK,
                      numx_root_newton(f_linear, df_linear, 2.0f, ROOT_TOL, &root));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 2.0f, root);
}

void test_newton_null_f(void)
{
    numx_real_t root;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
                      numx_root_newton(NULL, df_linear, 0.0f, ROOT_TOL, &root));
}

void test_newton_null_df(void)
{
    numx_real_t root;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
                      numx_root_newton(f_linear, NULL, 0.0f, ROOT_TOL, &root));
}

void test_newton_zero_derivative(void)
{
    numx_real_t root;
    numx_status_t s = numx_root_newton(f_double_root, df_double_root,
                                       1.0f, ROOT_TOL, &root);
    TEST_ASSERT_TRUE(s == NUMX_OK || s == NUMX_ERR_SINGULAR);
}

void test_newton_tol_negative(void)
{
    numx_real_t root;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_root_newton(f_linear, df_linear, 0.0f, -1.0f, &root));
}

void test_brent_linear(void)
{
    numx_real_t root;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_root_brent(f_linear, 0.0f, 5.0f, ROOT_TOL, &root));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 2.0f, root);
}

void test_brent_quadratic(void)
{
    numx_real_t root;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_root_brent(f_quadratic, 0.0f, 5.0f, ROOT_TOL, &root));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 2.0f, root);
}

void test_brent_cubic_root_at_one(void)
{
    numx_real_t root;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_root_brent(f_cubic, 0.5f, 1.5f, ROOT_TOL, &root));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 1.0f, root);
}

void test_brent_residual_near_zero(void)
{
    numx_real_t root, res;
    numx_root_brent(f_cubic, -1.5f, -0.5f, ROOT_TOL, &root);
    res = f_cubic(root);
    if (res < 0.0f) res = -res;
    TEST_ASSERT_TRUE(res < 1e-3f);
}

void test_brent_null(void)
{
    numx_real_t root;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
                      numx_root_brent(NULL, 0.0f, 5.0f, ROOT_TOL, &root));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
                      numx_root_brent(f_linear, 0.0f, 5.0f, ROOT_TOL, NULL));
}

void test_brent_no_bracket(void)
{
    numx_real_t root;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_root_brent(f_quadratic, 3.0f, 5.0f, ROOT_TOL, &root));
}

void test_brent_tol_zero(void)
{
    numx_real_t root;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_root_brent(f_linear, 0.0f, 5.0f, 0.0f, &root));
}

void numx_test_roots(void)
{
    RUN_TEST(test_bisect_linear);
    RUN_TEST(test_bisect_quadratic_positive);
    RUN_TEST(test_bisect_quadratic_negative);
    RUN_TEST(test_bisect_residual_near_zero);
    RUN_TEST(test_bisect_root_at_left_endpoint);
    RUN_TEST(test_bisect_root_at_right_endpoint);
    RUN_TEST(test_bisect_null_f);
    RUN_TEST(test_bisect_null_root);
    RUN_TEST(test_bisect_no_bracket);
    RUN_TEST(test_bisect_tol_zero);
    RUN_TEST(test_newton_linear);
    RUN_TEST(test_newton_quadratic);
    RUN_TEST(test_newton_residual_near_zero);
    RUN_TEST(test_newton_already_at_root);
    RUN_TEST(test_newton_null_f);
    RUN_TEST(test_newton_null_df);
    RUN_TEST(test_newton_zero_derivative);
    RUN_TEST(test_newton_tol_negative);
    RUN_TEST(test_brent_linear);
    RUN_TEST(test_brent_quadratic);
    RUN_TEST(test_brent_cubic_root_at_one);
    RUN_TEST(test_brent_residual_near_zero);
    RUN_TEST(test_brent_null);
    RUN_TEST(test_brent_no_bracket);
    RUN_TEST(test_brent_tol_zero);
}
