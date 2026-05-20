/**
 * @file interpolate.h
 * @brief Interpolation algorithms for numx.
 *
 * Input nodes (xs, ys) must be sorted in strictly ascending order of xs.
 * For cubic spline, a two-step API is provided: precompute moments once,
 * then evaluate cheaply at many query points.
 */

#ifndef NUMX_INTERPOLATE_H
#define NUMX_INTERPOLATE_H

#include "numx_types.h"
#include "numx_config.h"

/**
 * @brief Evaluate a piecewise-linear interpolant at x.
 *
 * Finds the interval [xs[i], xs[i+1]] containing x and linearly
 * interpolates. Returns the boundary value for x outside [xs[0], xs[n-1]].
 *
 * @param[in]  xs      Sorted node x-coordinates. Must not be NULL.
 * @param[in]  ys      Node y-values, same length as xs. Must not be NULL.
 * @param[in]  n       Number of nodes. Must be >= 2 and <= NUMX_MAX_INTERP_NODES.
 * @param[in]  x       Query point.
 * @param[out] result  Interpolated value. Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if xs, ys, or result is NULL.
 *         NUMX_ERR_INVALID_ARG if n < 2 or n > NUMX_MAX_INTERP_NODES.
 */
numx_status_t numx_interp_linear(
    const numx_real_t *xs,
    const numx_real_t *ys,
    numx_size_t n,
    numx_real_t x,
    numx_real_t *result);

/**
 * @brief Precompute second derivatives (moments) for a natural cubic spline.
 *
 * Solves the (n-2) x (n-2) tridiagonal system arising from the natural
 * spline boundary conditions (m[0] = m[n-1] = 0) using the Thomas algorithm.
 * Call once per dataset, then call numx_interp_spline_eval many times.
 *
 * @param[in]  xs  Sorted node x-coordinates, n elements. Must not be NULL.
 * @param[in]  ys  Node y-values, n elements. Must not be NULL.
 * @param[in]  n   Number of nodes. Must be >= 2 and <= NUMX_MAX_INTERP_NODES.
 * @param[out] m   Output moments (second derivatives), n elements. Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if xs, ys, or m is NULL.
 *         NUMX_ERR_INVALID_ARG if n < 2 or n > NUMX_MAX_INTERP_NODES.
 */
numx_status_t numx_interp_spline_precompute(
    const numx_real_t *xs,
    const numx_real_t *ys,
    numx_size_t n,
    numx_real_t *m);

/**
 * @brief Evaluate a natural cubic spline at x given precomputed moments.
 *
 * Uses the moments from numx_interp_spline_precompute(). O(log n) search
 * for the containing interval, then O(1) polynomial evaluation.
 *
 * @param[in]  xs  Sorted node x-coordinates, n elements. Must not be NULL.
 * @param[in]  ys  Node y-values, n elements. Must not be NULL.
 * @param[in]  m   Moments from numx_interp_spline_precompute(), n elements.
 * @param[in]  n   Number of nodes. Must be >= 2 and <= NUMX_MAX_INTERP_NODES.
 * @param[in]  x   Query point.
 * @param[out] result  Interpolated value. Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if xs, ys, m, or result is NULL.
 *         NUMX_ERR_INVALID_ARG if n < 2 or n > NUMX_MAX_INTERP_NODES.
 */
numx_status_t numx_interp_spline_eval(
    const numx_real_t *xs,
    const numx_real_t *ys,
    const numx_real_t *m,
    numx_size_t n,
    numx_real_t x,
    numx_real_t *result);

/**
 * @brief One-shot natural cubic spline: precompute moments on the stack and evaluate.
 *
 * Convenience wrapper combining numx_interp_spline_precompute() and
 * numx_interp_spline_eval(). Suitable when evaluating the spline at only
 * one or a few query points per dataset.
 *
 * @param[in]  xs      Sorted node x-coordinates, n elements. Must not be NULL.
 * @param[in]  ys      Node y-values, n elements. Must not be NULL.
 * @param[in]  n       Number of nodes. Must be >= 2 and <= NUMX_MAX_INTERP_NODES.
 * @param[in]  x       Query point.
 * @param[out] result  Interpolated value. Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if xs, ys, or result is NULL.
 *         NUMX_ERR_INVALID_ARG if n < 2 or n > NUMX_MAX_INTERP_NODES.
 *
 * @note Allocates a moments buffer of n elements on the stack.
 */
numx_status_t numx_interp_spline_cubic(
    const numx_real_t *xs,
    const numx_real_t *ys,
    numx_size_t n,
    numx_real_t x,
    numx_real_t *result);

/**
 * @brief Interpolate f on [a, b] using n Chebyshev nodes (barycentric form).
 *
 * Evaluates f at n Chebyshev nodes of the first kind, then uses the
 * barycentric interpolation formula to evaluate the interpolant at x.
 * Near-minimax accuracy: Runge's phenomenon is suppressed.
 *
 * @param[in]  f       Function to interpolate. Must not be NULL.
 * @param[in]  n       Number of Chebyshev nodes. 2 <= n <= NUMX_MAX_INTERP_NODES.
 * @param[in]  a       Left endpoint of interpolation interval.
 * @param[in]  b       Right endpoint. Must satisfy b > a.
 * @param[in]  x       Query point (may be outside [a,b]; extrapolation).
 * @param[out] result  Interpolated value. Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if f or result is NULL.
 *         NUMX_ERR_INVALID_ARG if n < 2, n > NUMX_MAX_INTERP_NODES, or b <= a.
 */
numx_status_t numx_interp_chebyshev(
    numx_func1d_t f,
    numx_size_t n,
    numx_real_t a,
    numx_real_t b,
    numx_real_t x,
    numx_real_t *result);

#endif /* NUMX_INTERPOLATE_H */
