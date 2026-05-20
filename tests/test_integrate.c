/**
 * @file test_integrate.c
 * @brief Unit tests for numx numerical integration module.
 *
 * Test levels:
 *   L1 - Known-answer tests: verified against analytical truth.
 *   L2 - Property tests: linearity of the integral, symmetry.
 *   L3 - Edge cases: minimum n, single interval.
 *   L4 - Error handling: null pointers, invalid args.
 */

#include "unity.h"
#include "numx/numx_types.h"
#include "numx/integrate.h"

#define TOL_TRAP 1e-3f  /* trapezoid O(h^2) with n=100 */
#define TOL_SIMP 1e-5f  /* Simpson exact for degree<=3 polynomials */
#define TOL_GAUSS 1e-5f /* Gauss-8 exact for degree<=15 polynomials */

/* ── Test functions ────────────────────────────────────────────────── */

static numx_real_t f_one(numx_real_t x)
{
    (void)x;
    return (numx_real_t)1.0;
}
static numx_real_t f_linear(numx_real_t x) { return x; }
static numx_real_t f_quad(numx_real_t x) { return x * x; }
static numx_real_t f_cubic(numx_real_t x) { return x * x * x; }
static numx_real_t f_neg(numx_real_t x) { return -x; }

/* ════════════════════════════════════════════════════════════════════
 *  numx_integrate_trap
 * ════════════════════════════════════════════════════════════════════ */

/* L1 */
void test_trap_constant_one(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_integrate_trap(f_one, 0.0f, 1.0f, 100, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL_TRAP, 1.0f, r);
}

void test_trap_linear(void)
{
    /* integral of x from 0 to 1 = 0.5 */
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_integrate_trap(f_linear, 0.0f, 1.0f, 100, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL_TRAP, 0.5f, r);
}

void test_trap_quadratic(void)
{
    /* integral of x^2 from 0 to 1 = 1/3 */
    numx_real_t r;
    numx_integrate_trap(f_quad, 0.0f, 1.0f, 1000, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TRAP, 1.0f / 3.0f, r);
}

/* L2 */
void test_trap_linearity(void)
{
    /* integral(-x) = -integral(x) */
    numx_real_t r1, r2;
    numx_integrate_trap(f_linear, 0.0f, 2.0f, 100, &r1);
    numx_integrate_trap(f_neg, 0.0f, 2.0f, 100, &r2);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TRAP, r1, -r2);
}

/* L3 */
void test_trap_n1(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_integrate_trap(f_one, 0.0f, 1.0f, 1, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL_TRAP, 1.0f, r);
}

/* L4 */
void test_trap_null(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_integrate_trap(NULL, 0.0f, 1.0f, 10, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_integrate_trap(f_one, 0.0f, 1.0f, 10, NULL));
}

void test_trap_a_ge_b(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_integrate_trap(f_one, 1.0f, 0.0f, 10, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_integrate_trap(f_one, 1.0f, 1.0f, 10, &r));
}

void test_trap_n_zero(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_integrate_trap(f_one, 0.0f, 1.0f, 0, &r));
}

/* ════════════════════════════════════════════════════════════════════
 *  numx_integrate_simpson
 * ════════════════════════════════════════════════════════════════════ */

/* L1 */
void test_simpson_constant_one(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_integrate_simpson(f_one, 0.0f, 1.0f, 2, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL_SIMP, 1.0f, r);
}

void test_simpson_linear_exact(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_integrate_simpson(f_linear, 0.0f, 1.0f, 2, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL_SIMP, 0.5f, r);
}

void test_simpson_quadratic_exact(void)
{
    /* Simpson is exact for degree <= 3; x^2 -> integral = 1/3 */
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_integrate_simpson(f_quad, 0.0f, 1.0f, 4, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL_SIMP, 1.0f / 3.0f, r);
}

void test_simpson_cubic_exact(void)
{
    /* integral of x^3 from 0 to 1 = 0.25 */
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_integrate_simpson(f_cubic, 0.0f, 1.0f, 4, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL_SIMP, 0.25f, r);
}

/* L2 */
void test_simpson_wider_interval(void)
{
    /* integral of x^2 from 0 to 3 = 9 */
    numx_real_t r;
    numx_integrate_simpson(f_quad, 0.0f, 3.0f, 6, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL_SIMP, 9.0f, r);
}

/* L4 */
void test_simpson_odd_n_rejected(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_integrate_simpson(f_one, 0.0f, 1.0f, 3, &r));
}

void test_simpson_n_lt_2_rejected(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_integrate_simpson(f_one, 0.0f, 1.0f, 0, &r));
}

void test_simpson_null(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
                      numx_integrate_simpson(NULL, 0.0f, 1.0f, 4, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
                      numx_integrate_simpson(f_one, 0.0f, 1.0f, 4, NULL));
}

/* ════════════════════════════════════════════════════════════════════
 *  numx_integrate_gauss
 * ════════════════════════════════════════════════════════════════════ */

/* L1 */
void test_gauss2_linear(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_integrate_gauss(f_linear, 0.0f, 1.0f, 2, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL_GAUSS, 0.5f, r);
}

void test_gauss4_quadratic_exact(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_integrate_gauss(f_quad, 0.0f, 1.0f, 4, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL_GAUSS, 1.0f / 3.0f, r);
}

void test_gauss8_cubic_exact(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_integrate_gauss(f_cubic, 0.0f, 1.0f, 8, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL_GAUSS, 0.25f, r);
}

void test_gauss8_constant_exact(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_integrate_gauss(f_one, -3.0f, 3.0f, 8, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL_GAUSS, 6.0f, r);
}

/* L4 */
void test_gauss_null(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
                      numx_integrate_gauss(NULL, 0.0f, 1.0f, 4, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
                      numx_integrate_gauss(f_one, 0.0f, 1.0f, 4, NULL));
}

void test_gauss_invalid_npts(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_integrate_gauss(f_one, 0.0f, 1.0f, 3, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_integrate_gauss(f_one, 0.0f, 1.0f, 16, &r));
}

void test_gauss_a_ge_b(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_integrate_gauss(f_one, 1.0f, 0.0f, 4, &r));
}

/* ════════════════════════════════════════════════════════════════════
 *  Suite entry point
 * ════════════════════════════════════════════════════════════════════ */

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
