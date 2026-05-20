/**
 * @file roots.h
 * @brief Root-finding algorithms for numx.
 *
 * All solvers take a tolerance and respect NUMX_MAX_ITER. Functions
 * require the caller to bracket the root (provide a sign change interval)
 * except numx_root_newton which takes only an initial guess.
 */

#ifndef NUMX_ROOTS_H
#define NUMX_ROOTS_H

#include "numx_types.h"
#include "numx_config.h"

/**
 * @brief Find a root of f in [a, b] via bisection.
 *
 * Guarantees convergence whenever f(a) and f(b) have opposite signs.
 * Convergence is linear — approximately one bit of accuracy per iteration.
 *
 * Mathematical guarantee:
 *   |root - midpoint| <= |b - a| / 2^k  after k iterations.
 *
 * @param[in]  f     Function whose root is sought. Must not be NULL.
 * @param[in]  a     Left bracket endpoint.
 * @param[in]  b     Right bracket endpoint.
 * @param[in]  tol   Convergence tolerance (interval half-width). Must be > 0.
 * @param[out] root  Pointer to store the root. Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if f or root is NULL.
 *         NUMX_ERR_INVALID_ARG if tol <= 0 or f(a)*f(b) >= 0 (no sign change).
 *         NUMX_ERR_NO_CONVERGE if NUMX_MAX_ITER exceeded before |b-a| < tol.
 */
numx_status_t numx_root_bisect(
    numx_func1d_t f,
    numx_real_t a,
    numx_real_t b,
    numx_real_t tol,
    numx_real_t *root);

/**
 * @brief Find a root of f near x0 via Newton-Raphson.
 *
 * Requires f and its derivative df. Converges quadratically near a simple
 * root. May diverge if x0 is far from a root or near an inflection point.
 *
 * Iteration:
 *   x_{n+1} = x_n - f(x_n) / f'(x_n)
 *
 * @param[in]  f     Function whose root is sought. Must not be NULL.
 * @param[in]  df    Derivative of f. Must not be NULL.
 * @param[in]  x0    Initial guess.
 * @param[in]  tol   Convergence tolerance on |f(x)|. Must be > 0.
 * @param[out] root  Pointer to store the root. Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if f, df, or root is NULL.
 *         NUMX_ERR_INVALID_ARG if tol <= 0.
 *         NUMX_ERR_SINGULAR    if df(x) == 0 during iteration.
 *         NUMX_ERR_NO_CONVERGE if NUMX_MAX_ITER exceeded before |f(x)| < tol.
 */
numx_status_t numx_root_newton(
    numx_func1d_t f,
    numx_func1d_t df,
    numx_real_t x0,
    numx_real_t tol,
    numx_real_t *root);

/**
 * @brief Find a root of f in [a, b] via Brent's method.
 *
 * Combines bisection (guaranteed), secant, and inverse quadratic
 * interpolation. Superlinear convergence in practice while retaining
 * bisection's reliability. This is the recommended general-purpose solver.
 *
 * @param[in]  f     Function whose root is sought. Must not be NULL.
 * @param[in]  a     Left bracket endpoint.
 * @param[in]  b     Right bracket endpoint.
 * @param[in]  tol   Convergence tolerance. Must be > 0.
 * @param[out] root  Pointer to store the root. Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if f or root is NULL.
 *         NUMX_ERR_INVALID_ARG if tol <= 0 or f(a)*f(b) > 0 (no bracket).
 *         NUMX_ERR_NO_CONVERGE if NUMX_MAX_ITER exceeded.
 */
numx_status_t numx_root_brent(
    numx_func1d_t f,
    numx_real_t a,
    numx_real_t b,
    numx_real_t tol,
    numx_real_t *root);

#endif /* NUMX_ROOTS_H */
