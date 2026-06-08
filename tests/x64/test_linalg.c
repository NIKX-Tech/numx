/**
 * @file test_linalg.c
 * @brief Linear algebra tests — float64 / x64 host build.
 *        numx_real_t = double  (compiled with -DNUMX_USE_DOUBLE)
 *
 * L1 Known-answer | L2 Property | L3 Edge case | L4 Error handling
 */

#include "unity.h"
#include "numx/numx_types.h"
#include "numx/linalg.h"

#define TOL    1e-10
#define TOL_LU 1e-9

/* ── numx_vec_dot ──────────────────────────────────────────────────── */

void test_vec_dot_known(void)
{
    numx_real_t a[] = {1.0, 2.0, 3.0};
    numx_real_t b[] = {4.0, 5.0, 6.0};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_vec_dot(a, b, 3, &r));
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 32.0, r);
}

void test_vec_dot_orthogonal(void)
{
    numx_real_t a[] = {1.0, 0.0, 0.0};
    numx_real_t b[] = {0.0, 1.0, 0.0};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_vec_dot(a, b, 3, &r));
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 0.0, r);
}

void test_vec_dot_commutative(void)
{
    numx_real_t a[] = {1.0, 2.0, 3.0};
    numx_real_t b[] = {4.0, 5.0, 6.0};
    numx_real_t r1, r2;
    numx_vec_dot(a, b, 3, &r1);
    numx_vec_dot(b, a, 3, &r2);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, r1, r2);
}

void test_vec_dot_self_equals_sq_norm(void)
{
    numx_real_t a[] = {3.0, 4.0};
    numx_real_t r;
    numx_vec_dot(a, a, 2, &r);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 25.0, r);
}

void test_vec_dot_single_element(void)
{
    numx_real_t a[] = {7.0};
    numx_real_t b[] = {3.0};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_vec_dot(a, b, 1, &r));
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 21.0, r);
}

void test_vec_dot_zero_vector(void)
{
    numx_real_t a[] = {0.0, 0.0, 0.0};
    numx_real_t b[] = {1.0, 2.0, 3.0};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_vec_dot(a, b, 3, &r));
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 0.0, r);
}

void test_vec_dot_null_a(void)
{
    numx_real_t b[] = {1.0}, r;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_vec_dot(NULL, b, 1, &r));
}

void test_vec_dot_null_b(void)
{
    numx_real_t a[] = {1.0}, r;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_vec_dot(a, NULL, 1, &r));
}

void test_vec_dot_null_result(void)
{
    numx_real_t a[] = {1.0}, b[] = {1.0};
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_vec_dot(a, b, 1, NULL));
}

void test_vec_dot_zero_n(void)
{
    numx_real_t a[] = {1.0}, b[] = {1.0}, r;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_vec_dot(a, b, 0, &r));
}

/* ── numx_vec_norm ─────────────────────────────────────────────────── */

void test_vec_norm_l2_pythagorean(void)
{
    numx_real_t a[] = {3.0, 4.0};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_vec_norm(a, 2, NUMX_NORM_L2, &r));
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 5.0, r);
}

void test_vec_norm_l1_known(void)
{
    numx_real_t a[] = {3.0, -4.0, 0.0};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_vec_norm(a, 3, NUMX_NORM_L1, &r));
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 7.0, r);
}

void test_vec_norm_linf_known(void)
{
    numx_real_t a[] = {3.0, -5.0, 4.0};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_vec_norm(a, 3, NUMX_NORM_INF, &r));
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 5.0, r);
}

void test_vec_norm_unit_vector_is_one(void)
{
    numx_real_t a[] = {1.0, 0.0, 0.0};
    numx_real_t r;
    numx_vec_norm(a, 3, NUMX_NORM_L2, &r);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 1.0, r);
}

void test_vec_norm_zero_vector_is_zero(void)
{
    numx_real_t a[] = {0.0, 0.0, 0.0};
    numx_real_t r;
    numx_vec_norm(a, 3, NUMX_NORM_L2, &r);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 0.0, r);
}

void test_vec_norm_null_a(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_vec_norm(NULL, 3, NUMX_NORM_L2, &r));
}

void test_vec_norm_null_result(void)
{
    numx_real_t a[] = {1.0, 2.0};
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_vec_norm(a, 2, NUMX_NORM_L2, NULL));
}

void test_vec_norm_unknown_type(void)
{
    numx_real_t a[] = {1.0}, r;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_vec_norm(a, 1, (numx_norm_t)99, &r));
}

/* ── numx_vec_cross3 ───────────────────────────────────────────────── */

void test_vec_cross3_x_cross_y(void)
{
    numx_real_t a[] = {1.0, 0.0, 0.0};
    numx_real_t b[] = {0.0, 1.0, 0.0};
    numx_real_t r[3];
    TEST_ASSERT_EQUAL(NUMX_OK, numx_vec_cross3(a, b, r));
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 0.0, r[0]);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 0.0, r[1]);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 1.0, r[2]);
}

void test_vec_cross3_anticommutative(void)
{
    numx_real_t a[] = {1.0, 2.0, 3.0};
    numx_real_t b[] = {4.0, 5.0, 6.0};
    numx_real_t axb[3], bxa[3];
    numx_vec_cross3(a, b, axb);
    numx_vec_cross3(b, a, bxa);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, -axb[0], bxa[0]);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, -axb[1], bxa[1]);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, -axb[2], bxa[2]);
}

void test_vec_cross3_parallel_is_zero(void)
{
    numx_real_t a[] = {2.0, 0.0, 0.0};
    numx_real_t b[] = {5.0, 0.0, 0.0};
    numx_real_t r[3];
    numx_vec_cross3(a, b, r);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 0.0, r[0]);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 0.0, r[1]);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 0.0, r[2]);
}

void test_vec_cross3_alias_safe(void)
{
    numx_real_t a[] = {1.0, 0.0, 0.0};
    numx_real_t b[] = {0.0, 1.0, 0.0};
    numx_vec_cross3(a, b, a);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 0.0, a[0]);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 0.0, a[1]);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 1.0, a[2]);
}

void test_vec_cross3_null(void)
{
    numx_real_t b[] = {1.0, 0.0, 0.0}, r[3];
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_vec_cross3(NULL, b, r));
}

/* ── numx_mat_mul ──────────────────────────────────────────────────── */

void test_mat_mul_2x2(void)
{
    numx_real_t A[] = {1.0, 2.0, 3.0, 4.0};
    numx_real_t B[] = {5.0, 6.0, 7.0, 8.0};
    numx_real_t C[4];
    TEST_ASSERT_EQUAL(NUMX_OK, numx_mat_mul(A, 2, 2, B, 2, 2, C));
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 19.0, C[0]);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 22.0, C[1]);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 43.0, C[2]);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 50.0, C[3]);
}

void test_mat_mul_2x3_times_3x2(void)
{
    numx_real_t A[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    numx_real_t B[] = {7.0, 8.0, 9.0, 10.0, 11.0, 12.0};
    numx_real_t C[4];
    TEST_ASSERT_EQUAL(NUMX_OK, numx_mat_mul(A, 2, 3, B, 3, 2, C));
    TEST_ASSERT_DOUBLE_WITHIN(TOL,  58.0, C[0]);
    TEST_ASSERT_DOUBLE_WITHIN(TOL,  64.0, C[1]);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 139.0, C[2]);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 154.0, C[3]);
}

void test_mat_mul_identity(void)
{
    numx_real_t A[] = {1.0, 2.0, 3.0, 4.0};
    numx_real_t I[] = {1.0, 0.0, 0.0, 1.0};
    numx_real_t C[4];
    numx_mat_mul(A, 2, 2, I, 2, 2, C);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, A[0], C[0]);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, A[1], C[1]);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, A[2], C[2]);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, A[3], C[3]);
}

void test_mat_mul_dim_mismatch(void)
{
    numx_real_t A[6] = {0}, B[4] = {0}, C[4] = {0};
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_mat_mul(A, 2, 3, B, 2, 2, C));
}

void test_mat_mul_null_A(void)
{
    numx_real_t B[4] = {0}, C[4] = {0};
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_mat_mul(NULL, 2, 2, B, 2, 2, C));
}

/* ── numx_mat_transpose ────────────────────────────────────────────── */

void test_mat_transpose_2x3(void)
{
    numx_real_t A[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    numx_real_t AT[6];
    TEST_ASSERT_EQUAL(NUMX_OK, numx_mat_transpose(A, 2, 3, AT));
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 1.0, AT[0]);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 4.0, AT[1]);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 2.0, AT[2]);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 5.0, AT[3]);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 3.0, AT[4]);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 6.0, AT[5]);
}

void test_mat_transpose_double_is_identity(void)
{
    numx_real_t A[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    numx_real_t AT[6], ATT[6];
    numx_size_t i;
    numx_mat_transpose(A, 2, 3, AT);
    numx_mat_transpose(AT, 3, 2, ATT);
    for (i = 0; i < 6; i++)
        TEST_ASSERT_DOUBLE_WITHIN(TOL, A[i], ATT[i]);
}

void test_mat_transpose_sq_inplace(void)
{
    numx_real_t A[] = {1.0, 2.0, 3.0, 4.0};
    TEST_ASSERT_EQUAL(NUMX_OK, numx_mat_transpose_sq(A, 2));
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 1.0, A[0]);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 3.0, A[1]);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 2.0, A[2]);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 4.0, A[3]);
}

void test_mat_transpose_sq_twice_is_identity(void)
{
    numx_real_t A[] = {1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0};
    numx_real_t orig[9];
    numx_size_t i;
    for (i = 0; i < 9; i++) orig[i] = A[i];
    numx_mat_transpose_sq(A, 3);
    numx_mat_transpose_sq(A, 3);
    for (i = 0; i < 9; i++)
        TEST_ASSERT_DOUBLE_WITHIN(TOL, orig[i], A[i]);
}

void test_mat_transpose_null(void)
{
    numx_real_t AT[4];
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_mat_transpose(NULL, 2, 2, AT));
}

void test_mat_transpose_sq_null(void)
{
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_mat_transpose_sq(NULL, 2));
}

/* ── numx_mat_det ──────────────────────────────────────────────────── */

void test_mat_det_1x1(void)
{
    numx_real_t A[] = {7.0};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_mat_det(A, 1, &r));
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 7.0, r);
}

void test_mat_det_2x2(void)
{
    numx_real_t A[] = {1.0, 2.0, 3.0, 4.0};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_mat_det(A, 2, &r));
    TEST_ASSERT_DOUBLE_WITHIN(TOL, -2.0, r);
}

void test_mat_det_3x3(void)
{
    numx_real_t A[] = {1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,10.0};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_mat_det(A, 3, &r));
    TEST_ASSERT_DOUBLE_WITHIN(TOL, -3.0, r);
}

void test_mat_det_identity_is_one(void)
{
    numx_real_t I[] = {1.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,1.0};
    numx_real_t r;
    numx_mat_det(I, 3, &r);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 1.0, r);
}

void test_mat_det_singular_is_zero(void)
{
    numx_real_t A[] = {1.0, 2.0, 2.0, 4.0};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_mat_det(A, 2, &r));
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 0.0, r);
}

void test_mat_det_null(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_mat_det(NULL, 2, &r));
}

void test_mat_det_null_result(void)
{
    numx_real_t A[] = {1.0, 0.0, 0.0, 1.0};
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_mat_det(A, 2, NULL));
}

/* ── numx_lu_decompose / numx_lu_solve ────────────────────────────── */

void test_lu_solve_3x3_textbook(void)
{
    numx_real_t A[] = { 2.0, 1.0,-1.0,
                       -3.0,-1.0, 2.0,
                       -2.0, 1.0, 2.0};
    numx_real_t b[] = {8.0,-11.0,-3.0};
    numx_real_t LU[9]; numx_idx_t pivot[3]; numx_real_t x[3];
    TEST_ASSERT_EQUAL(NUMX_OK, numx_lu_decompose(A, 3, LU, pivot));
    TEST_ASSERT_EQUAL(NUMX_OK, numx_lu_solve(LU, pivot, 3, b, x));
    TEST_ASSERT_DOUBLE_WITHIN(TOL_LU,  2.0, x[0]);
    TEST_ASSERT_DOUBLE_WITHIN(TOL_LU,  3.0, x[1]);
    TEST_ASSERT_DOUBLE_WITHIN(TOL_LU, -1.0, x[2]);
}

void test_lu_solve_identity_system(void)
{
    numx_real_t A[] = {1.0,0.0,0.0,1.0};
    numx_real_t b[] = {5.0,7.0};
    numx_real_t LU[4]; numx_idx_t pivot[2]; numx_real_t x[2];
    numx_lu_decompose(A, 2, LU, pivot);
    numx_lu_solve(LU, pivot, 2, b, x);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 5.0, x[0]);
    TEST_ASSERT_DOUBLE_WITHIN(TOL, 7.0, x[1]);
}

void test_lu_solve_residual_is_zero(void)
{
    numx_real_t A[] = { 2.0, 1.0,-1.0,
                       -3.0,-1.0, 2.0,
                       -2.0, 1.0, 2.0};
    numx_real_t b[] = {8.0,-11.0,-3.0};
    numx_real_t LU[9]; numx_idx_t pivot[3];
    numx_real_t x[3], Ax[3];
    numx_lu_decompose(A, 3, LU, pivot);
    numx_lu_solve(LU, pivot, 3, b, x);
    numx_mat_mul(A, 3, 3, x, 3, 1, Ax);
    TEST_ASSERT_DOUBLE_WITHIN(TOL_LU, b[0], Ax[0]);
    TEST_ASSERT_DOUBLE_WITHIN(TOL_LU, b[1], Ax[1]);
    TEST_ASSERT_DOUBLE_WITHIN(TOL_LU, b[2], Ax[2]);
}

void test_lu_decompose_singular(void)
{
    numx_real_t A[] = {1.0,2.0,2.0,4.0};
    numx_real_t LU[4]; numx_idx_t pivot[2];
    TEST_ASSERT_EQUAL(NUMX_ERR_SINGULAR, numx_lu_decompose(A, 2, LU, pivot));
}

void test_lu_decompose_null_A(void)
{
    numx_real_t LU[4]; numx_idx_t pivot[2];
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_lu_decompose(NULL, 2, LU, pivot));
}

void test_lu_decompose_null_LU(void)
{
    numx_real_t A[4] = {0}; numx_idx_t pivot[2] = {0};
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_lu_decompose(A, 2, NULL, pivot));
}

void test_lu_solve_null_LU(void)
{
    numx_real_t b[2] = {0}, x[2] = {0}; numx_idx_t pivot[2] = {0,1};
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_lu_solve(NULL, pivot, 2, b, x));
}

void test_lu_solve_null_x(void)
{
    numx_real_t LU[4] = {1,0,0,1}, b[2] = {1,1}; numx_idx_t pivot[2] = {0,1};
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_lu_solve(LU, pivot, 2, b, NULL));
}

/* ── Suite entry point ─────────────────────────────────────────────── */

void numx_test_linalg(void)
{
    RUN_TEST(test_vec_dot_known);
    RUN_TEST(test_vec_dot_orthogonal);
    RUN_TEST(test_vec_dot_commutative);
    RUN_TEST(test_vec_dot_self_equals_sq_norm);
    RUN_TEST(test_vec_dot_single_element);
    RUN_TEST(test_vec_dot_zero_vector);
    RUN_TEST(test_vec_dot_null_a);
    RUN_TEST(test_vec_dot_null_b);
    RUN_TEST(test_vec_dot_null_result);
    RUN_TEST(test_vec_dot_zero_n);
    RUN_TEST(test_vec_norm_l2_pythagorean);
    RUN_TEST(test_vec_norm_l1_known);
    RUN_TEST(test_vec_norm_linf_known);
    RUN_TEST(test_vec_norm_unit_vector_is_one);
    RUN_TEST(test_vec_norm_zero_vector_is_zero);
    RUN_TEST(test_vec_norm_null_a);
    RUN_TEST(test_vec_norm_null_result);
    RUN_TEST(test_vec_norm_unknown_type);
    RUN_TEST(test_vec_cross3_x_cross_y);
    RUN_TEST(test_vec_cross3_anticommutative);
    RUN_TEST(test_vec_cross3_parallel_is_zero);
    RUN_TEST(test_vec_cross3_alias_safe);
    RUN_TEST(test_vec_cross3_null);
    RUN_TEST(test_mat_mul_2x2);
    RUN_TEST(test_mat_mul_2x3_times_3x2);
    RUN_TEST(test_mat_mul_identity);
    RUN_TEST(test_mat_mul_dim_mismatch);
    RUN_TEST(test_mat_mul_null_A);
    RUN_TEST(test_mat_transpose_2x3);
    RUN_TEST(test_mat_transpose_double_is_identity);
    RUN_TEST(test_mat_transpose_sq_inplace);
    RUN_TEST(test_mat_transpose_sq_twice_is_identity);
    RUN_TEST(test_mat_transpose_null);
    RUN_TEST(test_mat_transpose_sq_null);
    RUN_TEST(test_mat_det_1x1);
    RUN_TEST(test_mat_det_2x2);
    RUN_TEST(test_mat_det_3x3);
    RUN_TEST(test_mat_det_identity_is_one);
    RUN_TEST(test_mat_det_singular_is_zero);
    RUN_TEST(test_mat_det_null);
    RUN_TEST(test_mat_det_null_result);
    RUN_TEST(test_lu_solve_3x3_textbook);
    RUN_TEST(test_lu_solve_identity_system);
    RUN_TEST(test_lu_solve_residual_is_zero);
    RUN_TEST(test_lu_decompose_singular);
    RUN_TEST(test_lu_decompose_null_A);
    RUN_TEST(test_lu_decompose_null_LU);
    RUN_TEST(test_lu_solve_null_LU);
    RUN_TEST(test_lu_solve_null_x);
}
