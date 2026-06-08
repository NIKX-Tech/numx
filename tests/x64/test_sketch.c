/**
 * @file test_sketch.c
 * @brief Randomized SVD tests — float64 / x64 host build.
 *        numx_real_t = double  (compiled with -DNUMX_USE_DOUBLE)
 *
 * Tolerances remain loose: rSVD is a randomized algorithm and reconstruction
 * quality is bounded by algorithm design, not floating-point precision.
 */

#include "unity.h"
#include "numx/sketch.h"

#define TOL_RECON 0.5
#define TOL_SV    0.2

static numx_real_t recon_err_sq(
    const numx_real_t *A,
    const numx_real_t *U, const numx_real_t *S, const numx_real_t *Vt,
    numx_size_t m, numx_size_t n, numx_size_t rank)
{
    numx_size_t i, j, k;
    numx_real_t val, diff, err = (numx_real_t)0.0;
    for (i = 0; i < m; i++)
        for (j = 0; j < n; j++)
        {
            val = (numx_real_t)0.0;
            for (k = 0; k < rank; k++)
                val += U[i*rank+k]*S[k]*Vt[k*n+j];
            diff = A[i*n+j]-val;
            err += diff*diff;
        }
    return err;
}

void test_rsvd_rank1_reconstruction(void)
{
    numx_real_t A[6]  = {4.0,0.0, 0.0,0.0, 0.0,0.0};
    numx_real_t U[3]  = {0};
    numx_real_t S[1]  = {0};
    numx_real_t Vt[2] = {0};
    TEST_ASSERT_EQUAL(NUMX_OK,
        numx_sketch_rsvd(A, 3, 2, 1, 1, 42u, U, S, Vt));
    TEST_ASSERT_DOUBLE_WITHIN(TOL_SV, 4.0, S[0]);
    TEST_ASSERT_DOUBLE_WITHIN(TOL_RECON, 0.0,
        recon_err_sq(A, U, S, Vt, 3, 2, 1));
}

void test_rsvd_diagonal_rank2(void)
{
    numx_real_t A[16] = {
        3.0,0.0,0.0,0.0,
        0.0,2.0,0.0,0.0,
        0.0,0.0,0.0,0.0,
        0.0,0.0,0.0,0.0
    };
    numx_real_t U[8]  = {0};
    numx_real_t S[2]  = {0};
    numx_real_t Vt[8] = {0};
    TEST_ASSERT_EQUAL(NUMX_OK,
        numx_sketch_rsvd(A, 4, 4, 2, 2, 42u, U, S, Vt));
    TEST_ASSERT_TRUE(S[0] >= S[1]);
    TEST_ASSERT_DOUBLE_WITHIN(TOL_SV, 3.0, S[0]);
    TEST_ASSERT_DOUBLE_WITHIN(TOL_SV, 2.0, S[1]);
    TEST_ASSERT_DOUBLE_WITHIN(TOL_RECON, 0.0,
        recon_err_sq(A, U, S, Vt, 4, 4, 2));
}

void test_rsvd_tall_matrix(void)
{
    numx_real_t A[6]  = {1,0, 1,0, 1,0};
    numx_real_t U[3]  = {0};
    numx_real_t S[1]  = {0};
    numx_real_t Vt[2] = {0};
    TEST_ASSERT_EQUAL(NUMX_OK,
        numx_sketch_rsvd(A, 3, 2, 1, 1, 7u, U, S, Vt));
    TEST_ASSERT_DOUBLE_WITHIN(TOL_SV, 1.732050808, S[0]);
    TEST_ASSERT_DOUBLE_WITHIN(TOL_RECON, 0.0,
        recon_err_sq(A, U, S, Vt, 3, 2, 1));
}

void test_rsvd_null_returns_error(void)
{
    numx_real_t A[4]={0},U[2]={0},S[1]={0},Vt[2]={0};
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
        numx_sketch_rsvd(NULL,2,2,1,1,1u,U,S,Vt));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
        numx_sketch_rsvd(A,2,2,1,1,1u,NULL,S,Vt));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
        numx_sketch_rsvd(A,2,2,1,1,1u,U,NULL,Vt));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
        numx_sketch_rsvd(A,2,2,1,1,1u,U,S,NULL));
}

void test_rsvd_invalid_arg_returns_error(void)
{
    numx_real_t A[4]={0},U[2]={0},S[1]={0},Vt[2]={0};
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
        numx_sketch_rsvd(A,2,2,0,1,1u,U,S,Vt));
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
        numx_sketch_rsvd(A,0,2,1,1,1u,U,S,Vt));
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
        numx_sketch_rsvd(A,2,2,NUMX_MAX_SKETCH_RANK,1,1u,U,S,Vt));
}

void numx_test_sketch(void)
{
    RUN_TEST(test_rsvd_rank1_reconstruction);
    RUN_TEST(test_rsvd_diagonal_rank2);
    RUN_TEST(test_rsvd_tall_matrix);
    RUN_TEST(test_rsvd_null_returns_error);
    RUN_TEST(test_rsvd_invalid_arg_returns_error);
}
