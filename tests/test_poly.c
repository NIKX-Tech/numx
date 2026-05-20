/**
 * @file test_poly.c
 * @brief Unit tests for numx polynomial module.
 *
 * Coefficient convention: descending order — coeffs[0] is the leading
 * coefficient, coeffs[degree] is the constant term.
 *
 * Test levels:
 *   L1 - Known-answer tests: verified against analytical truth.
 *   L2 - Property tests: p(root) ≈ 0 for each found root.
 *   L3 - Edge cases: degree 1, degree 0 constant polynomial.
 *   L4 - Error handling: null pointers, invalid degree.
 */

#include "unity.h"
#include "numx/numx_types.h"
#include "numx/poly.h"

#define TOL 1e-4f
#define TOL_ROOT 1e-3f /* tolerance passed to root finder */

/* ════════════════════════════════════════════════════════════════════
 *  numx_poly_eval
 * ════════════════════════════════════════════════════════════════════ */

/* L1 */
void test_poly_eval_quadratic_at_3(void)
{
    /* p(x) = x^2 + 0*x + 1  ->  p(3) = 10 */
    numx_real_t c[] = {1.0f, 0.0f, 1.0f};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_poly_eval(c, 2, 3.0f, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 10.0f, r);
}

void test_poly_eval_cubic_at_2(void)
{
    /* p(x) = x^3 - 2x + 1  ->  p(2) = 8 - 4 + 1 = 5 */
    numx_real_t c[] = {1.0f, 0.0f, -2.0f, 1.0f};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_poly_eval(c, 3, 2.0f, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 5.0f, r);
}

void test_poly_eval_at_zero(void)
{
    /* p(0) = constant term = coeffs[degree] */
    numx_real_t c[] = {3.0f, 7.0f, -2.0f}; /* 3x^2 + 7x - 2 */
    numx_real_t r;
    numx_poly_eval(c, 2, 0.0f, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL, -2.0f, r);
}

void test_poly_eval_at_one(void)
{
    /* p(1) = sum of all coefficients */
    numx_real_t c[] = {1.0f, -3.0f, 2.0f}; /* x^2 - 3x + 2: p(1) = 0 */
    numx_real_t r;
    numx_poly_eval(c, 2, 1.0f, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 0.0f, r);
}

/* L2 */
void test_poly_eval_degree_0(void)
{
    /* degree 0 = constant */
    numx_real_t c[] = {5.0f};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_poly_eval(c, 0, 99.0f, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 5.0f, r);
}

void test_poly_eval_horner_consistency(void)
{
    /* manually verify Horner for p(x) = 2x^2 + 3x + 4 at x=5:
     * acc = 2, acc = 2*5+3 = 13, acc = 13*5+4 = 69 */
    numx_real_t c[] = {2.0f, 3.0f, 4.0f};
    numx_real_t r;
    numx_poly_eval(c, 2, 5.0f, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 69.0f, r);
}

/* L4 */
void test_poly_eval_null_coeffs(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_poly_eval(NULL, 2, 1.0f, &r));
}

void test_poly_eval_null_result(void)
{
    numx_real_t c[] = {1.0f, 0.0f, 0.0f};
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_poly_eval(c, 2, 1.0f, NULL));
}

/* ════════════════════════════════════════════════════════════════════
 *  numx_poly_roots
 * ════════════════════════════════════════════════════════════════════ */

static numx_real_t priv_absf(numx_real_t x) { return x < 0.0f ? -x : x; }

/* Evaluate p(x) for given coeffs/degree directly (Horner). */
static numx_real_t poly_at(const numx_real_t *c, numx_size_t deg, numx_real_t x)
{
    numx_real_t acc = c[0];
    numx_size_t i;
    for (i = 1; i <= deg; i++)
        acc = acc * x + c[i];
    return acc;
}

/* L1 */
void test_poly_roots_linear(void)
{
    /* p(x) = x + 1,  root = -1 */
    numx_real_t c[] = {1.0f, 1.0f};
    numx_real_t roots[1];
    numx_size_t nroots;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_poly_roots(c, 1, roots, &nroots, TOL_ROOT));
    TEST_ASSERT_EQUAL(1, (int)nroots);
    TEST_ASSERT_FLOAT_WITHIN(TOL, -1.0f, roots[0]);
}

void test_poly_roots_quadratic_two_reals(void)
{
    /* p(x) = x^2 - 5x + 6 = (x-2)(x-3) */
    numx_real_t c[] = {1.0f, -5.0f, 6.0f};
    numx_real_t roots[2];
    numx_size_t nroots;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_poly_roots(c, 2, roots, &nroots, TOL_ROOT));
    TEST_ASSERT_EQUAL(2, (int)nroots);
    /* Verify both roots satisfy p(root) ≈ 0 */
    TEST_ASSERT_FLOAT_WITHIN(TOL, 0.0f, poly_at(c, 2, roots[0]));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 0.0f, poly_at(c, 2, roots[1]));
}

void test_poly_roots_quadratic_root_values(void)
{
    /* sort the two roots and compare to {2, 3} */
    numx_real_t c[] = {1.0f, -5.0f, 6.0f};
    numx_real_t roots[2];
    numx_size_t nroots;
    numx_real_t lo, hi, tmp;
    numx_poly_roots(c, 2, roots, &nroots, TOL_ROOT);
    lo = roots[0];
    hi = roots[1];
    if (lo > hi)
    {
        tmp = lo;
        lo = hi;
        hi = tmp;
    }
    TEST_ASSERT_FLOAT_WITHIN(TOL, 2.0f, lo);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 3.0f, hi);
}

/* L2 — f(root) must vanish */
void test_poly_roots_cubic_residuals(void)
{
    /* p(x) = x^3 - 6x^2 + 11x - 6 = (x-1)(x-2)(x-3) */
    numx_real_t c[] = {1.0f, -6.0f, 11.0f, -6.0f};
    numx_real_t roots[3];
    numx_size_t nroots, i;
    numx_poly_roots(c, 3, roots, &nroots, TOL_ROOT);
    for (i = 0; i < nroots; i++)
    {
        TEST_ASSERT_TRUE(priv_absf(poly_at(c, 3, roots[i])) < TOL);
    }
}

/* L3 */
void test_poly_roots_single_real_found(void)
{
    /* Expect at least one real root from a cubic */
    numx_real_t c[] = {1.0f, 0.0f, 0.0f, -8.0f}; /* x^3 - 8: root at 2 */
    numx_real_t roots[3];
    numx_size_t nroots;
    numx_poly_roots(c, 3, roots, &nroots, TOL_ROOT);
    TEST_ASSERT_TRUE(nroots >= 1);
    /* The real root is x=2 */
    int found = 0;
    numx_size_t i;
    for (i = 0; i < nroots; i++)
    {
        if (priv_absf(roots[i] - 2.0f) < TOL)
            found = 1;
    }
    TEST_ASSERT_TRUE(found);
}

/* L4 */
void test_poly_roots_null(void)
{
    numx_real_t c[] = {1.0f, 0.0f, -1.0f};
    numx_real_t roots[2];
    numx_size_t nroots;
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
    numx_real_t roots[1];
    numx_size_t nroots;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_poly_roots(c, 0, roots, &nroots, TOL_ROOT));
}

void test_poly_roots_tol_nonpositive(void)
{
    numx_real_t c[] = {1.0f, -1.0f};
    numx_real_t roots[1];
    numx_size_t nroots;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_poly_roots(c, 1, roots, &nroots, 0.0f));
}

/* ════════════════════════════════════════════════════════════════════
 *  Suite entry point
 * ════════════════════════════════════════════════════════════════════ */

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
