/**
 * @file test_compressed_sensing.c
 * @brief Compressed sensing tests — float32 / x86 host build.
 *        numx_real_t = float  (NUMX_USE_DOUBLE not set)
 */

#include "unity.h"
#include "numx/compressed_sensing.h"

#define TOL       (numx_real_t)1e-3
#define TOL_TIGHT (numx_real_t)1e-4

void test_omp_identity_1sparse(void)
{
    numx_real_t A[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
    numx_real_t y[4]  = {(numx_real_t)1.0,(numx_real_t)0.0,(numx_real_t)0.0,(numx_real_t)0.0};
    numx_real_t x[4]  = {0};
    TEST_ASSERT_EQUAL(NUMX_OK, numx_cs_omp(A, y, 4, 4, 1, x));
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, (numx_real_t)1.0, x[0]);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, (numx_real_t)0.0, x[1]);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, (numx_real_t)0.0, x[2]);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, (numx_real_t)0.0, x[3]);
}

void test_omp_identity_2sparse(void)
{
    numx_real_t A[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
    numx_real_t y[4]  = {(numx_real_t)0.0,(numx_real_t)1.0,(numx_real_t)0.0,(numx_real_t)1.0};
    numx_real_t x[4]  = {0};
    TEST_ASSERT_EQUAL(NUMX_OK, numx_cs_omp(A, y, 4, 4, 2, x));
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, (numx_real_t)0.0, x[0]);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, (numx_real_t)1.0, x[1]);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, (numx_real_t)0.0, x[2]);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, (numx_real_t)1.0, x[3]);
}

void test_omp_overdetermined_1sparse(void)
{
    numx_real_t A[6] = {1,0, 0,1, 1,1};
    numx_real_t y[3] = {(numx_real_t)2.0,(numx_real_t)0.0,(numx_real_t)2.0};
    numx_real_t x[2] = {0};
    TEST_ASSERT_EQUAL(NUMX_OK, numx_cs_omp(A, y, 3, 2, 1, x));
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, (numx_real_t)2.0, x[0]);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, (numx_real_t)0.0, x[1]);
}

void test_omp_null_returns_error(void)
{
    numx_real_t A[4]={0},y[2]={0},x[2]={0};
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_cs_omp(NULL,y,2,2,1,x));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_cs_omp(A,NULL,2,2,1,x));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_cs_omp(A,y,2,2,1,NULL));
}

void test_omp_invalid_arg_returns_error(void)
{
    numx_real_t A[4]={0},y[2]={0},x[2]={0};
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_cs_omp(A,y,2,2,3,x));
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_cs_omp(A,y,0,2,1,x));
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_cs_omp(A,y,2,2,0,x));
}

void test_ista_identity_shrinkage(void)
{
    numx_real_t A[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
    numx_real_t y[4]  = {(numx_real_t)1.0,(numx_real_t)0.0,(numx_real_t)0.0,(numx_real_t)0.0};
    numx_real_t x[4]  = {0};
    TEST_ASSERT_EQUAL(NUMX_OK, numx_cs_ista(A, y, 4, 4, (numx_real_t)0.1, (numx_real_t)1.0, 10, x));
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)0.9, x[0]);
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)0.0, x[1]);
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)0.0, x[2]);
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)0.0, x[3]);
}

void test_ista_zero_lambda_recovery(void)
{
    numx_real_t A[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
    numx_real_t y[4]  = {(numx_real_t)1.0,(numx_real_t)0.0,(numx_real_t)0.0,(numx_real_t)0.0};
    numx_real_t x[4]  = {0};
    TEST_ASSERT_EQUAL(NUMX_OK, numx_cs_ista(A, y, 4, 4, (numx_real_t)0.0, (numx_real_t)0.9, 100, x));
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)1.0, x[0]);
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)0.0, x[1]);
}

void test_ista_large_lambda_zeros(void)
{
    numx_real_t A[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
    numx_real_t y[4]  = {(numx_real_t)1.0,(numx_real_t)0.0,(numx_real_t)0.0,(numx_real_t)0.0};
    numx_real_t x[4]  = {0};
    TEST_ASSERT_EQUAL(NUMX_OK, numx_cs_ista(A, y, 4, 4, (numx_real_t)2.0, (numx_real_t)1.0, 20, x));
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)0.0, x[0]);
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)0.0, x[1]);
}

void test_ista_null_returns_error(void)
{
    numx_real_t A[4]={0},y[2]={0},x[2]={0};
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
                      numx_cs_ista(NULL,y,2,2,(numx_real_t)0.1,(numx_real_t)1.0,10,x));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
                      numx_cs_ista(A,NULL,2,2,(numx_real_t)0.1,(numx_real_t)1.0,10,x));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
                      numx_cs_ista(A,y,2,2,(numx_real_t)0.1,(numx_real_t)1.0,10,NULL));
}

void test_ista_invalid_step_returns_error(void)
{
    numx_real_t A[4]={0},y[2]={0},x[2]={0};
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_cs_ista(A,y,2,2,(numx_real_t)0.1,(numx_real_t)0.0,10,x));
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_cs_ista(A,y,2,2,-(numx_real_t)0.1,(numx_real_t)1.0,10,x));
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_cs_ista(A,y,0,2,(numx_real_t)0.1,(numx_real_t)1.0,10,x));
}

void test_spectral_norm_identity(void)
{
    numx_real_t A[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
    numx_real_t sigma;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_cs_spectral_norm(A, 4, 4, 32, &sigma));
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)1.0, sigma);
}

void test_spectral_norm_scaled_identity(void)
{
    numx_real_t A[16] = {3,0,0,0, 0,3,0,0, 0,0,3,0, 0,0,0,3};
    numx_real_t sigma;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_cs_spectral_norm(A, 4, 4, 32, &sigma));
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)3.0, sigma);
}

void test_spectral_norm_tall_matrix(void)
{
    numx_real_t A[6] = {1,0, 0,1, 0,0};
    numx_real_t sigma;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_cs_spectral_norm(A, 3, 2, 32, &sigma));
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)1.0, sigma);
}

void test_spectral_norm_null_returns_error(void)
{
    numx_real_t A[4]={0},sigma;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
                      numx_cs_spectral_norm(NULL,2,2,32,&sigma));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
                      numx_cs_spectral_norm(A,2,2,32,NULL));
}

void test_spectral_norm_invalid_arg(void)
{
    numx_real_t A[4]={0},sigma;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_cs_spectral_norm(A,0,2,32,&sigma));
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_cs_spectral_norm(A,2,0,32,&sigma));
}

void numx_test_compressed_sensing(void)
{
    RUN_TEST(test_omp_identity_1sparse);
    RUN_TEST(test_omp_identity_2sparse);
    RUN_TEST(test_omp_overdetermined_1sparse);
    RUN_TEST(test_omp_null_returns_error);
    RUN_TEST(test_omp_invalid_arg_returns_error);
    RUN_TEST(test_ista_identity_shrinkage);
    RUN_TEST(test_ista_zero_lambda_recovery);
    RUN_TEST(test_ista_large_lambda_zeros);
    RUN_TEST(test_ista_null_returns_error);
    RUN_TEST(test_ista_invalid_step_returns_error);
    RUN_TEST(test_spectral_norm_identity);
    RUN_TEST(test_spectral_norm_scaled_identity);
    RUN_TEST(test_spectral_norm_tall_matrix);
    RUN_TEST(test_spectral_norm_null_returns_error);
    RUN_TEST(test_spectral_norm_invalid_arg);
}
