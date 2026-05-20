/**
 * @file ode.h
 * @brief ODE solvers for numx.
 *
 * Solvers advance a state vector y(t) in R^n from t0 toward t1.
 * The ODE is given as dy/dt = f(t, y, n, dy). Both solvers return only
 * the final state — no trajectory is stored (no dynamic allocation).
 */

#ifndef NUMX_ODE_H
#define NUMX_ODE_H

#include "numx_types.h"
#include "numx_config.h"

/**
 * @brief ODE right-hand side: dy/dt = f(t, y).
 *
 * @param[in]  t     Current time.
 * @param[in]  y     Current state vector, n elements. Must not be NULL.
 * @param[in]  n     State dimension.
 * @param[out] dydt  Output derivative vector, n elements. Must not be NULL.
 * @return NUMX_OK on success, or an error code to abort the solver.
 */
typedef numx_status_t (*numx_ode_func_t)(
    numx_real_t t,
    const numx_real_t *y,
    numx_size_t n,
    numx_real_t *dydt);

/**
 * @brief Advance an ODE system by fixed steps using classical 4th-order Runge-Kutta.
 *
 * Each step requires 4 evaluations of f. The global error is O(h^4).
 * Output y receives the state after exactly `steps` steps of size h.
 *
 * @param[in]  f      ODE right-hand side. Must not be NULL.
 * @param[in]  t0     Initial time.
 * @param[in]  y0     Initial state, n elements. Must not be NULL.
 * @param[in]  n      State dimension. Must be >= 1 and <= NUMX_MAX_ODE_DIM.
 * @param[in]  h      Step size. Must be > 0.
 * @param[in]  steps  Number of steps. Must be >= 1.
 * @param[out] y      Final state, n elements. Must not be NULL. May alias y0.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if f, y0, or y is NULL.
 *         NUMX_ERR_INVALID_ARG if n == 0, n > NUMX_MAX_ODE_DIM, h <= 0, or steps == 0.
 *         Any error code returned by f (propagated immediately).
 */
numx_status_t numx_ode_rk4(
    numx_ode_func_t f,
    numx_real_t t0,
    const numx_real_t *y0,
    numx_size_t n,
    numx_real_t h,
    numx_size_t steps,
    numx_real_t *y);

/**
 * @brief Advance an ODE system from t0 to t1 using adaptive Runge-Kutta-Fehlberg.
 *
 * Uses the RKF45 embedded pair (4th and 5th order). The step size is
 * automatically adjusted to keep the estimated local error below tol.
 * Output y receives the state at t1.
 *
 * Step control:
 *   h_new = h * 0.9 * (tol / ||err||_inf)^(1/5)
 *   clamped to [h/10, 10*h] to prevent extreme changes.
 *
 * @param[in]  f      ODE right-hand side. Must not be NULL.
 * @param[in]  t0     Initial time.
 * @param[in]  t1     Final time. Must satisfy t1 > t0.
 * @param[in]  y0     Initial state, n elements. Must not be NULL.
 * @param[in]  n      State dimension. Must be >= 1 and <= NUMX_MAX_ODE_DIM.
 * @param[in]  tol    Local error tolerance per step. Must be > 0.
 * @param[out] y      Final state at t1, n elements. Must not be NULL. May alias y0.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if f, y0, or y is NULL.
 *         NUMX_ERR_INVALID_ARG if n == 0, n > NUMX_MAX_ODE_DIM, tol <= 0, or t1 <= t0.
 *         NUMX_ERR_NO_CONVERGE if NUMX_MAX_ITER steps are taken without reaching t1.
 *         Any error code returned by f.
 */
numx_status_t numx_ode_rk45(
    numx_ode_func_t f,
    numx_real_t t0,
    numx_real_t t1,
    const numx_real_t *y0,
    numx_size_t n,
    numx_real_t tol,
    numx_real_t *y);

#endif /* NUMX_ODE_H */
