/**
 * @file integrate.h
 * @brief Numerical integration for numx.
 *
 * All integrators take a function pointer, interval [a, b], and number
 * of subintervals n. The Gauss integrator takes a point count (2, 4, or 8).
 */

#ifndef NUMX_INTEGRATE_H
#define NUMX_INTEGRATE_H

#include "numx_types.h"
#include "numx_config.h"

/**
 * @brief Integrate f over [a, b] using the trapezoidal rule.
 *
 * Uses n subintervals of equal width h = (b-a)/n. Error is O(h^2).
 *
 * Mathematical definition:
 *   integral ≈ (h/2) * [f(a) + 2*f(x_1) + ... + 2*f(x_{n-1}) + f(b)]
 *
 * @param[in]  f       Integrand. Must not be NULL.
 * @param[in]  a       Left endpoint of integration interval.
 * @param[in]  b       Right endpoint. Must satisfy b > a.
 * @param[in]  n       Number of subintervals. Must be >= 1.
 * @param[out] result  Pointer to store the integral estimate. Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if f or result is NULL.
 *         NUMX_ERR_INVALID_ARG if n == 0 or b <= a.
 */
numx_status_t numx_integrate_trap(
    numx_func1d_t  f,
    numx_real_t    a,
    numx_real_t    b,
    numx_size_t    n,
    numx_real_t   *result
);

/**
 * @brief Integrate f over [a, b] using Simpson's 1/3 rule.
 *
 * Uses n subintervals (n must be even). Error is O(h^4), significantly
 * more accurate than the trapezoidal rule for smooth functions.
 *
 * Mathematical definition:
 *   integral ≈ (h/3) * [f(a) + 4f(x_1) + 2f(x_2) + 4f(x_3) + ... + f(b)]
 *
 * @param[in]  f       Integrand. Must not be NULL.
 * @param[in]  a       Left endpoint.
 * @param[in]  b       Right endpoint. Must satisfy b > a.
 * @param[in]  n       Number of subintervals. Must be even and >= 2.
 * @param[out] result  Pointer to store the integral estimate. Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if f or result is NULL.
 *         NUMX_ERR_INVALID_ARG if n == 0, n is odd, or b <= a.
 */
numx_status_t numx_integrate_simpson(
    numx_func1d_t  f,
    numx_real_t    a,
    numx_real_t    b,
    numx_size_t    n,
    numx_real_t   *result
);

/**
 * @brief Integrate f over [a, b] using Gauss-Legendre quadrature.
 *
 * Uses npts fixed quadrature nodes and weights on [-1, 1], mapped to [a, b].
 * Exact for polynomials of degree up to 2*npts - 1. Nodes and weights are
 * precomputed constants (no runtime solve needed).
 *
 * @param[in]  f       Integrand. Must not be NULL.
 * @param[in]  a       Left endpoint.
 * @param[in]  b       Right endpoint. Must satisfy b > a.
 * @param[in]  npts    Number of quadrature points: 2, 4, or 8.
 * @param[out] result  Pointer to store the integral estimate. Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if f or result is NULL.
 *         NUMX_ERR_INVALID_ARG if npts not in {2, 4, 8} or b <= a.
 *
 * @note npts=2 is exact for cubics; npts=8 is exact for degree-15 polynomials.
 */
numx_status_t numx_integrate_gauss(
    numx_func1d_t  f,
    numx_real_t    a,
    numx_real_t    b,
    numx_size_t    npts,
    numx_real_t   *result
);

#endif /* NUMX_INTEGRATE_H */
