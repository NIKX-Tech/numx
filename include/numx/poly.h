/**
 * @file poly.h
 * @brief Polynomial evaluation and root-finding for numx.
 *
 * Polynomials are represented as coefficient arrays in descending order:
 *   p(x) = coeffs[0]*x^degree + coeffs[1]*x^(degree-1) + ... + coeffs[degree]
 */

#ifndef NUMX_POLY_H
#define NUMX_POLY_H

#include "numx_types.h"
#include "numx_config.h"

/**
 * @brief Evaluate a polynomial at x using Horner's method.
 *
 * Horner's method evaluates an n-th degree polynomial in O(n) multiplications
 * and additions, which is optimal and numerically more stable than the naive
 * power-sum approach.
 *
 * Mathematical definition (Horner factoring):
 *   p(x) = (...((coeffs[0]*x + coeffs[1])*x + coeffs[2])*x ... + coeffs[degree])
 *
 * @param[in]  coeffs  Coefficients in descending order (coeffs[0] is leading).
 *                     Must not be NULL. Length must be degree + 1.
 * @param[in]  degree  Degree of the polynomial. Must be <= NUMX_MAX_POLY_DEGREE.
 * @param[in]  x       Evaluation point.
 * @param[out] result  Pointer to store p(x). Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if coeffs or result is NULL.
 *         NUMX_ERR_INVALID_ARG if degree > NUMX_MAX_POLY_DEGREE.
 */
numx_status_t numx_poly_eval(
    const numx_real_t *coeffs,
    numx_size_t degree,
    numx_real_t x,
    numx_real_t *result);

/**
 * @brief Find real roots of a polynomial using Newton-Raphson with deflation.
 *
 * Iterates Newton's method from multiple starting points, then deflates
 * the polynomial by each found root to isolate remaining roots. Finds only
 * real roots; complex conjugate pairs are not returned.
 *
 * @param[in]  coeffs   Coefficients in descending order. Must not be NULL.
 * @param[in]  degree   Degree of the polynomial. 1 <= degree <= NUMX_MAX_POLY_DEGREE.
 * @param[out] roots    Buffer for real roots, at least degree elements. Must not be NULL.
 * @param[out] nroots   Number of real roots found. Must not be NULL.
 * @param[in]  tol      Root-finding tolerance. Must be > 0.
 *
 * @return NUMX_OK on success (nroots may be < degree if complex roots exist).
 *         NUMX_ERR_NULL_PTR    if coeffs, roots, or nroots is NULL.
 *         NUMX_ERR_INVALID_ARG if degree == 0, degree > NUMX_MAX_POLY_DEGREE, or tol <= 0.
 *
 * @note Ill-conditioned polynomials (Wilkinson-type) may yield inaccurate roots.
 *       Use double precision (NUMX_USE_DOUBLE) for degree > 8.
 */
numx_status_t numx_poly_roots(
    const numx_real_t *coeffs,
    numx_size_t degree,
    numx_real_t *roots,
    numx_size_t *nroots,
    numx_real_t tol);

#endif /* NUMX_POLY_H */
