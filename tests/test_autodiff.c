/**
 * @file test_autodiff.c
 * @brief Tests for numx autodiff — forward-mode (dual numbers) and
 *        reverse-mode (Wengert tape).
 *
 * Tolerance notes:
 *  TOL_TIGHT — exact arithmetic (add, sub, mul, neg, const, var)
 *  TOL       — transcendental ops (sin, cos, exp, log, sqrt) via
 *              Taylor/Newton private helpers
 */

#include "unity.h"
#include "numx/autodiff.h"

#define TOL       1e-3f
#define TOL_TIGHT 1e-5f

/* ════════════════════════════════════════════════════════════════════
 *  Forward-mode: numx_dual_t
 * ════════════════════════════════════════════════════════════════════ */

/* L1 — basic constructors */
void test_dual_const(void)
{
    numx_dual_t c = numx_dual_const(5.0f);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, 5.0f, c.re);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, 0.0f, c.du);
}

void test_dual_var(void)
{
    numx_dual_t v = numx_dual_var(3.0f);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, 3.0f, v.re);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, 1.0f, v.du);
}

/* L1 — arithmetic */
void test_dual_add(void)
{
    /* d/dx (x + 3) at x=2: primal=5, deriv=1 */
    numx_dual_t a = numx_dual_var(2.0f);
    numx_dual_t b = numx_dual_const(3.0f);
    numx_dual_t r = numx_dual_add(a, b);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, 5.0f, r.re);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, 1.0f, r.du);
}

void test_dual_sub(void)
{
    /* d/dx (x - 3) at x=5: primal=2, deriv=1 */
    numx_dual_t a = numx_dual_var(5.0f);
    numx_dual_t b = numx_dual_const(3.0f);
    numx_dual_t r = numx_dual_sub(a, b);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, 2.0f, r.re);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, 1.0f, r.du);
}

void test_dual_mul(void)
{
    /* d/dx x^2 at x=3: primal=9, deriv=6 */
    numx_dual_t a = numx_dual_var(3.0f);
    numx_dual_t r = numx_dual_mul(a, a);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, 9.0f, r.re);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, 6.0f, r.du);
}

void test_dual_div(void)
{
    /* d/dx (x/3) at x=6: primal=2, deriv=1/3 */
    numx_dual_t a = numx_dual_var(6.0f);
    numx_dual_t b = numx_dual_const(3.0f);
    numx_dual_t r = numx_dual_div(a, b);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, 2.0f,          r.re);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, 1.0f / 3.0f,   r.du);
}

void test_dual_neg(void)
{
    /* d/dx (-x) at x=3: primal=-3, deriv=-1 */
    numx_dual_t a = numx_dual_var(3.0f);
    numx_dual_t r = numx_dual_neg(a);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, -3.0f, r.re);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, -1.0f, r.du);
}

/* L1 — transcendentals */
void test_dual_sin(void)
{
    /* d/dx sin(x) at x=0: primal=0, deriv=cos(0)=1 */
    numx_dual_t a = numx_dual_var(0.0f);
    numx_dual_t r = numx_dual_sin(a);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 0.0f, r.re);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 1.0f, r.du);
}

void test_dual_cos(void)
{
    /* d/dx cos(x) at x=0: primal=1, deriv=-sin(0)=0 */
    numx_dual_t a = numx_dual_var(0.0f);
    numx_dual_t r = numx_dual_cos(a);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 1.0f, r.re);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 0.0f, r.du);
}

void test_dual_exp(void)
{
    /* d/dx exp(x) at x=0: primal=1, deriv=1 */
    numx_dual_t a = numx_dual_var(0.0f);
    numx_dual_t r = numx_dual_exp(a);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 1.0f, r.re);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 1.0f, r.du);
}

void test_dual_log(void)
{
    /* d/dx log(x) at x=1: primal=0, deriv=1 */
    numx_dual_t a = numx_dual_var(1.0f);
    numx_dual_t r = numx_dual_log(a);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 0.0f, r.re);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 1.0f, r.du);
}

void test_dual_sqrt(void)
{
    /* d/dx sqrt(x) at x=4: primal=2, deriv=1/(2*2)=0.25 */
    numx_dual_t a = numx_dual_var(4.0f);
    numx_dual_t r = numx_dual_sqrt(a);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 2.0f,  r.re);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 0.25f, r.du);
}

/* L2 — chain rule */
void test_dual_chain_quadratic(void)
{
    /* d/dx (x^2 + 5) at x=3: primal=14, deriv=6 */
    numx_dual_t x  = numx_dual_var(3.0f);
    numx_dual_t x2 = numx_dual_mul(x, x);
    numx_dual_t c  = numx_dual_const(5.0f);
    numx_dual_t r  = numx_dual_add(x2, c);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, 14.0f, r.re);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT,  6.0f, r.du);
}

/* L4 — error / boundary cases */
void test_dual_div_by_zero(void)
{
    numx_dual_t a = numx_dual_var(1.0f);
    numx_dual_t z = numx_dual_const(0.0f);
    numx_dual_t r = numx_dual_div(a, z);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, 0.0f, r.re);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, 0.0f, r.du);
}

void test_dual_log_nonpositive(void)
{
    numx_dual_t a = numx_dual_var(0.0f);
    numx_dual_t r = numx_dual_log(a);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, 0.0f, r.re);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, 0.0f, r.du);
}

void test_dual_sqrt_negative(void)
{
    numx_dual_t a = numx_dual_var(-4.0f);
    numx_dual_t r = numx_dual_sqrt(a);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, 0.0f, r.re);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, 0.0f, r.du);
}

/* ════════════════════════════════════════════════════════════════════
 *  Reverse-mode: Wengert tape
 * ════════════════════════════════════════════════════════════════════ */

/* L1 — tape management */
void test_ad_init(void)
{
    numx_ad_tape_t tape;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ad_init(&tape));
    TEST_ASSERT_EQUAL_UINT32(0, tape.len);
}

void test_ad_var(void)
{
    numx_ad_tape_t tape;
    numx_size_t    ix;
    numx_ad_init(&tape);
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ad_var(&tape, 5.0f, &ix));
    TEST_ASSERT_EQUAL_UINT32(0, ix);
    TEST_ASSERT_EQUAL_UINT32(1, tape.len);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, 5.0f, numx_ad_val(&tape, ix));
}

/* L1 — backward: arithmetic */
void test_ad_add_backward(void)
{
    /* z = x + y; dz/dx = 1, dz/dy = 1 */
    numx_ad_tape_t tape;
    numx_size_t xi, yi, zi;
    numx_ad_init(&tape);
    numx_ad_var(&tape, 3.0f, &xi);
    numx_ad_var(&tape, 4.0f, &yi);
    numx_ad_add(&tape, xi, yi, &zi);
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ad_backward(&tape, zi));
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, 7.0f, numx_ad_val(&tape, zi));
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, 1.0f, numx_ad_grad(&tape, xi));
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, 1.0f, numx_ad_grad(&tape, yi));
}

void test_ad_sub_backward(void)
{
    /* z = x - y; dz/dx = 1, dz/dy = -1 */
    numx_ad_tape_t tape;
    numx_size_t xi, yi, zi;
    numx_ad_init(&tape);
    numx_ad_var(&tape, 5.0f, &xi);
    numx_ad_var(&tape, 2.0f, &yi);
    numx_ad_sub(&tape, xi, yi, &zi);
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ad_backward(&tape, zi));
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT,  1.0f, numx_ad_grad(&tape, xi));
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, -1.0f, numx_ad_grad(&tape, yi));
}

void test_ad_mul_backward(void)
{
    /* z = x*y at x=3, y=4; dz/dx=4, dz/dy=3 */
    numx_ad_tape_t tape;
    numx_size_t xi, yi, zi;
    numx_ad_init(&tape);
    numx_ad_var(&tape, 3.0f, &xi);
    numx_ad_var(&tape, 4.0f, &yi);
    numx_ad_mul(&tape, xi, yi, &zi);
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ad_backward(&tape, zi));
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, 12.0f, numx_ad_val(&tape, zi));
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT,  4.0f, numx_ad_grad(&tape, xi));
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT,  3.0f, numx_ad_grad(&tape, yi));
}

void test_ad_div_backward(void)
{
    /* z = x/y at x=6, y=2; dz/dx=0.5, dz/dy=-1.5 */
    numx_ad_tape_t tape;
    numx_size_t xi, yi, zi;
    numx_ad_init(&tape);
    numx_ad_var(&tape, 6.0f, &xi);
    numx_ad_var(&tape, 2.0f, &yi);
    numx_ad_div(&tape, xi, yi, &zi);
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ad_backward(&tape, zi));
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT,  0.5f, numx_ad_grad(&tape, xi));
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, -1.5f, numx_ad_grad(&tape, yi));
}

void test_ad_neg_backward(void)
{
    /* z = -x at x=5; dz/dx = -1 */
    numx_ad_tape_t tape;
    numx_size_t xi, zi;
    numx_ad_init(&tape);
    numx_ad_var(&tape, 5.0f, &xi);
    numx_ad_neg(&tape, xi, &zi);
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ad_backward(&tape, zi));
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, -5.0f, numx_ad_val(&tape, zi));
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, -1.0f, numx_ad_grad(&tape, xi));
}

/* L1 — backward: transcendentals */
void test_ad_sin_backward(void)
{
    /* z = sin(x) at x=0; dz/dx = cos(0) = 1 */
    numx_ad_tape_t tape;
    numx_size_t xi, zi;
    numx_ad_init(&tape);
    numx_ad_var(&tape, 0.0f, &xi);
    numx_ad_sin(&tape, xi, &zi);
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ad_backward(&tape, zi));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 1.0f, numx_ad_grad(&tape, xi));
}

void test_ad_cos_backward(void)
{
    /* z = cos(x) at x=0; dz/dx = -sin(0) = 0 */
    numx_ad_tape_t tape;
    numx_size_t xi, zi;
    numx_ad_init(&tape);
    numx_ad_var(&tape, 0.0f, &xi);
    numx_ad_cos(&tape, xi, &zi);
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ad_backward(&tape, zi));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 0.0f, numx_ad_grad(&tape, xi));
}

void test_ad_exp_backward(void)
{
    /* z = exp(x) at x=0; dz/dx = exp(0) = 1 */
    numx_ad_tape_t tape;
    numx_size_t xi, zi;
    numx_ad_init(&tape);
    numx_ad_var(&tape, 0.0f, &xi);
    numx_ad_exp(&tape, xi, &zi);
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ad_backward(&tape, zi));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 1.0f, numx_ad_val(&tape, zi));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 1.0f, numx_ad_grad(&tape, xi));
}

void test_ad_log_backward(void)
{
    /* z = log(x) at x=1; dz/dx = 1/1 = 1 */
    numx_ad_tape_t tape;
    numx_size_t xi, zi;
    numx_ad_init(&tape);
    numx_ad_var(&tape, 1.0f, &xi);
    numx_ad_log(&tape, xi, &zi);
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ad_backward(&tape, zi));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 0.0f, numx_ad_val(&tape, zi));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 1.0f, numx_ad_grad(&tape, xi));
}

void test_ad_sqrt_backward(void)
{
    /* z = sqrt(x) at x=4; dz/dx = 1/(2*2) = 0.25 */
    numx_ad_tape_t tape;
    numx_size_t xi, zi;
    numx_ad_init(&tape);
    numx_ad_var(&tape, 4.0f, &xi);
    numx_ad_sqrt(&tape, xi, &zi);
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ad_backward(&tape, zi));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 2.0f,  numx_ad_val(&tape, zi));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 0.25f, numx_ad_grad(&tape, xi));
}

/* L2 — multi-variable gradient */
void test_ad_quadratic(void)
{
    /* z = x^2 + y^2 at x=3, y=4; dz/dx=6, dz/dy=8 */
    numx_ad_tape_t tape;
    numx_size_t xi, yi, x2i, y2i, zi;
    numx_ad_init(&tape);
    numx_ad_var(&tape, 3.0f, &xi);
    numx_ad_var(&tape, 4.0f, &yi);
    numx_ad_mul(&tape, xi, xi, &x2i);
    numx_ad_mul(&tape, yi, yi, &y2i);
    numx_ad_add(&tape, x2i, y2i, &zi);
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ad_backward(&tape, zi));
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, 25.0f, numx_ad_val(&tape, zi));
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT,  6.0f, numx_ad_grad(&tape, xi));
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT,  8.0f, numx_ad_grad(&tape, yi));
}

/* L4 — error cases */
void test_ad_null_returns_error(void)
{
    numx_ad_tape_t tape;
    numx_size_t ix;
    numx_ad_init(&tape);
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_ad_init(NULL));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_ad_var(NULL, 1.0f, &ix));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_ad_var(&tape, 1.0f, NULL));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_ad_backward(NULL, 0));
}

void test_ad_invalid_idx_returns_error(void)
{
    numx_ad_tape_t tape;
    numx_size_t xi, zi;
    numx_ad_init(&tape);
    numx_ad_var(&tape, 1.0f, &xi);   /* tape.len == 1, valid idx = 0 */
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_ad_backward(&tape, 999));
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_ad_add(&tape, 0, 999, &zi));
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_ad_neg(&tape, 999, &zi));
}

void test_ad_div_by_zero_returns_error(void)
{
    numx_ad_tape_t tape;
    numx_size_t xi, yi, zi;
    numx_ad_init(&tape);
    numx_ad_var(&tape, 6.0f, &xi);
    numx_ad_var(&tape, 0.0f, &yi);
    TEST_ASSERT_EQUAL(NUMX_ERR_SINGULAR, numx_ad_div(&tape, xi, yi, &zi));
}

void test_ad_log_nonpositive_returns_error(void)
{
    numx_ad_tape_t tape;
    numx_size_t xi, zi;
    numx_ad_init(&tape);
    numx_ad_var(&tape, 0.0f, &xi);
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_ad_log(&tape, xi, &zi));
}

void test_ad_sqrt_negative_returns_error(void)
{
    numx_ad_tape_t tape;
    numx_size_t xi, zi;
    numx_ad_init(&tape);
    numx_ad_var(&tape, -1.0f, &xi);
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_ad_sqrt(&tape, xi, &zi));
}

/* ════════════════════════════════════════════════════════════════════
 *  Suite entry point
 * ════════════════════════════════════════════════════════════════════ */

void numx_test_autodiff(void)
{
    /* Forward-mode */
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
    /* Reverse-mode */
    RUN_TEST(test_ad_init);
    RUN_TEST(test_ad_var);
    RUN_TEST(test_ad_add_backward);
    RUN_TEST(test_ad_sub_backward);
    RUN_TEST(test_ad_mul_backward);
    RUN_TEST(test_ad_div_backward);
    RUN_TEST(test_ad_neg_backward);
    RUN_TEST(test_ad_sin_backward);
    RUN_TEST(test_ad_cos_backward);
    RUN_TEST(test_ad_exp_backward);
    RUN_TEST(test_ad_log_backward);
    RUN_TEST(test_ad_sqrt_backward);
    RUN_TEST(test_ad_quadratic);
    RUN_TEST(test_ad_null_returns_error);
    RUN_TEST(test_ad_invalid_idx_returns_error);
    RUN_TEST(test_ad_div_by_zero_returns_error);
    RUN_TEST(test_ad_log_nonpositive_returns_error);
    RUN_TEST(test_ad_sqrt_negative_returns_error);
}
