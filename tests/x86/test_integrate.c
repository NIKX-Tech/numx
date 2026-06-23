/**
 * @file test_integrate.c
 * @brief Integration tests — float32 / x86 host build.
 *        numx_real_t = float  (NUMX_USE_DOUBLE not set)
 */

#include "unity.h"
#include "numx/numx_types.h"
#include "numx/integrate.h"

#define TOL_TRAP  (numx_real_t)1e-3
#define TOL_SIMP  (numx_real_t)1e-5
#define TOL_GAUSS (numx_real_t)1e-5

static numx_real_t f_one(numx_real_t x)   { (void)x; return (numx_real_t)1.0; }
static numx_real_t f_linear(numx_real_t x){ return x; }
static numx_real_t f_quad(numx_real_t x)  { return x*x; }
static numx_real_t f_cubic(numx_real_t x) { return x*x*x; }
static numx_real_t f_neg(numx_real_t x)   { return -x; }

void test_trap_constant_one(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_integrate_trap(f_one, (numx_real_t)0.0, (numx_real_t)1.0, 100, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL_TRAP, (numx_real_t)1.0, r);
}

void test_trap_linear(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_integrate_trap(f_linear, (numx_real_t)0.0, (numx_real_t)1.0, 100, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL_TRAP, (numx_real_t)0.5, r);
}

void test_trap_quadratic(void)
{
    numx_real_t r;
    numx_integrate_trap(f_quad, (numx_real_t)0.0, (numx_real_t)1.0, 1000, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TRAP, (numx_real_t)1.0/(numx_real_t)3.0, r);
}

void test_trap_linearity(void)
{
    numx_real_t r1, r2;
    numx_integrate_trap(f_linear, (numx_real_t)0.0, (numx_real_t)2.0, 100, &r1);
    numx_integrate_trap(f_neg, (numx_real_t)0.0, (numx_real_t)2.0, 100, &r2);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TRAP, r1, -r2);
}

void test_trap_n1(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_integrate_trap(f_one, (numx_real_t)0.0, (numx_real_t)1.0, 1, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL_TRAP, (numx_real_t)1.0, r);
}

void test_trap_null(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_integrate_trap(NULL, (numx_real_t)0.0, (numx_real_t)1.0, 10, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_integrate_trap(f_one, (numx_real_t)0.0, (numx_real_t)1.0, 10, NULL));
}

void test_trap_a_ge_b(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_integrate_trap(f_one, (numx_real_t)1.0, (numx_real_t)0.0, 10, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_integrate_trap(f_one, (numx_real_t)1.0, (numx_real_t)1.0, 10, &r));
}

void test_trap_n_zero(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_integrate_trap(f_one, (numx_real_t)0.0, (numx_real_t)1.0, 0, &r));
}

void test_simpson_constant_one(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_integrate_simpson(f_one, (numx_real_t)0.0, (numx_real_t)1.0, 2, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL_SIMP, (numx_real_t)1.0, r);
}

void test_simpson_linear_exact(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_integrate_simpson(f_linear, (numx_real_t)0.0, (numx_real_t)1.0, 2, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL_SIMP, (numx_real_t)0.5, r);
}

void test_simpson_quadratic_exact(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_integrate_simpson(f_quad, (numx_real_t)0.0, (numx_real_t)1.0, 4, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL_SIMP, (numx_real_t)1.0/(numx_real_t)3.0, r);
}

void test_simpson_cubic_exact(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_integrate_simpson(f_cubic, (numx_real_t)0.0, (numx_real_t)1.0, 4, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL_SIMP, (numx_real_t)0.25, r);
}

void test_simpson_wider_interval(void)
{
    numx_real_t r;
    numx_integrate_simpson(f_quad, (numx_real_t)0.0, (numx_real_t)3.0, 6, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL_SIMP, (numx_real_t)9.0, r);
}

void test_simpson_odd_n_rejected(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_integrate_simpson(f_one, (numx_real_t)0.0, (numx_real_t)1.0, 3, &r));
}

void test_simpson_n_lt_2_rejected(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_integrate_simpson(f_one, (numx_real_t)0.0, (numx_real_t)1.0, 0, &r));
}

void test_simpson_null(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
                      numx_integrate_simpson(NULL, (numx_real_t)0.0, (numx_real_t)1.0, 4, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
                      numx_integrate_simpson(f_one, (numx_real_t)0.0, (numx_real_t)1.0, 4, NULL));
}

void test_gauss2_linear(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_integrate_gauss(f_linear, (numx_real_t)0.0, (numx_real_t)1.0, 2, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL_GAUSS, (numx_real_t)0.5, r);
}

void test_gauss4_quadratic_exact(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_integrate_gauss(f_quad, (numx_real_t)0.0, (numx_real_t)1.0, 4, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL_GAUSS, (numx_real_t)1.0/(numx_real_t)3.0, r);
}

void test_gauss8_cubic_exact(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_integrate_gauss(f_cubic, (numx_real_t)0.0, (numx_real_t)1.0, 8, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL_GAUSS, (numx_real_t)0.25, r);
}

void test_gauss8_constant_exact(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_integrate_gauss(f_one, -(numx_real_t)3.0, (numx_real_t)3.0, 8, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL_GAUSS, (numx_real_t)6.0, r);
}

void test_gauss_null(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
                      numx_integrate_gauss(NULL, (numx_real_t)0.0, (numx_real_t)1.0, 4, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
                      numx_integrate_gauss(f_one, (numx_real_t)0.0, (numx_real_t)1.0, 4, NULL));
}

void test_gauss_invalid_npts(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_integrate_gauss(f_one, (numx_real_t)0.0, (numx_real_t)1.0, 3, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_integrate_gauss(f_one, (numx_real_t)0.0, (numx_real_t)1.0, 16, &r));
}

void test_gauss_a_ge_b(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_integrate_gauss(f_one, (numx_real_t)1.0, (numx_real_t)0.0, 4, &r));
}

void numx_test_integrate(void)
{
    RUN_TEST(test_trap_constant_one);
    RUN_TEST(test_trap_linear);
    RUN_TEST(test_trap_quadratic);
    RUN_TEST(test_trap_linearity);
    RUN_TEST(test_trap_n1);
    RUN_TEST(test_trap_null);
    RUN_TEST(test_trap_a_ge_b);
    RUN_TEST(test_trap_n_zero);
    RUN_TEST(test_simpson_constant_one);
    RUN_TEST(test_simpson_linear_exact);
    RUN_TEST(test_simpson_quadratic_exact);
    RUN_TEST(test_simpson_cubic_exact);
    RUN_TEST(test_simpson_wider_interval);
    RUN_TEST(test_simpson_odd_n_rejected);
    RUN_TEST(test_simpson_n_lt_2_rejected);
    RUN_TEST(test_simpson_null);
    RUN_TEST(test_gauss2_linear);
    RUN_TEST(test_gauss4_quadratic_exact);
    RUN_TEST(test_gauss8_cubic_exact);
    RUN_TEST(test_gauss8_constant_exact);
    RUN_TEST(test_gauss_null);
    RUN_TEST(test_gauss_invalid_npts);
    RUN_TEST(test_gauss_a_ge_b);
}
