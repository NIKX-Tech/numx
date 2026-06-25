/**
 * @file ntt.h
 * @brief Number Theoretic Transform for numx.
 *
 * Provides the negacyclic NTT over Z_q with parameters tuned for
 * CRYSTALS-Kyber and CRYSTALS-Dilithium (q = 3329, n = 256).
 *
 * The ring is Z_3329[x]/(x^256 + 1). The NTT decomposes this ring into 128
 * degree-2 quotient rings via the factorisation:
 *
 *   x^256 + 1 = product_{k=0}^{127} (x^2 - zeta^{2k+1})
 *
 * where zeta = 17 (a primitive 256th root of unity mod 3329, with 17^128 = -1).
 *
 * Seven Cooley-Tukey butterfly stages produce 128 pairs (a_{2i}, a_{2i+1})
 * representing f mod (x^2 - c_i) for each i. Inverse NTT uses the
 * Gentleman-Sande structure and includes the normalization factor
 * 128^{-1} = 3303 (mod 3329).
 *
 * Barrett reduction (ceil(2^26/3329) = 20159) keeps all intermediate values
 * in [0, q-1] without any division. No heap allocation; all buffers are
 * caller-supplied.
 */

#ifndef NUMX_NTT_H
#define NUMX_NTT_H

#include "numx_config.h"
#include "numx_types.h"

/**
 * @brief In-place forward NTT over Z_3329[x]/(x^256 + 1).
 *
 * Transforms 256 coefficients in Z_3329 from polynomial domain into NTT domain
 * using seven Cooley-Tukey butterfly stages with precomputed twiddle factors.
 * The output is 128 pairs (f[2i], f[2i+1]) representing f mod (x^2 - c_i).
 * All coefficients are reduced to [0, q-1] after the transform.
 *
 * @param[in,out] f  Array of NUMX_NTT_N coefficients. Must not be NULL.
 *                   Each value must be in [0, NUMX_NTT_Q - 1] on entry.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR if f is NULL.
 */
numx_status_t numx_ntt_forward(numx_q15_t *f);

/**
 * @brief In-place inverse NTT over Z_3329[x]/(x^256 + 1).
 *
 * Transforms 256 NTT-domain coefficients back to polynomial domain using seven
 * Gentleman-Sande butterfly stages followed by multiplication by the
 * normalization constant 128^{-1} = 3303 (mod 3329). The output coefficients
 * are reduced to [0, q-1].
 *
 * @param[in,out] f  Array of NUMX_NTT_N NTT-domain coefficients. Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR if f is NULL.
 */
numx_status_t numx_ntt_inverse(numx_q15_t *f);

/**
 * @brief Pointwise multiplication of two NTT-domain arrays.
 *
 * Computes the product of two polynomials already in NTT domain. Each pair
 * (a[2i], a[2i+1]) and (b[2i], b[2i+1]) is multiplied as elements of the
 * degree-2 ring Z_3329[x]/(x^2 - c_i) using the basemul formula:
 *
 *   out[2i]   = a[2i]*b[2i] + c_i * a[2i+1]*b[2i+1]   (mod 3329)
 *   out[2i+1] = a[2i]*b[2i+1] + a[2i+1]*b[2i]         (mod 3329)
 *
 * Combined with numx_ntt_inverse, this implements polynomial multiplication
 * in Z_3329[x]/(x^256 + 1) in O(n) time after the O(n log n) NTT steps.
 *
 * @param[in]  a    First NTT-domain array of NUMX_NTT_N coefficients. Must not be NULL.
 * @param[in]  b    Second NTT-domain array of NUMX_NTT_N coefficients. Must not be NULL.
 * @param[out] out  Output array of NUMX_NTT_N coefficients. Must not be NULL.
 *                  May alias a or b.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR if a, b, or out is NULL.
 */
numx_status_t numx_ntt_pointwise_mul(
    const numx_q15_t *a,
    const numx_q15_t *b,
    numx_q15_t       *out
);

/**
 * @brief Full polynomial multiplication in Z_3329[x]/(x^256 + 1).
 *
 * Computes out = a * b in the ring Z_3329[x]/(x^256 + 1) using the NTT:
 *
 *   1. Forward NTT of a into a temporary buffer.
 *   2. Forward NTT of b into a temporary buffer.
 *   3. Pointwise multiplication of the two NTT arrays.
 *   4. Inverse NTT to recover the product.
 *
 * All temporaries are stack-allocated (512 bytes). The caller must supply
 * distinct output buffer out; a and b may be the same pointer.
 *
 * @param[in]  a    First polynomial: NUMX_NTT_N coefficients in [0, q-1]. Must not be NULL.
 * @param[in]  b    Second polynomial: NUMX_NTT_N coefficients in [0, q-1]. Must not be NULL.
 * @param[out] out  Product polynomial: NUMX_NTT_N coefficients in [0, q-1]. Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR if a, b, or out is NULL.
 */
numx_status_t numx_ntt_polymul(
    const numx_q15_t *a,
    const numx_q15_t *b,
    numx_q15_t       *out
);

/**
 * @brief Reduce all coefficients of a polynomial modulo q = 3329.
 *
 * Applies Barrett reduction to each of the NUMX_NTT_N coefficients, mapping
 * values in [0, 2*q-1] to the canonical range [0, q-1]. Useful after
 * external coefficient arithmetic before passing to numx_ntt_forward.
 *
 * @param[in,out] f  Array of NUMX_NTT_N coefficients. Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR if f is NULL.
 */
numx_status_t numx_ntt_reduce(numx_q15_t *f);

/**
 * @brief Coefficient-wise addition of two polynomials modulo q = 3329.
 *
 * Computes out[i] = (a[i] + b[i]) mod 3329 for each of the NUMX_NTT_N
 * coefficients. Works in both polynomial and NTT domain because the NTT
 * is linear. Inputs must be in [0, q-1]; output is in [0, q-1].
 *
 * @param[in]  a    First polynomial. Must not be NULL.
 * @param[in]  b    Second polynomial. Must not be NULL.
 * @param[out] out  Result polynomial. Must not be NULL. May alias a or b.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR if a, b, or out is NULL.
 */
numx_status_t numx_ntt_poly_add(
    const numx_q15_t *a,
    const numx_q15_t *b,
    numx_q15_t       *out
);

/**
 * @brief Coefficient-wise subtraction of two polynomials modulo q = 3329.
 *
 * Computes out[i] = (a[i] - b[i]) mod 3329 for each of the NUMX_NTT_N
 * coefficients. Works in both polynomial and NTT domain. Inputs must be
 * in [0, q-1]; output is in [0, q-1].
 *
 * @param[in]  a    Minuend polynomial. Must not be NULL.
 * @param[in]  b    Subtrahend polynomial. Must not be NULL.
 * @param[out] out  Result polynomial. Must not be NULL. May alias a or b.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR if a, b, or out is NULL.
 */
numx_status_t numx_ntt_poly_sub(
    const numx_q15_t *a,
    const numx_q15_t *b,
    numx_q15_t       *out
);

#endif /* NUMX_NTT_H */
