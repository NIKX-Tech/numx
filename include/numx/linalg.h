/**
 * @file linalg.h
 * @brief Linear algebra functions for numx.
 *
 * Provides vector and matrix operations for sizes up to NUMX_MAX_MAT_ROWS x
 * NUMX_MAX_MAT_COLS (configured in numx_config.h). All memory is caller-
 * supplied; this module performs zero dynamic allocation.
 *
 * Matrices are stored in row-major order as flat 1-D arrays.
 * Element (r, c) of a matrix with `cols` columns is at index r*cols + c.
 */

#ifndef NUMX_LINALG_H
#define NUMX_LINALG_H

#include "numx_types.h"
#include "numx_config.h"

/* ── Norm type ─────────────────────────────────────────────────────── */

typedef enum
{
    NUMX_NORM_L1 = 1, /* sum of absolute values           */
    NUMX_NORM_L2 = 2, /* Euclidean norm (sqrt of dot self) */
    NUMX_NORM_INF = 0 /* maximum absolute value            */
} numx_norm_t;

/* ── Vector operations ─────────────────────────────────────────────── */

/**
 * @brief Compute the dot product of two vectors.
 *
 * Computes result = sum(a[i] * b[i]) for i in [0, n).
 *
 * Mathematical definition:
 *   result = a . b = sum_i (a_i * b_i)
 *
 * @param[in]  a       First input vector. Must not be NULL.
 * @param[in]  b       Second input vector. Must not be NULL.
 * @param[in]  n       Number of elements. Must be > 0 and <= NUMX_MAX_VEC_SIZE.
 * @param[out] result  Pointer to store the dot product. Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if a, b, or result is NULL.
 *         NUMX_ERR_INVALID_ARG if n == 0 or n > NUMX_MAX_VEC_SIZE.
 */
numx_status_t numx_vec_dot(
    const numx_real_t *a,
    const numx_real_t *b,
    numx_size_t n,
    numx_real_t *result);

/**
 * @brief Compute a vector norm (L1, L2, or L-infinity).
 *
 * Computes the L1, L2, or L-inf norm. The L2 norm uses a Newton-Raphson
 * square root; no dependency on <math.h>.
 *
 * Mathematical definitions:
 *   L1:  ||a||_1 = sum_i |a_i|
 *   L2:  ||a||_2 = sqrt(sum_i a_i^2)
 *   Linf: ||a||_inf = max_i |a_i|
 *
 * @param[in]  a       Input vector. Must not be NULL.
 * @param[in]  n       Number of elements. Must be > 0 and <= NUMX_MAX_VEC_SIZE.
 * @param[in]  type    NUMX_NORM_L1, NUMX_NORM_L2, or NUMX_NORM_INF.
 * @param[out] result  Pointer to store the norm value. Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if a or result is NULL.
 *         NUMX_ERR_INVALID_ARG if n == 0, n > NUMX_MAX_VEC_SIZE, or type unknown.
 */
numx_status_t numx_vec_norm(
    const numx_real_t *a,
    numx_size_t n,
    numx_norm_t type,
    numx_real_t *result);

/**
 * @brief Compute the 3-D cross product: result = a x b.
 *
 * The result is perpendicular to both a and b with magnitude
 * ||a|| * ||b|| * sin(theta). Safe to call with result aliasing a or b.
 *
 * Mathematical definition:
 *   result[0] = a[1]*b[2] - a[2]*b[1]
 *   result[1] = a[2]*b[0] - a[0]*b[2]
 *   result[2] = a[0]*b[1] - a[1]*b[0]
 *
 * @param[in]  a       First 3-element vector. Must not be NULL.
 * @param[in]  b       Second 3-element vector. Must not be NULL.
 * @param[out] result  3-element output vector a x b. Must not be NULL.
 *                     May alias a or b (intermediates computed first).
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR if a, b, or result is NULL.
 */
numx_status_t numx_vec_cross3(
    const numx_real_t *a,
    const numx_real_t *b,
    numx_real_t *result);

/* ── Matrix operations ─────────────────────────────────────────────── */

/**
 * @brief Multiply two matrices: C = A * B.
 *
 * A is (ra x ca), B is (rb x cb); requires ca == rb.
 * C must point to a caller-supplied buffer of at least ra * cb elements.
 * C must not alias A or B.
 *
 * Mathematical definition:
 *   C[i][j] = sum_k A[i][k] * B[k][j]
 *
 * @param[in]  A   Matrix A, ra * ca elements (row-major). Must not be NULL.
 * @param[in]  ra  Row count of A. 1 <= ra <= NUMX_MAX_MAT_ROWS.
 * @param[in]  ca  Column count of A. 1 <= ca <= NUMX_MAX_MAT_COLS.
 * @param[in]  B   Matrix B, rb * cb elements (row-major). Must not be NULL.
 * @param[in]  rb  Row count of B. Must equal ca.
 * @param[in]  cb  Column count of B. 1 <= cb <= NUMX_MAX_MAT_COLS.
 * @param[out] C   Output matrix, ra * cb elements. Must not be NULL or alias A/B.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if A, B, or C is NULL.
 *         NUMX_ERR_INVALID_ARG if dimensions are out of range or ca != rb.
 */
numx_status_t numx_mat_mul(
    const numx_real_t *A, numx_size_t ra, numx_size_t ca,
    const numx_real_t *B, numx_size_t rb, numx_size_t cb,
    numx_real_t *C);

/**
 * @brief Transpose a matrix out-of-place: AT = A^T.
 *
 * A is (rows x cols); AT must be a caller-supplied buffer of at least
 * cols * rows elements. A and AT must not alias.
 *
 * @param[in]  A     Input matrix, rows * cols elements. Must not be NULL.
 * @param[in]  rows  Row count of A. 1 <= rows <= NUMX_MAX_MAT_ROWS.
 * @param[in]  cols  Column count of A. 1 <= cols <= NUMX_MAX_MAT_COLS.
 * @param[out] AT    Output matrix, cols * rows elements. Must not alias A.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if A or AT is NULL.
 *         NUMX_ERR_INVALID_ARG if dimensions are out of range.
 */
numx_status_t numx_mat_transpose(
    const numx_real_t *A,
    numx_size_t rows,
    numx_size_t cols,
    numx_real_t *AT);

/**
 * @brief Transpose a square matrix in-place.
 *
 * Swaps A[i][j] with A[j][i] for all i < j. Only valid for square matrices.
 *
 * @param[in,out] A  Square n x n matrix (row-major). Must not be NULL.
 * @param[in]     n  Dimension. 1 <= n <= NUMX_MAX_MAT_ROWS.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if A is NULL.
 *         NUMX_ERR_INVALID_ARG if n == 0 or n > NUMX_MAX_MAT_ROWS.
 */
numx_status_t numx_mat_transpose_sq(
    numx_real_t *A,
    numx_size_t n);

/**
 * @brief Compute the determinant of a square matrix.
 *
 * Uses exact cofactor formulas for n <= 4 and LU decomposition with
 * partial pivoting for n > 4. The input matrix A is not modified.
 *
 * @param[in]  A       Square n x n input matrix. Must not be NULL.
 * @param[in]  n       Dimension. 1 <= n <= NUMX_MAX_MAT_ROWS.
 * @param[out] result  Pointer to store the determinant. Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if A or result is NULL.
 *         NUMX_ERR_INVALID_ARG if n == 0 or n > NUMX_MAX_MAT_ROWS.
 *
 * @note For n > 4, a stack buffer of n*n floats is allocated internally.
 *       Reduce NUMX_MAX_MAT_ROWS if stack space is critical.
 */
numx_status_t numx_mat_det(
    const numx_real_t *A,
    numx_size_t n,
    numx_real_t *result);

/* ── LU decomposition ──────────────────────────────────────────────── */

/**
 * @brief LU decompose a square matrix with partial pivoting: P*A = L*U.
 *
 * Uses Doolittle's method. The result is stored compactly in LU: the
 * strictly lower triangle holds L (unit diagonal implied), the upper
 * triangle including the diagonal holds U. pivot[i] records the original
 * row index now occupying position i after all row swaps.
 *
 * @param[in]  A      Square n x n input matrix. Must not be NULL.
 * @param[in]  n      Dimension. 1 <= n <= NUMX_MAX_MAT_ROWS.
 * @param[out] LU     Compact LU output, n * n elements. Must not be NULL.
 * @param[out] pivot  Permutation array, n elements. Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if A, LU, or pivot is NULL.
 *         NUMX_ERR_INVALID_ARG if n == 0 or n > NUMX_MAX_MAT_ROWS.
 *         NUMX_ERR_SINGULAR    if A is numerically singular.
 */
numx_status_t numx_lu_decompose(
    const numx_real_t *A,
    numx_size_t n,
    numx_real_t *LU,
    numx_idx_t *pivot);

/**
 * @brief Solve A*x = b given an LU factorisation from numx_lu_decompose().
 *
 * Applies the permutation P to b, then performs forward substitution
 * (L*y = P*b) and backward substitution (U*x = y). x may alias b.
 *
 * Mathematical operation:
 *   Forward:  L * y = P * b
 *   Backward: U * x = y
 *
 * @param[in]  LU     Compact LU from numx_lu_decompose(), n * n elements.
 * @param[in]  pivot  Permutation array from numx_lu_decompose(), n elements.
 * @param[in]  n      System dimension. 1 <= n <= NUMX_MAX_MAT_ROWS.
 * @param[in]  b      Right-hand side vector, n elements. Must not be NULL.
 * @param[out] x      Solution vector, n elements. Must not be NULL. May alias b.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if LU, pivot, b, or x is NULL.
 *         NUMX_ERR_INVALID_ARG if n == 0 or n > NUMX_MAX_MAT_ROWS.
 *         NUMX_ERR_SINGULAR    if a zero diagonal is encountered.
 */
numx_status_t numx_lu_solve(
    const numx_real_t *LU,
    const numx_idx_t *pivot,
    numx_size_t n,
    const numx_real_t *b,
    numx_real_t *x);

#endif /* NUMX_LINALG_H */
