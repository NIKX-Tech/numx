/**
 * @file test_poly.c
 * @brief Polynomial tests — float32 / x86 host build.
 *        numx_real_t = float  (NUMX_USE_DOUBLE not set)
 */

#include "unity.h"
#include "numx/numx_types.h"
#include "numx/poly.h"

#define TOL      1e-4f
#define TOL_ROOT 1e-3f

static numx_real_t priv_absf(numx_real_t x){ return x < 0.0f ? -x : x; }

static numx_real_t poly_at(const numx_real_t *c, numx_size_t deg, numx_real_t x)
{
    numx_real_t acc = c[0]; numx_size_t i;
    for (i = 1; i <= deg; i++) acc = acc*x + c[i];
    return acc;
}

void test_poly_eval_quadratic_at_3(void)
{
    numx_real_t c[] = {1.0f,0.0f,1.0f}, r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_poly_eval(c, 2, 3.0f, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 10.0f, r);
}

void test_poly_eval_cubic_at_2(void)
{
    numx_real_t c[] = {1.0f,0.0f,-2.0f,1.0f}, r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_poly_eval(c, 3, 2.0f, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 5.0f, r);
}

void test_poly_eval_at_zero(void)
{
    numx_real_t c[] = {3.0f,7.0f,-2.0f}, r;
    numx_poly_eval(c, 2, 0.0f, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL, -2.0f, r);
}

void test_poly_eval_at_one(void)
{
    numx_real_t c[] = {1.0f,-3.0f,2.0f}, r;
    numx_poly_eval(c, 2, 1.0f, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 0.0f, r);
}

void test_poly_eval_degree_0(void)
{
    numx_real_t c[] = {5.0f}, r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_poly_eval(c, 0, 99.0f, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 5.0f, r);
}

void test_poly_eval_horner_consistency(void)
{
    numx_real_t c[] = {2.0f,3.0f,4.0f}, r;
    numx_poly_eval(c, 2, 5.0f, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 69.0f, r);
}

void test_poly_eval_null_coeffs(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_poly_eval(NULL, 2, 1.0f, &r));
}

void test_poly_eval_null_result(void)
{
    numx_real_t c[] = {1.0f,0.0f,0.0f};
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_poly_eval(c, 2, 1.0f, NULL));
}

void test_poly_roots_linear(void)
{
    numx_real_t c[] = {1.0f,1.0f};
    numx_real_t roots[1]; numx_size_t nroots;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_poly_roots(c, 1, roots, &nroots, TOL_ROOT));
    TEST_ASSERT_EQUAL(1, (int)nroots);
    TEST_ASSERT_FLOAT_WITHIN(TOL, -1.0f, roots[0]);
}

void test_poly_roots_quadratic_two_reals(void)
{
    numx_real_t c[] = {1.0f,-5.0f,6.0f};
    numx_real_t roots[2]; numx_size_t nroots;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_poly_roots(c, 2, roots, &nroots, TOL_ROOT));
    TEST_ASSERT_EQUAL(2, (int)nroots);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 0.0f, poly_at(c, 2, roots[0]));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 0.0f, poly_at(c, 2, roots[1]));
}

void test_poly_roots_quadratic_root_values(void)
{
    numx_real_t c[] = {1.0f,-5.0f,6.0f};
    numx_real_t roots[2]; numx_size_t nroots;
    numx_real_t lo, hi, tmp;
    numx_poly_roots(c, 2, roots, &nroots, TOL_ROOT);
    lo = roots[0]; hi = roots[1];
    if (lo > hi) { tmp=lo; lo=hi; hi=tmp; }
    TEST_ASSERT_FLOAT_WITHIN(TOL, 2.0f, lo);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 3.0f, hi);
}

void test_poly_roots_cubic_residuals(void)
{
    numx_real_t c[] = {1.0f,-6.0f,11.0f,-6.0f};
    numx_real_t roots[3]; numx_size_t nroots, i;
    numx_poly_roots(c, 3, roots, &nroots, TOL_ROOT);
    for (i = 0; i < nroots; i++)
        TEST_ASSERT_TRUE(priv_absf(poly_at(c, 3, roots[i])) < TOL);
}

void test_poly_roots_single_real_found(void)
{
    numx_real_t c[] = {1.0f,0.0f,0.0f,-8.0f};
    numx_real_t roots[3]; numx_size_t nroots; int found = 0; numx_size_t i;
    numx_poly_roots(c, 3, roots, &nroots, TOL_ROOT);
    TEST_ASSERT_TRUE(nroots >= 1);
    for (i = 0; i < nroots; i++)
        if (priv_absf(roots[i]-2.0f) < TOL) found = 1;
    TEST_ASSERT_TRUE(found);
}

void test_poly_roots_null(void)
{
    numx_real_t c[] = {1.0f,0.0f,-1.0f};
    numx_real_t roots[2]; numx_size_t nroots;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
                      numx_poly_roots(NULL, 2, roots, &nroots, TOL_ROOT));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
                      numx_poly_roots(c, 2, NULL, &nroots, TOL_ROOT));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
                      numx_poly_roots(c, 2, roots, NULL, TOL_ROOT));
}

void test_poly_roots_degree_zero_rejected(void)
{
    numx_real_t c[] = {1.0f};
    numx_real_t roots[1]; numx_size_t nroots;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_poly_roots(c, 0, roots, &nroots, TOL_ROOT));
}

void test_poly_roots_tol_nonpositive(void)
{
    numx_real_t c[] = {1.0f,-1.0f};
    numx_real_t roots[1]; numx_size_t nroots;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_poly_roots(c, 1, roots, &nroots, 0.0f));
}

void numx_test_poly(void)
{
    RUN_TEST(test_poly_eval_quadratic_at_3);
    RUN_TEST(test_poly_eval_cubic_at_2);
    RUN_TEST(test_poly_eval_at_zero);
    RUN_TEST(test_poly_eval_at_one);
    RUN_TEST(test_poly_eval_degree_0);
    RUN_TEST(test_poly_eval_horner_consistency);
    RUN_TEST(test_poly_eval_null_coeffs);
    RUN_TEST(test_poly_eval_null_result);
    RUN_TEST(test_poly_roots_linear);
    RUN_TEST(test_poly_roots_quadratic_two_reals);
    RUN_TEST(test_poly_roots_quadratic_root_values);
    RUN_TEST(test_poly_roots_cubic_residuals);
    RUN_TEST(test_poly_roots_single_real_found);
    RUN_TEST(test_poly_roots_null);
    RUN_TEST(test_poly_roots_degree_zero_rejected);
    RUN_TEST(test_poly_roots_tol_nonpositive);
}
