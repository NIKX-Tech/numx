/**
 * @file test_compressed_sensing.c
 * @brief Tests for numx OMP and ISTA sparse recovery.
 *
 * Test matrices use the identity or simple over-determined forms so that
 * ground-truth solutions can be verified analytically.
 */

#include "unity.h"
#include "numx/compressed_sensing.h"

#define TOL       1e-3f   /* ISTA / power-iteration results     */
#define TOL_TIGHT 1e-4f   /* OMP with identity (exact algebra)  */

/* ════════════════════════════════════════════════════════════════════
 *  numx_cs_omp
 * ════════════════════════════════════════════════════════════════════ */

/* L1 — exact 1-sparse recovery with A = I_4 */
void test_omp_identity_1sparse(void)
{
    /* A = I_4, y = e_0 = [1,0,0,0], k=1 → x = [1,0,0,0] */
    numx_real_t A[16] = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };
    numx_real_t y[4] = {1.0f, 0.0f, 0.0f, 0.0f};
    numx_real_t x[4] = {0};
    TEST_ASSERT_EQUAL(NUMX_OK, numx_cs_omp(A, y, 4, 4, 1, x));
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, 1.0f, x[0]);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, 0.0f, x[1]);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, 0.0f, x[2]);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, 0.0f, x[3]);
}

/* L1 — exact 2-sparse recovery with A = I_4 */
void test_omp_identity_2sparse(void)
{
    /* A = I_4, y = [0,1,0,1] = e_1 + e_3, k=2 → x = [0,1,0,1] */
    numx_real_t A[16] = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };
    numx_real_t y[4] = {0.0f, 1.0f, 0.0f, 1.0f};
    numx_real_t x[4] = {0};
    TEST_ASSERT_EQUAL(NUMX_OK, numx_cs_omp(A, y, 4, 4, 2, x));
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, 0.0f, x[0]);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, 1.0f, x[1]);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, 0.0f, x[2]);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, 1.0f, x[3]);
}

/* L2 — over-determined sensing (m > n): 1-sparse recovery */
void test_omp_overdetermined_1sparse(void)
{
    /* A is 3×2, y = A * [2, 0]:
     *   A = [[1,0],[0,1],[1,1]], y = [2,0,2]
     * OMP k=1 selects column 0 (highest correlation), solves 1×1 LS:
     *   G = A[:,0]^T A[:,0] = 1+0+1 = 2, b = [1,0,1]^T [2,0,2] = 4
     *   coeffs = 2 → x = [2, 0] */
    numx_real_t A[6] = {1,0, 0,1, 1,1};
    numx_real_t y[3] = {2.0f, 0.0f, 2.0f};
    numx_real_t x[2] = {0};
    TEST_ASSERT_EQUAL(NUMX_OK, numx_cs_omp(A, y, 3, 2, 1, x));
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, 2.0f, x[0]);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, 0.0f, x[1]);
}

/* L4 — error cases */
void test_omp_null_returns_error(void)
{
    numx_real_t A[4] = {0}, y[2] = {0}, x[2] = {0};
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_cs_omp(NULL, y, 2, 2, 1, x));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_cs_omp(A, NULL, 2, 2, 1, x));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_cs_omp(A, y, 2, 2, 1, NULL));
}

void test_omp_invalid_arg_returns_error(void)
{
    numx_real_t A[4] = {0}, y[2] = {0}, x[2] = {0};
    /* k > n */
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_cs_omp(A, y, 2, 2, 3, x));
    /* m == 0 */
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_cs_omp(A, y, 0, 2, 1, x));
    /* k == 0 */
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_cs_omp(A, y, 2, 2, 0, x));
}

/* ════════════════════════════════════════════════════════════════════
 *  numx_cs_ista
 * ════════════════════════════════════════════════════════════════════ */

/* L1 — 1-sparse recovery with identity and soft shrinkage
 *
 * A = I_4, y = [1,0,0,0], lambda=0.1, step=1.
 * Analytical solution: x* = [max(1-lambda,0), 0, 0, 0] = [0.9, 0, 0, 0].
 * With step=1 on identity, ISTA converges in one iteration. */
void test_ista_identity_shrinkage(void)
{
    numx_real_t A[16] = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };
    numx_real_t y[4] = {1.0f, 0.0f, 0.0f, 0.0f};
    numx_real_t x[4] = {0};
    TEST_ASSERT_EQUAL(NUMX_OK,
        numx_cs_ista(A, y, 4, 4, 0.1f, 1.0f, 10, x));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 0.9f, x[0]);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 0.0f, x[1]);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 0.0f, x[2]);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 0.0f, x[3]);
}

/* L2 — zero lambda: reduces to gradient descent, converges to y */
void test_ista_zero_lambda_recovery(void)
{
    /* A = I_4, lambda=0, step=0.9.  x_t[0] = 1 - 0.1^t → 1 after 100 iters */
    numx_real_t A[16] = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };
    numx_real_t y[4] = {1.0f, 0.0f, 0.0f, 0.0f};
    numx_real_t x[4] = {0};
    TEST_ASSERT_EQUAL(NUMX_OK,
        numx_cs_ista(A, y, 4, 4, 0.0f, 0.9f, 100, x));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 1.0f, x[0]);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 0.0f, x[1]);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 0.0f, x[2]);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 0.0f, x[3]);
}

/* L2 — large lambda drives all coefficients to zero */
void test_ista_large_lambda_zeros(void)
{
    /* lambda*step = 2 > 1 = max(|y|), so soft-threshold kills everything */
    numx_real_t A[16] = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };
    numx_real_t y[4] = {1.0f, 0.0f, 0.0f, 0.0f};
    numx_real_t x[4] = {0};
    TEST_ASSERT_EQUAL(NUMX_OK,
        numx_cs_ista(A, y, 4, 4, 2.0f, 1.0f, 20, x));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 0.0f, x[0]);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 0.0f, x[1]);
}

/* L4 — error cases */
void test_ista_null_returns_error(void)
{
    numx_real_t A[4] = {0}, y[2] = {0}, x[2] = {0};
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
        numx_cs_ista(NULL, y, 2, 2, 0.1f, 1.0f, 10, x));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
        numx_cs_ista(A, NULL, 2, 2, 0.1f, 1.0f, 10, x));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
        numx_cs_ista(A, y, 2, 2, 0.1f, 1.0f, 10, NULL));
}

void test_ista_invalid_step_returns_error(void)
{
    numx_real_t A[4] = {0}, y[2] = {0}, x[2] = {0};
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
        numx_cs_ista(A, y, 2, 2, 0.1f, 0.0f, 10, x)); /* step=0 */
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
        numx_cs_ista(A, y, 2, 2, -0.1f, 1.0f, 10, x)); /* lambda<0 */
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
        numx_cs_ista(A, y, 0, 2, 0.1f, 1.0f, 10, x)); /* m=0 */
}

/* ════════════════════════════════════════════════════════════════════
 *  numx_cs_spectral_norm
 * ════════════════════════════════════════════════════════════════════ */

/* L1 — identity matrix: sigma_max = 1 */
void test_spectral_norm_identity(void)
{
    numx_real_t A[16] = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };
    numx_real_t sigma;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_cs_spectral_norm(A, 4, 4, 32, &sigma));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 1.0f, sigma);
}

/* L1 — scaled identity: sigma_max = scale */
void test_spectral_norm_scaled_identity(void)
{
    /* A = 3 * I_4 → sigma_max = 3 */
    numx_real_t A[16] = {
        3,0,0,0,
        0,3,0,0,
        0,0,3,0,
        0,0,0,3
    };
    numx_real_t sigma;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_cs_spectral_norm(A, 4, 4, 32, &sigma));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 3.0f, sigma);
}

/* L2 — over-determined: A = [[1,0],[0,1],[0,0]], sigma_max = 1 */
void test_spectral_norm_tall_matrix(void)
{
    numx_real_t A[6] = {1,0, 0,1, 0,0};
    numx_real_t sigma;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_cs_spectral_norm(A, 3, 2, 32, &sigma));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 1.0f, sigma);
}

/* L4 — error cases */
void test_spectral_norm_null_returns_error(void)
{
    numx_real_t A[4] = {0}, sigma;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
        numx_cs_spectral_norm(NULL, 2, 2, 32, &sigma));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
        numx_cs_spectral_norm(A, 2, 2, 32, NULL));
}

void test_spectral_norm_invalid_arg(void)
{
    numx_real_t A[4] = {0}, sigma;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
        numx_cs_spectral_norm(A, 0, 2, 32, &sigma));
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
        numx_cs_spectral_norm(A, 2, 0, 32, &sigma));
}

/* ════════════════════════════════════════════════════════════════════
 *  Suite entry point
 * ════════════════════════════════════════════════════════════════════ */

void numx_test_compressed_sensing(void)
{
    /* OMP */
    RUN_TEST(test_omp_identity_1sparse);
    RUN_TEST(test_omp_identity_2sparse);
    RUN_TEST(test_omp_overdetermined_1sparse);
    RUN_TEST(test_omp_null_returns_error);
    RUN_TEST(test_omp_invalid_arg_returns_error);
    /* ISTA */
    RUN_TEST(test_ista_identity_shrinkage);
    RUN_TEST(test_ista_zero_lambda_recovery);
    RUN_TEST(test_ista_large_lambda_zeros);
    RUN_TEST(test_ista_null_returns_error);
    RUN_TEST(test_ista_invalid_step_returns_error);
    /* Spectral norm */
    RUN_TEST(test_spectral_norm_identity);
    RUN_TEST(test_spectral_norm_scaled_identity);
    RUN_TEST(test_spectral_norm_tall_matrix);
    RUN_TEST(test_spectral_norm_null_returns_error);
    RUN_TEST(test_spectral_norm_invalid_arg);
}
