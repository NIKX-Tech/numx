/**
 * @file compressed_sensing.h
 * @brief Sparse recovery — Orthogonal Matching Pursuit (OMP) and
 *        Iterative Shrinkage-Thresholding Algorithm (ISTA).
 *
 * Both algorithms solve the problem of recovering a sparse signal @c x
 * from compressed measurements @c y = A*x, where @c A is an
 * m×n sensing matrix with m < n.
 *
 * No dynamic allocation is used. Stack usage per call:
 *  - OMP:           ~9 KB  (NUMX_MAX_CS_SPARSITY=32, NUMX_MAX_CS_MEASUREMENTS=256)
 *  - ISTA:          ~2 KB  (NUMX_MAX_CS_MEASUREMENTS=256, NUMX_MAX_CS_SIGNAL_DIM=256)
 *  - spectral_norm: ~3 KB  (same limits)
 *
 * Reduce NUMX_MAX_CS_SPARSITY and NUMX_MAX_CS_MEASUREMENTS in
 * numx_config.h to lower memory for constrained targets.
 *
 * Matrix @c A must be stored in row-major order: element (i,j) is at
 * @c A[i*n + j].
 */

#ifndef NUMX_COMPRESSED_SENSING_H
#define NUMX_COMPRESSED_SENSING_H

#include "numx_config.h"
#include "numx_types.h"

/**
 * @brief Orthogonal Matching Pursuit (OMP) sparse recovery.
 *
 * Greedy algorithm: at each of @p k iterations selects the dictionary
 * column most correlated with the current residual, then solves a
 * least-squares problem over the selected support.  Exact recovery is
 * guaranteed when the restricted isometry constant @c δ_{2k} < √2 − 1.
 *
 * @param A    m×n sensing matrix (row-major, must not be NULL).
 * @param y    m-dimensional measurement vector.
 * @param m    Number of measurements (≤ NUMX_MAX_CS_MEASUREMENTS).
 * @param n    Signal dimension    (≤ NUMX_MAX_CS_SIGNAL_DIM).
 * @param k    Target sparsity     (≤ NUMX_MAX_CS_SPARSITY, ≤ m, ≤ n).
 * @param x    Output: n-dimensional recovered signal (caller-allocated).
 *
 * @return @c NUMX_OK on success.
 * @return @c NUMX_ERR_NULL_PTR if any pointer is NULL.
 * @return @c NUMX_ERR_INVALID_ARG if dimensions are out of range.
 * @return @c NUMX_ERR_SINGULAR if the selected sub-matrix is singular.
 */
numx_status_t numx_cs_omp(
    const numx_real_t *A,
    const numx_real_t *y,
    numx_size_t m, numx_size_t n, numx_size_t k,
    numx_real_t *x);

/**
 * @brief Iterative Shrinkage-Thresholding Algorithm (ISTA) sparse recovery.
 *
 * Minimises the LASSO objective:
 *   @f[ \min_x \tfrac{1}{2}\|Ax - y\|_2^2 + \lambda\|x\|_1 @f]
 *
 * Each iteration applies a gradient step followed by the soft-threshold
 * proximal operator.  Converges when @p step < 1/σ_max(A)² where
 * σ_max(A) is the largest singular value of @c A (use
 * @c numx_cs_spectral_norm to compute it).
 *
 * @param A         m×n sensing matrix (row-major).
 * @param y         m-dimensional measurement vector.
 * @param m         Number of measurements (≤ NUMX_MAX_CS_MEASUREMENTS).
 * @param n         Signal dimension    (≤ NUMX_MAX_CS_SIGNAL_DIM).
 * @param lambda    L1 regularisation weight (≥ 0).
 * @param step      Gradient step size (> 0, must satisfy step < 1/σ²).
 * @param max_iter  Number of iterations to run.
 * @param x         Output: n-dimensional recovered signal (zeroed on entry).
 *
 * @return @c NUMX_OK on success.
 * @return @c NUMX_ERR_NULL_PTR if any pointer is NULL.
 * @return @c NUMX_ERR_INVALID_ARG if step ≤ 0, lambda < 0, or dimensions OOB.
 */
numx_status_t numx_cs_ista(
    const numx_real_t *A,
    const numx_real_t *y,
    numx_size_t m, numx_size_t n,
    numx_real_t lambda, numx_real_t step,
    numx_size_t max_iter,
    numx_real_t *x);

/**
 * @brief Estimate the largest singular value of @c A via power iteration.
 *
 * The ISTA step size should satisfy: @c step < 1 / result².
 *
 * @param A         m×n sensing matrix (row-major).
 * @param m         Number of rows    (≤ NUMX_MAX_CS_MEASUREMENTS).
 * @param n         Number of columns (≤ NUMX_MAX_CS_SIGNAL_DIM).
 * @param max_iter  Power-iteration steps (32 is sufficient for most matrices).
 * @param result    Output: estimated σ_max(A).
 *
 * @return @c NUMX_OK on success.
 * @return @c NUMX_ERR_NULL_PTR if any pointer is NULL.
 * @return @c NUMX_ERR_INVALID_ARG if dimensions are 0 or out of range.
 */
numx_status_t numx_cs_spectral_norm(
    const numx_real_t *A,
    numx_size_t m, numx_size_t n,
    numx_size_t max_iter,
    numx_real_t *result);

#endif /* NUMX_COMPRESSED_SENSING_H */
