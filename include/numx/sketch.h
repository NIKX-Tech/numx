/**
 * @file sketch.h
 * @brief Randomized SVD via the Halko-Martinsson-Tropp algorithm.
 *
 * Computes a rank-@p rank approximation to an m×n matrix @c A:
 *
 *   @f[ A \approx U\,\Sigma\,V^\top @f]
 *
 * where @c U ∈ ℝ^{m×rank}, @c Σ = diag(S) ∈ ℝ^{rank×rank} (descending),
 * and @c Vt = V^⊤ ∈ ℝ^{rank×n}.
 *
 * No dynamic allocation is used.  Approximate stack usage per call:
 *   ~14 KB  (NUMX_MAX_SKETCH_M = NUMX_MAX_SKETCH_N = 64, NUMX_MAX_SKETCH_RANK = 16)
 *
 * Matrix @c A must be stored in row-major order: A(i,j) = @c A[i*n + j].
 * Output @c U is row-major (U(i,k) = U[i*rank + k]).
 * Output @c Vt is row-major (Vt(k,j) = Vt[k*n + j]).
 */

#ifndef NUMX_SKETCH_H
#define NUMX_SKETCH_H

#include <stdint.h>
#include "numx_config.h"
#include "numx_types.h"

/**
 * @brief Randomized SVD: A ≈ U * diag(S) * Vt.
 *
 * Algorithm (Halko, Martinsson, Tropp 2011):
 *  1. Draw random Gaussian test matrix Ω ∈ ℝ^{n×(rank+oversample)}.
 *  2. Form Y = A·Ω and orthogonalise via Modified Gram-Schmidt → Q.
 *  3. Project B = Q⊤·A and compute its Gram matrix B·B⊤.
 *  4. Jacobi eigendecomposition of B·B⊤ → singular values/vectors.
 *  5. Recover U = Q·Û, Vt = diag(1/Σ)·Û⊤·B, output @p rank largest triplets.
 *
 * @param A          m×n input matrix (row-major, must not be NULL).
 * @param m          Number of rows    (≤ NUMX_MAX_SKETCH_M).
 * @param n          Number of columns (≤ NUMX_MAX_SKETCH_N).
 * @param rank       Target rank of the approximation (> 0).
 * @param oversample Extra columns for the range finder (≥ 0).
 *                   Capped so that rank + oversample ≤ min(m,n)
 *                   and ≤ NUMX_MAX_SKETCH_RANK.
 * @param seed       32-bit PRNG seed (0 becomes 1 internally).
 * @param U          Output m×rank matrix (caller-allocated, row-major).
 * @param S          Output rank singular values in descending order.
 * @param Vt         Output rank×n matrix (caller-allocated, row-major).
 *
 * @return @c NUMX_OK on success.
 * @return @c NUMX_ERR_NULL_PTR if any pointer is NULL.
 * @return @c NUMX_ERR_INVALID_ARG if dimensions are 0 or out of range,
 *         or if rank + oversample > NUMX_MAX_SKETCH_RANK.
 */
numx_status_t numx_sketch_rsvd(
    const numx_real_t *A,
    numx_size_t m, numx_size_t n,
    numx_size_t rank, numx_size_t oversample,
    uint32_t seed,
    numx_real_t *U,
    numx_real_t *S,
    numx_real_t *Vt);

#endif /* NUMX_SKETCH_H */
