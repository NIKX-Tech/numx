/**
 * @file differentiate.h
 * @brief Numerical differentiation for numx.
 *
 * All three methods approximate f'(x) from function evaluations.
 * The step size h is caller-supplied; a good default is sqrt(epsilon)
 * for forward/central and epsilon^(1/3) for Richardson.
 */

#ifndef NUMX_DIFFERENTIATE_H
#define NUMX_DIFFERENTIATE_H

#include "numx_types.h"

/**
 * @brief Approximate f'(x) using the forward difference formula.
 *
 * Error is O(h) — one order lower than central difference. Use when
 * only forward evaluations are available or h is extremely small.
 *
 * Mathematical definition:
 *   f'(x) ≈ [f(x + h) - f(x)] / h
 *
 * @param[in]  f       Function to differentiate. Must not be NULL.
 * @param[in]  x       Point at which to evaluate the derivative.
 * @param[in]  h       Step size. Must be > 0.
 * @param[out] result  Pointer to store f'(x). Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if f or result is NULL.
 *         NUMX_ERR_INVALID_ARG if h <= 0.
 */
numx_status_t numx_diff_forward(
    numx_func1d_t f,
    numx_real_t x,
    numx_real_t h,
    numx_real_t *result);

/**
 * @brief Approximate f'(x) using the central difference formula.
 *
 * Error is O(h^2) — symmetric cancellation eliminates the O(h) term.
 * Recommended over forward difference for smooth functions.
 *
 * Mathematical definition:
 *   f'(x) ≈ [f(x + h) - f(x - h)] / (2h)
 *
 * @param[in]  f       Function to differentiate. Must not be NULL.
 * @param[in]  x       Point at which to evaluate the derivative.
 * @param[in]  h       Step size. Must be > 0.
 * @param[out] result  Pointer to store f'(x). Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if f or result is NULL.
 *         NUMX_ERR_INVALID_ARG if h <= 0.
 */
numx_status_t numx_diff_central(
    numx_func1d_t f,
    numx_real_t x,
    numx_real_t h,
    numx_real_t *result);

/**
 * @brief Approximate f'(x) via Richardson extrapolation on central differences.
 *
 * Combines central difference at h and h/2 to cancel the O(h^2) error,
 * yielding O(h^4) accuracy. Uses 4 function evaluations.
 *
 * Mathematical definition:
 *   D(h)   = [f(x+h) - f(x-h)] / (2h)        — O(h^2) estimate
 *   D(h/2) = [f(x+h/2) - f(x-h/2)] / h       — O(h^2) estimate
 *   f'(x)  ≈ (4*D(h/2) - D(h)) / 3            — O(h^4) after cancellation
 *
 * @param[in]  f       Function to differentiate. Must not be NULL.
 * @param[in]  x       Point at which to evaluate the derivative.
 * @param[in]  h       Initial step size. Must be > 0.
 * @param[out] result  Pointer to store f'(x). Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if f or result is NULL.
 *         NUMX_ERR_INVALID_ARG if h <= 0.
 */
numx_status_t numx_diff_richardson(
    numx_func1d_t f,
    numx_real_t x,
    numx_real_t h,
    numx_real_t *result);

#endif /* NUMX_DIFFERENTIATE_H */
