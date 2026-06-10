/**
 * @file test_ode.c
 * @brief ODE solver tests — float32 / x86 host build.
 *        numx_real_t = float  (NUMX_USE_DOUBLE not set)
 */

#include "unity.h"
#include "numx/numx_types.h"
#include "numx/ode.h"

#define TOL_RK4  1e-3f
#define TOL_RK45 1e-3f
#define RK45_TOL 1e-4f

static numx_status_t ode_growth(numx_real_t t, const numx_real_t *y,
                                numx_size_t n, numx_real_t *dydt)
{
    numx_size_t i; (void)t;
    for (i = 0; i < n; i++) dydt[i] = y[i];
    return NUMX_OK;
}

static numx_status_t ode_decay(numx_real_t t, const numx_real_t *y,
                               numx_size_t n, numx_real_t *dydt)
{
    numx_size_t i; (void)t;
    for (i = 0; i < n; i++) dydt[i] = -y[i];
    return NUMX_OK;
}

static numx_status_t ode_harmonic(numx_real_t t, const numx_real_t *y,
                                  numx_size_t n, numx_real_t *dydt)
{
    (void)t; (void)n;
    dydt[0] =  y[1];
    dydt[1] = -y[0];
    return NUMX_OK;
}

static numx_status_t ode_error(numx_real_t t, const numx_real_t *y,
                               numx_size_t n, numx_real_t *dydt)
{
    (void)t; (void)y; (void)n; (void)dydt;
    return NUMX_ERR_SINGULAR;
}

void test_rk4_growth_at_t1(void)
{
    numx_real_t y[] = {1.0f};
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ode_rk4(ode_growth, 0.0f, y, 1, 1e-3f, 1000, y));
    TEST_ASSERT_FLOAT_WITHIN(TOL_RK4, 2.71828f, y[0]);
}

void test_rk4_decay_at_t1(void)
{
    numx_real_t y[] = {1.0f};
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ode_rk4(ode_decay, 0.0f, y, 1, 1e-3f, 1000, y));
    TEST_ASSERT_FLOAT_WITHIN(TOL_RK4, 0.36788f, y[0]);
}

void test_rk4_growth_at_t2(void)
{
    numx_real_t y[] = {1.0f};
    numx_ode_rk4(ode_growth, 0.0f, y, 1, 1e-3f, 2000, y);
    TEST_ASSERT_FLOAT_WITHIN(TOL_RK4, 7.38906f, y[0]);
}

void test_rk4_harmonic_energy_conserved(void)
{
    numx_real_t y[] = {1.0f, 0.0f};
    numx_real_t E_init  = 0.5f*(y[0]*y[0]+y[1]*y[1]);
    numx_ode_rk4(ode_harmonic, 0.0f, y, 2, 1e-3f, 1000, y);
    numx_real_t E_final = 0.5f*(y[0]*y[0]+y[1]*y[1]);
    numx_real_t err = E_final-E_init;
    if (err < 0.0f) err = -err;
    TEST_ASSERT_TRUE(err < 1e-2f);
}

void test_rk4_single_step(void)
{
    numx_real_t y[] = {1.0f};
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ode_rk4(ode_growth, 0.0f, y, 1, 0.01f, 1, y));
    TEST_ASSERT_FLOAT_WITHIN(1e-4f, 1.01005f, y[0]);
}

void test_rk4_propagates_rhs_error(void)
{
    numx_real_t y[] = {1.0f};
    TEST_ASSERT_EQUAL(NUMX_ERR_SINGULAR,
                      numx_ode_rk4(ode_error, 0.0f, y, 1, 0.1f, 10, y));
}

void test_rk4_null(void)
{
    numx_real_t y0[] = {1.0f}, y[1];
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
                      numx_ode_rk4(NULL, 0.0f, y0, 1, 0.1f, 10, y));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
                      numx_ode_rk4(ode_growth, 0.0f, NULL, 1, 0.1f, 10, y));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
                      numx_ode_rk4(ode_growth, 0.0f, y0, 1, 0.1f, 10, NULL));
}

void test_rk4_h_nonpositive(void)
{
    numx_real_t y0[] = {1.0f}, y[1];
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_ode_rk4(ode_growth, 0.0f, y0, 1, 0.0f, 10, y));
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_ode_rk4(ode_growth, 0.0f, y0, 1, -1.0f, 10, y));
}

void test_rk4_steps_zero(void)
{
    numx_real_t y0[] = {1.0f}, y[1];
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_ode_rk4(ode_growth, 0.0f, y0, 1, 0.1f, 0, y));
}

void test_rk4_n_zero(void)
{
    numx_real_t y0[] = {1.0f}, y[1];
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_ode_rk4(ode_growth, 0.0f, y0, 0, 0.1f, 10, y));
}

void test_rk45_growth_at_t1(void)
{
    numx_real_t y[] = {1.0f};
    TEST_ASSERT_EQUAL(NUMX_OK,
                      numx_ode_rk45(ode_growth, 0.0f, 1.0f, y, 1, RK45_TOL, y));
    TEST_ASSERT_FLOAT_WITHIN(TOL_RK45, 2.71828f, y[0]);
}

void test_rk45_decay_at_t1(void)
{
    numx_real_t y[] = {1.0f};
    TEST_ASSERT_EQUAL(NUMX_OK,
                      numx_ode_rk45(ode_decay, 0.0f, 1.0f, y, 1, RK45_TOL, y));
    TEST_ASSERT_FLOAT_WITHIN(TOL_RK45, 0.36788f, y[0]);
}

void test_rk45_harmonic_energy_conserved(void)
{
    numx_real_t y[] = {1.0f, 0.0f};
    numx_real_t E_init  = 0.5f*(y[0]*y[0]+y[1]*y[1]);
    numx_ode_rk45(ode_harmonic, 0.0f, 10.0f, y, 2, RK45_TOL, y);
    numx_real_t E_final = 0.5f*(y[0]*y[0]+y[1]*y[1]);
    numx_real_t err = E_final-E_init;
    if (err < 0.0f) err = -err;
    TEST_ASSERT_TRUE(err < 1e-2f);
}

void test_rk45_propagates_rhs_error(void)
{
    numx_real_t y[] = {1.0f};
    TEST_ASSERT_EQUAL(NUMX_ERR_SINGULAR,
                      numx_ode_rk45(ode_error, 0.0f, 1.0f, y, 1, RK45_TOL, y));
}

void test_rk45_null(void)
{
    numx_real_t y0[] = {1.0f}, y[1];
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
                      numx_ode_rk45(NULL, 0.0f, 1.0f, y0, 1, RK45_TOL, y));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
                      numx_ode_rk45(ode_growth, 0.0f, 1.0f, NULL, 1, RK45_TOL, y));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
                      numx_ode_rk45(ode_growth, 0.0f, 1.0f, y0, 1, RK45_TOL, NULL));
}

void test_rk45_t1_le_t0(void)
{
    numx_real_t y0[] = {1.0f}, y[1];
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_ode_rk45(ode_growth, 1.0f, 0.0f, y0, 1, RK45_TOL, y));
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_ode_rk45(ode_growth, 1.0f, 1.0f, y0, 1, RK45_TOL, y));
}

void test_rk45_tol_nonpositive(void)
{
    numx_real_t y0[] = {1.0f}, y[1];
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_ode_rk45(ode_growth, 0.0f, 1.0f, y0, 1, 0.0f, y));
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_ode_rk45(ode_growth, 0.0f, 1.0f, y0, 1, -1e-3f, y));
}

void test_rk45_n_zero(void)
{
    numx_real_t y0[] = {1.0f}, y[1];
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_ode_rk45(ode_growth, 0.0f, 1.0f, y0, 0, RK45_TOL, y));
}

void numx_test_ode(void)
{
    RUN_TEST(test_rk4_growth_at_t1);
    RUN_TEST(test_rk4_decay_at_t1);
    RUN_TEST(test_rk4_growth_at_t2);
    RUN_TEST(test_rk4_harmonic_energy_conserved);
    RUN_TEST(test_rk4_single_step);
    RUN_TEST(test_rk4_propagates_rhs_error);
    RUN_TEST(test_rk4_null);
    RUN_TEST(test_rk4_h_nonpositive);
    RUN_TEST(test_rk4_steps_zero);
    RUN_TEST(test_rk4_n_zero);
    RUN_TEST(test_rk45_growth_at_t1);
    RUN_TEST(test_rk45_decay_at_t1);
    RUN_TEST(test_rk45_harmonic_energy_conserved);
    RUN_TEST(test_rk45_propagates_rhs_error);
    RUN_TEST(test_rk45_null);
    RUN_TEST(test_rk45_t1_le_t0);
    RUN_TEST(test_rk45_tol_nonpositive);
    RUN_TEST(test_rk45_n_zero);
}
