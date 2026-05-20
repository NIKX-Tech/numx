/**
 * @file test_sketch.c
 * @brief Tests for numx randomized SVD (sketch module).
 *
 * Correctness is verified by reconstruction error ||A - U*diag(S)*Vt||_F
 * (Frobenius norm of the approximation error) rather than individual
 * singular vectors, which have inherent sign ambiguity.
 */

#include "unity.h"
#include "numx/sketch.h"

#define TOL_RECON 0.5f   /* Frobenius reconstruction error (squared) */
#define TOL_SV    0.2f   /* singular value absolute error             */

/* Compute squared Frobenius reconstruction error for an m×n matrix. */
static numx_real_t recon_err_sq(
    const numx_real_t *A,
    const numx_real_t *U, const numx_real_t *S, const numx_real_t *Vt,
    numx_size_t m, numx_size_t n, numx_size_t rank)
{
    numx_size_t i, j, k;
    numx_real_t val, diff, err = (numx_real_t)0.0;
    for (i = 0; i < m; i++)
    {
        for (j = 0; j < n; j++)
        {
            val = (numx_real_t)0.0;
            for (k = 0; k < rank; k++)
                val += U[i * rank + k] * S[k] * Vt[k * n + j];
            diff = A[i * n + j] - val;
            err += diff * diff;
        }
    }
    return err;
}

/* ════════════════════════════════════════════════════════════════════
 *  numx_sketch_rsvd
 * ════════════════════════════════════════════════════════════════════ */

/* L1 — exact rank-1 matrix: σ_1 = 4, reconstruction error ≈ 0 */
void test_rsvd_rank1_reconstruction(void)
{
    /* A is 3×2 with only A[0,0] = 4 (rank 1, σ1 = 4). */
    numx_real_t A[6]  = {4.0f,0.0f, 0.0f,0.0f, 0.0f,0.0f};
    numx_real_t U[3]  = {0};  /* 3×1 */
    numx_real_t S[1]  = {0};
    numx_real_t Vt[2] = {0};  /* 1×2 */

    TEST_ASSERT_EQUAL(NUMX_OK,
        numx_sketch_rsvd(A, 3, 2, 1, 1, 42u, U, S, Vt));

    TEST_ASSERT_FLOAT_WITHIN(TOL_SV, 4.0f, S[0]);
    TEST_ASSERT_FLOAT_WITHIN(TOL_RECON, 0.0f,
        recon_err_sq(A, U, S, Vt, 3, 2, 1));
}

/* L1 — rank-2 diagonal: exact reconstruction when rank+oversample = min(m,n) */
void test_rsvd_diagonal_rank2(void)
{
    /* A = diag(3, 2) embedded in 4×4 → true σ = [3, 2]. */
    numx_real_t A[16] = {
        3.0f,0.0f,0.0f,0.0f,
        0.0f,2.0f,0.0f,0.0f,
        0.0f,0.0f,0.0f,0.0f,
        0.0f,0.0f,0.0f,0.0f
    };
    numx_real_t U[8]  = {0};  /* 4×2 */
    numx_real_t S[2]  = {0};
    numx_real_t Vt[8] = {0};  /* 2×4 */

    /* rank=2, oversample=2 → k_ext=4 = min(4,4): full range, near-exact. */
    TEST_ASSERT_EQUAL(NUMX_OK,
        numx_sketch_rsvd(A, 4, 4, 2, 2, 42u, U, S, Vt));

    /* Singular values should be in descending order and close to 3 and 2. */
    TEST_ASSERT_TRUE(S[0] >= S[1]);
    TEST_ASSERT_FLOAT_WITHIN(TOL_SV, 3.0f, S[0]);
    TEST_ASSERT_FLOAT_WITHIN(TOL_SV, 2.0f, S[1]);

    TEST_ASSERT_FLOAT_WITHIN(TOL_RECON, 0.0f,
        recon_err_sq(A, U, S, Vt, 4, 4, 2));
}

/* L2 — over-determined (m > n): 3×2 matrix, rank-1 approximation */
void test_rsvd_tall_matrix(void)
{
    /* A = [[1,0],[1,0],[1,0]]: rank 1, σ1 = sqrt(3). */
    numx_real_t A[6]  = {1,0, 1,0, 1,0};
    numx_real_t U[3]  = {0};  /* 3×1 */
    numx_real_t S[1]  = {0};
    numx_real_t Vt[2] = {0};  /* 1×2 */

    TEST_ASSERT_EQUAL(NUMX_OK,
        numx_sketch_rsvd(A, 3, 2, 1, 1, 7u, U, S, Vt));

    /* σ1 = sqrt(3) ≈ 1.732 */
    TEST_ASSERT_FLOAT_WITHIN(TOL_SV, 1.732f, S[0]);
    TEST_ASSERT_FLOAT_WITHIN(TOL_RECON, 0.0f,
        recon_err_sq(A, U, S, Vt, 3, 2, 1));
}

/* L4 — error cases */
void test_rsvd_null_returns_error(void)
{
    numx_real_t A[4] = {0}, U[2] = {0}, S[1] = {0}, Vt[2] = {0};
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
        numx_sketch_rsvd(NULL, 2, 2, 1, 1, 1u, U, S, Vt));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
        numx_sketch_rsvd(A, 2, 2, 1, 1, 1u, NULL, S, Vt));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
        numx_sketch_rsvd(A, 2, 2, 1, 1, 1u, U, NULL, Vt));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
        numx_sketch_rsvd(A, 2, 2, 1, 1, 1u, U, S, NULL));
}

void test_rsvd_invalid_arg_returns_error(void)
{
    numx_real_t A[4] = {0}, U[2] = {0}, S[1] = {0}, Vt[2] = {0};
    /* rank=0 */
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
        numx_sketch_rsvd(A, 2, 2, 0, 1, 1u, U, S, Vt));
    /* m=0 */
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
        numx_sketch_rsvd(A, 0, 2, 1, 1, 1u, U, S, Vt));
    /* rank + oversample > NUMX_MAX_SKETCH_RANK */
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
        numx_sketch_rsvd(A, 2, 2, NUMX_MAX_SKETCH_RANK, 1, 1u, U, S, Vt));
}

/* ════════════════════════════════════════════════════════════════════
 *  Suite entry point
 * ════════════════════════════════════════════════════════════════════ */

void numx_test_sketch(void)
{
    RUN_TEST(test_rsvd_rank1_reconstruction);
    RUN_TEST(test_rsvd_diagonal_rank2);
    RUN_TEST(test_rsvd_tall_matrix);
    RUN_TEST(test_rsvd_null_returns_error);
    RUN_TEST(test_rsvd_invalid_arg_returns_error);
}
