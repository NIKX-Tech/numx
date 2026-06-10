/**
 * @file test_stats.c
 * @brief Statistics tests — float32 / x86 host build.
 *        numx_real_t = float  (NUMX_USE_DOUBLE not set)
 */

#include "unity.h"
#include "numx/numx_types.h"
#include "numx/stats.h"

#define TOL 1e-4f

void test_stats_mean_known(void)
{
    numx_real_t a[] = {1.0f,2.0f,3.0f,4.0f,5.0f};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_stats_mean(a, 5, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 3.0f, r);
}

void test_stats_mean_negative_values(void)
{
    numx_real_t a[] = {-3.0f,-1.0f,1.0f,3.0f};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_stats_mean(a, 4, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 0.0f, r);
}

void test_stats_mean_arithmetic_sequence(void)
{
    numx_real_t a[] = {1.0f,2.0f,3.0f,4.0f,5.0f,6.0f,7.0f};
    numx_real_t r;
    numx_stats_mean(a, 7, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 4.0f, r);
}

void test_stats_mean_constant_array(void)
{
    numx_real_t a[] = {5.0f,5.0f,5.0f,5.0f};
    numx_real_t r;
    numx_stats_mean(a, 4, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 5.0f, r);
}

void test_stats_mean_single_element(void)
{
    numx_real_t a[] = {42.0f};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_stats_mean(a, 1, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 42.0f, r);
}

void test_stats_mean_null_a(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_stats_mean(NULL, 3, &r));
}

void test_stats_mean_null_result(void)
{
    numx_real_t a[] = {1.0f,2.0f};
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_stats_mean(a, 2, NULL));
}

void test_stats_mean_n_zero(void)
{
    numx_real_t a[] = {1.0f}, r;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_stats_mean(a, 0, &r));
}

void test_stats_variance_pop_known(void)
{
    numx_real_t a[] = {2.0f,4.0f,4.0f,4.0f,5.0f,5.0f,7.0f,9.0f};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_stats_variance(a, 8, NUMX_VAR_POPULATION, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 4.0f, r);
}

void test_stats_variance_sample_known(void)
{
    numx_real_t a[] = {2.0f,4.0f,4.0f,4.0f,5.0f,5.0f,7.0f,9.0f};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_stats_variance(a, 8, NUMX_VAR_SAMPLE, &r));
    TEST_ASSERT_FLOAT_WITHIN(1e-3f, 32.0f/7.0f, r);
}

void test_stats_variance_constant_is_zero(void)
{
    numx_real_t a[] = {7.0f,7.0f,7.0f,7.0f};
    numx_real_t r;
    numx_stats_variance(a, 4, NUMX_VAR_POPULATION, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 0.0f, r);
}

void test_stats_variance_sample_ge_population(void)
{
    numx_real_t a[] = {1.0f,2.0f,3.0f,4.0f,5.0f};
    numx_real_t rpop, rsamp;
    numx_stats_variance(a, 5, NUMX_VAR_POPULATION, &rpop);
    numx_stats_variance(a, 5, NUMX_VAR_SAMPLE, &rsamp);
    TEST_ASSERT_TRUE(rsamp > rpop);
}

void test_stats_variance_sample_n1_rejected(void)
{
    numx_real_t a[] = {1.0f}, r;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_stats_variance(a, 1, NUMX_VAR_SAMPLE, &r));
}

void test_stats_variance_null(void)
{
    numx_real_t a[] = {1.0f,2.0f}, r;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
                      numx_stats_variance(NULL, 2, NUMX_VAR_POPULATION, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
                      numx_stats_variance(a, 2, NUMX_VAR_POPULATION, NULL));
}

void test_stats_median_odd_known(void)
{
    numx_real_t a[] = {5.0f,3.0f,1.0f,4.0f,2.0f};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_stats_median(a, 5, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 3.0f, r);
}

void test_stats_median_even_known(void)
{
    numx_real_t a[] = {4.0f,1.0f,3.0f,2.0f};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_stats_median(a, 4, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 2.5f, r);
}

void test_stats_median_sorted_middle(void)
{
    numx_real_t a[] = {10.0f,20.0f,30.0f,40.0f,50.0f};
    numx_real_t r;
    numx_stats_median(a, 5, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 30.0f, r);
}

void test_stats_median_does_not_modify_input(void)
{
    numx_real_t a[] = {5.0f,3.0f,1.0f};
    numx_real_t r;
    numx_stats_median(a, 3, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 5.0f, a[0]);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 3.0f, a[1]);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 1.0f, a[2]);
}

void test_stats_median_single_element(void)
{
    numx_real_t a[] = {7.0f};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_stats_median(a, 1, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 7.0f, r);
}

void test_stats_median_two_elements(void)
{
    numx_real_t a[] = {3.0f,1.0f};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_stats_median(a, 2, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 2.0f, r);
}

void test_stats_median_null(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_stats_median(NULL, 3, &r));
    numx_real_t a[] = {1.0f};
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_stats_median(a, 1, NULL));
}

void test_stats_percentile_0_is_min(void)
{
    numx_real_t a[] = {5.0f,3.0f,1.0f,4.0f,2.0f};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_stats_percentile(a, 5, 0.0f, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 1.0f, r);
}

void test_stats_percentile_100_is_max(void)
{
    numx_real_t a[] = {5.0f,3.0f,1.0f,4.0f,2.0f};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_stats_percentile(a, 5, 100.0f, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL, 5.0f, r);
}

void test_stats_percentile_does_not_modify_input(void)
{
    numx_real_t a[] = {3.0f,1.0f,2.0f};
    numx_real_t r;
    numx_stats_percentile(a, 3, 50.0f, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 3.0f, a[0]);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 1.0f, a[1]);
    TEST_ASSERT_FLOAT_WITHIN(TOL, 2.0f, a[2]);
}

void test_stats_percentile_null(void)
{
    numx_real_t a[] = {1.0f,2.0f}, r;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_stats_percentile(NULL, 2, 50.0f, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_stats_percentile(a, 2, 50.0f, NULL));
}

void test_stats_percentile_out_of_range(void)
{
    numx_real_t a[] = {1.0f,2.0f,3.0f}, r;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_stats_percentile(a, 3, -1.0f, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_stats_percentile(a, 3, 101.0f, &r));
}

void numx_test_stats(void)
{
    RUN_TEST(test_stats_mean_known);
    RUN_TEST(test_stats_mean_negative_values);
    RUN_TEST(test_stats_mean_arithmetic_sequence);
    RUN_TEST(test_stats_mean_constant_array);
    RUN_TEST(test_stats_mean_single_element);
    RUN_TEST(test_stats_mean_null_a);
    RUN_TEST(test_stats_mean_null_result);
    RUN_TEST(test_stats_mean_n_zero);
    RUN_TEST(test_stats_variance_pop_known);
    RUN_TEST(test_stats_variance_sample_known);
    RUN_TEST(test_stats_variance_constant_is_zero);
    RUN_TEST(test_stats_variance_sample_ge_population);
    RUN_TEST(test_stats_variance_sample_n1_rejected);
    RUN_TEST(test_stats_variance_null);
    RUN_TEST(test_stats_median_odd_known);
    RUN_TEST(test_stats_median_even_known);
    RUN_TEST(test_stats_median_sorted_middle);
    RUN_TEST(test_stats_median_does_not_modify_input);
    RUN_TEST(test_stats_median_single_element);
    RUN_TEST(test_stats_median_two_elements);
    RUN_TEST(test_stats_median_null);
    RUN_TEST(test_stats_percentile_0_is_min);
    RUN_TEST(test_stats_percentile_100_is_max);
    RUN_TEST(test_stats_percentile_does_not_modify_input);
    RUN_TEST(test_stats_percentile_null);
    RUN_TEST(test_stats_percentile_out_of_range);
}
