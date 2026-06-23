/**
 * @file test_autodiff.c
 * @brief Autodiff tests — float64 / x64 host build.
 *        numx_real_t = double  (compiled with -DNUMX_USE_DOUBLE)
 */

#include "unity.h"
#include "numx/autodiff.h"

#define TOL       1e-6
#define TOL_TIGHT 1e-10

/* ── Forward-mode ───────────────────────────────────────────────────── */

void test_dual_const(void)
{
    numx_dual_t c = numx_dual_const(5.0);
    TEST_ASSERT_DOUBLE_WITHIN(TOL_TIGHT, 5.0, c.re);
    TEST_ASSERT_DOUBLE_WITHIN(TOL_TIGHT, 0.0, c.du);
}

void test_dual_var(void)
{
    numx_dual_t v = numx_dual_var(3.0);
    TEST_ASSERT_DOUBLE_WITHIN(TOL_TIGHT, 3.0, v.re);
    TEST_ASSERT_DOUBLE_WITHIN(TOL_TIGHT, 1.0, v.du);
}

void test_dual_add(void)
{
    numx_dual_t r = numx_dual_add(numx_dual_var(2.0), numx_dual_const(3.0));
    TEST_ASSERT_DOUBLE_WITHIN(TOL_TIGHT, 5.0, r.re);
    TEST_ASSERT_DOUBLE_WITHIN(TOL_TIGHT, 1.0, r.du);
}

void test_dual_sub(void)
{
    numx_dual_t r = numx_dual_sub(numx_dual_var(5.0), numx_dual_const(3.0));
    TEST_ASSERT_DOUBLE_WITHIN(TOL_TIGHT, 2.0, r.re);
    TEST_ASSERT_DOUBLE_WITHIN(TOL_TIGHT, 1.0, r.du);
}

void test_dual_mul(void)
{
    numx_dual_t a = numx_dual_var(3.0);
    numx_dual_t r = numx_dual_mul(a, a);
    TEST_ASSERT_DOUBLE_WITHIN(TOL_TIGHT, 9.0, r.re);
    TEST_ASSERT_DOUBLE_WITHIN(TOL_TIGHT, 6.0, r.du);
}

void test_dual_div(void)
{
    numx_dual_t r = numx_dual_div(numx_dual_var(6.0), numx_dual_const(3.0));
    TEST_ASSERT_DOUBLE_WITHIN(TOL_TIGHT, 2.0,       r.re);
    TEST_ASSERT_DOUBLE_WITHIN(TOL_TIGHT, 1.0/3.0,   r.du);
}

void test_dual_neg(void)
{
    numx_dual_t r = numx_dual_neg(numx_dual_var(3.0));
    TEST_ASSERT_DOUBLE_WITHIN(TOL_TIGHT, -3.0, r.re);
    TEST_ASSERT_DOUBLE_WITHIN(TOL_TIGHT, -1.0, r.du);
}

void test_dual_sin(void)
{
    numx_dual_t r = numx_dual_sin(numx_dual_var(0.0));
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 0.0, r.re);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 1.0, r.du);
}

void test_dual_cos(void)
{
    numx_dual_t r = numx_dual_cos(numx_dual_var(0.0));
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 1.0, r.re);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 0.0, r.du);
}

void test_dual_exp(void)
{
    numx_dual_t r = numx_dual_exp(numx_dual_var(0.0));
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 1.0, r.re);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 1.0, r.du);
}

void test_dual_log(void)
{
    numx_dual_t r = numx_dual_log(numx_dual_var(1.0));
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 0.0, r.re);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 1.0, r.du);
}

void test_dual_sqrt(void)
{
    numx_dual_t r = numx_dual_sqrt(numx_dual_var(4.0));
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 2.0,  r.re);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 0.25, r.du);
}

void test_dual_chain_quadratic(void)
{
    numx_dual_t x  = numx_dual_var(3.0);
    numx_dual_t r  = numx_dual_add(numx_dual_mul(x, x), numx_dual_const(5.0));
    TEST_ASSERT_DOUBLE_WITHIN(TOL_TIGHT, 14.0, r.re);
    TEST_ASSERT_DOUBLE_WITHIN(TOL_TIGHT,  6.0, r.du);
}

void test_dual_div_by_zero(void)
{
    numx_dual_t r = numx_dual_div(numx_dual_var(1.0), numx_dual_const(0.0));
    TEST_ASSERT_DOUBLE_WITHIN(TOL_TIGHT, 0.0, r.re);
    TEST_ASSERT_DOUBLE_WITHIN(TOL_TIGHT, 0.0, r.du);
}

void test_dual_log_nonpositive(void)
{
    numx_dual_t r = numx_dual_log(numx_dual_var(0.0));
    TEST_ASSERT_DOUBLE_WITHIN(TOL_TIGHT, 0.0, r.re);
    TEST_ASSERT_DOUBLE_WITHIN(TOL_TIGHT, 0.0, r.du);
}

void test_dual_sqrt_negative(void)
{
    numx_dual_t r = numx_dual_sqrt(numx_dual_var(-4.0));
    TEST_ASSERT_DOUBLE_WITHIN(TOL_TIGHT, 0.0, r.re);
    TEST_ASSERT_DOUBLE_WITHIN(TOL_TIGHT, 0.0, r.du);
}

/* ── Reverse-mode ───────────────────────────────────────────────────── */

void test_ad_init(void)
{
    numx_ad_tape_t tape;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ad_init(&tape));
    TEST_ASSERT_EQUAL_UINT32(0, tape.len);
}

void test_ad_var(void)
{
    numx_ad_tape_t tape; numx_size_t ix;
    numx_ad_init(&tape);
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ad_var(&tape, 5.0, &ix));
    TEST_ASSERT_EQUAL_UINT32(0, ix);
    TEST_ASSERT_EQUAL_UINT32(1, tape.len);
    TEST_ASSERT_DOUBLE_WITHIN(TOL_TIGHT, 5.0, numx_ad_val(&tape, ix));
}

void test_ad_add_backward(void)
{
    numx_ad_tape_t tape; numx_size_t xi, yi, zi;
    numx_ad_init(&tape);
    numx_ad_var(&tape, 3.0, &xi);
    numx_ad_var(&tape, 4.0, &yi);
    numx_ad_add(&tape, xi, yi, &zi);
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ad_backward(&tape, zi));
    TEST_ASSERT_DOUBLE_WITHIN(TOL_TIGHT, 7.0, numx_ad_val(&tape, zi));
    TEST_ASSERT_DOUBLE_WITHIN(TOL_TIGHT, 1.0, numx_ad_grad(&tape, xi));
    TEST_ASSERT_DOUBLE_WITHIN(TOL_TIGHT, 1.0, numx_ad_grad(&tape, yi));
}

void test_ad_mul_backward(void)
{
    numx_ad_tape_t tape; numx_size_t xi, yi, zi;
    numx_ad_init(&tape);
    numx_ad_var(&tape, 3.0, &xi);
    numx_ad_var(&tape, 4.0, &yi);
    numx_ad_mul(&tape, xi, yi, &zi);
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ad_backward(&tape, zi));
    TEST_ASSERT_DOUBLE_WITHIN(TOL_TIGHT, 12.0, numx_ad_val(&tape, zi));
    TEST_ASSERT_DOUBLE_WITHIN(TOL_TIGHT,  4.0, numx_ad_grad(&tape, xi));
    TEST_ASSERT_DOUBLE_WITHIN(TOL_TIGHT,  3.0, numx_ad_grad(&tape, yi));
}

void test_ad_div_backward(void)
{
    numx_ad_tape_t tape; numx_size_t xi, yi, zi;
    numx_ad_init(&tape);
    numx_ad_var(&tape, 6.0, &xi);
    numx_ad_var(&tape, 2.0, &yi);
    numx_ad_div(&tape, xi, yi, &zi);
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ad_backward(&tape, zi));
    TEST_ASSERT_DOUBLE_WITHIN(TOL_TIGHT,  0.5, numx_ad_grad(&tape, xi));
    TEST_ASSERT_DOUBLE_WITHIN(TOL_TIGHT, -1.5, numx_ad_grad(&tape, yi));
}

void test_ad_sin_backward(void)
{
    numx_ad_tape_t tape; numx_size_t xi, zi;
    numx_ad_init(&tape);
    numx_ad_var(&tape, 0.0, &xi);
    numx_ad_sin(&tape, xi, &zi);
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ad_backward(&tape, zi));
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 1.0, numx_ad_grad(&tape, xi));
}

void test_ad_quadratic(void)
{
    numx_ad_tape_t tape; numx_size_t xi, yi, x2i, y2i, zi;
    numx_ad_init(&tape);
    numx_ad_var(&tape, 3.0, &xi);
    numx_ad_var(&tape, 4.0, &yi);
    numx_ad_mul(&tape, xi, xi, &x2i);
    numx_ad_mul(&tape, yi, yi, &y2i);
    numx_ad_add(&tape, x2i, y2i, &zi);
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ad_backward(&tape, zi));
    TEST_ASSERT_DOUBLE_WITHIN(TOL_TIGHT, 25.0, numx_ad_val(&tape, zi));
    TEST_ASSERT_DOUBLE_WITHIN(TOL_TIGHT,  6.0, numx_ad_grad(&tape, xi));
    TEST_ASSERT_DOUBLE_WITHIN(TOL_TIGHT,  8.0, numx_ad_grad(&tape, yi));
}

void test_ad_null_returns_error(void)
{
    numx_ad_tape_t tape; numx_size_t ix;
    numx_ad_init(&tape);
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_ad_init(NULL));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_ad_var(NULL, 1.0, &ix));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_ad_var(&tape, 1.0, NULL));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_ad_backward(NULL, 0));
}

void test_ad_invalid_idx_returns_error(void)
{
    numx_ad_tape_t tape; numx_size_t xi, zi;
    numx_ad_init(&tape);
    numx_ad_var(&tape, 1.0, &xi);
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_ad_backward(&tape, 999));
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_ad_add(&tape, 0, 999, &zi));
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_ad_neg(&tape, 999, &zi));
}

void test_ad_div_by_zero_returns_error(void)
{
    numx_ad_tape_t tape; numx_size_t xi, yi, zi;
    numx_ad_init(&tape);
    numx_ad_var(&tape, 6.0, &xi);
    numx_ad_var(&tape, 0.0, &yi);
    TEST_ASSERT_EQUAL(NUMX_ERR_SINGULAR, numx_ad_div(&tape, xi, yi, &zi));
}

void test_ad_log_nonpositive_returns_error(void)
{
    numx_ad_tape_t tape; numx_size_t xi, zi;
    numx_ad_init(&tape);
    numx_ad_var(&tape, 0.0, &xi);
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_ad_log(&tape, xi, &zi));
}

void test_ad_sqrt_negative_returns_error(void)
{
    numx_ad_tape_t tape; numx_size_t xi, zi;
    numx_ad_init(&tape);
    numx_ad_var(&tape, -1.0, &xi);
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_ad_sqrt(&tape, xi, &zi));
}

void numx_test_autodiff(void)
{
    RUN_TEST(test_dual_const);
    RUN_TEST(test_dual_var);
    RUN_TEST(test_dual_add);
    RUN_TEST(test_dual_sub);
    RUN_TEST(test_dual_mul);
    RUN_TEST(test_dual_div);
    RUN_TEST(test_dual_neg);
    RUN_TEST(test_dual_sin);
    RUN_TEST(test_dual_cos);
    RUN_TEST(test_dual_exp);
    RUN_TEST(test_dual_log);
    RUN_TEST(test_dual_sqrt);
    RUN_TEST(test_dual_chain_quadratic);
    RUN_TEST(test_dual_div_by_zero);
    RUN_TEST(test_dual_log_nonpositive);
    RUN_TEST(test_dual_sqrt_negative);
    RUN_TEST(test_ad_init);
    RUN_TEST(test_ad_var);
    RUN_TEST(test_ad_add_backward);
    RUN_TEST(test_ad_mul_backward);
    RUN_TEST(test_ad_div_backward);
    RUN_TEST(test_ad_sin_backward);
    RUN_TEST(test_ad_quadratic);
    RUN_TEST(test_ad_null_returns_error);
    RUN_TEST(test_ad_invalid_idx_returns_error);
    RUN_TEST(test_ad_div_by_zero_returns_error);
    RUN_TEST(test_ad_log_nonpositive_returns_error);
    RUN_TEST(test_ad_sqrt_negative_returns_error);
}
