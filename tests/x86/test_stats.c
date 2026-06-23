/**
 * @file test_stats.c
 * @brief Statistics tests — float32 / x86 host build.
 *        numx_real_t = float  (NUMX_USE_DOUBLE not set)
 */

#include "unity.h"
#include "numx/numx_types.h"
#include "numx/stats.h"

#define TOL (numx_real_t)1e-4

void test_stats_mean_known(void)
{
    numx_real_t a[] = {(numx_real_t)1.0,(numx_real_t)2.0,(numx_real_t)3.0,(numx_real_t)4.0,(numx_real_t)5.0};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_stats_mean(a, 5, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)3.0, r);
}

void test_stats_mean_negative_values(void)
{
    numx_real_t a[] = {-(numx_real_t)3.0,-(numx_real_t)1.0,(numx_real_t)1.0,(numx_real_t)3.0};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_stats_mean(a, 4, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)0.0, r);
}

void test_stats_mean_arithmetic_sequence(void)
{
    numx_real_t a[] = {(numx_real_t)1.0,(numx_real_t)2.0,(numx_real_t)3.0,(numx_real_t)4.0,(numx_real_t)5.0,(numx_real_t)6.0,(numx_real_t)7.0};
    numx_real_t r;
    numx_stats_mean(a, 7, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)4.0, r);
}

void test_stats_mean_constant_array(void)
{
    numx_real_t a[] = {(numx_real_t)5.0,(numx_real_t)5.0,(numx_real_t)5.0,(numx_real_t)5.0};
    numx_real_t r;
    numx_stats_mean(a, 4, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)5.0, r);
}

void test_stats_mean_single_element(void)
{
    numx_real_t a[] = {(numx_real_t)42.0};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_stats_mean(a, 1, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)42.0, r);
}

void test_stats_mean_null_a(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_stats_mean(NULL, 3, &r));
}

void test_stats_mean_null_result(void)
{
    numx_real_t a[] = {(numx_real_t)1.0,(numx_real_t)2.0};
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_stats_mean(a, 2, NULL));
}

void test_stats_mean_n_zero(void)
{
    numx_real_t a[] = {(numx_real_t)1.0}, r;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_stats_mean(a, 0, &r));
}

void test_stats_variance_pop_known(void)
{
    numx_real_t a[] = {(numx_real_t)2.0,(numx_real_t)4.0,(numx_real_t)4.0,(numx_real_t)4.0,(numx_real_t)5.0,(numx_real_t)5.0,(numx_real_t)7.0,(numx_real_t)9.0};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_stats_variance(a, 8, NUMX_VAR_POPULATION, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)4.0, r);
}

void test_stats_variance_sample_known(void)
{
    numx_real_t a[] = {(numx_real_t)2.0,(numx_real_t)4.0,(numx_real_t)4.0,(numx_real_t)4.0,(numx_real_t)5.0,(numx_real_t)5.0,(numx_real_t)7.0,(numx_real_t)9.0};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_stats_variance(a, 8, NUMX_VAR_SAMPLE, &r));
    TEST_ASSERT_FLOAT_WITHIN((numx_real_t)1e-3, (numx_real_t)32.0/(numx_real_t)7.0, r);
}

void test_stats_variance_constant_is_zero(void)
{
    numx_real_t a[] = {(numx_real_t)7.0,(numx_real_t)7.0,(numx_real_t)7.0,(numx_real_t)7.0};
    numx_real_t r;
    numx_stats_variance(a, 4, NUMX_VAR_POPULATION, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)0.0, r);
}

void test_stats_variance_sample_ge_population(void)
{
    numx_real_t a[] = {(numx_real_t)1.0,(numx_real_t)2.0,(numx_real_t)3.0,(numx_real_t)4.0,(numx_real_t)5.0};
    numx_real_t rpop, rsamp;
    numx_stats_variance(a, 5, NUMX_VAR_POPULATION, &rpop);
    numx_stats_variance(a, 5, NUMX_VAR_SAMPLE, &rsamp);
    TEST_ASSERT_TRUE(rsamp > rpop);
}

void test_stats_variance_sample_n1_rejected(void)
{
    numx_real_t a[] = {(numx_real_t)1.0}, r;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG,
                      numx_stats_variance(a, 1, NUMX_VAR_SAMPLE, &r));
}

void test_stats_variance_null(void)
{
    numx_real_t a[] = {(numx_real_t)1.0,(numx_real_t)2.0}, r;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
                      numx_stats_variance(NULL, 2, NUMX_VAR_POPULATION, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR,
                      numx_stats_variance(a, 2, NUMX_VAR_POPULATION, NULL));
}

void test_stats_median_odd_known(void)
{
    numx_real_t a[] = {(numx_real_t)5.0,(numx_real_t)3.0,(numx_real_t)1.0,(numx_real_t)4.0,(numx_real_t)2.0};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_stats_median(a, 5, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)3.0, r);
}

void test_stats_median_even_known(void)
{
    numx_real_t a[] = {(numx_real_t)4.0,(numx_real_t)1.0,(numx_real_t)3.0,(numx_real_t)2.0};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_stats_median(a, 4, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)2.5, r);
}

void test_stats_median_sorted_middle(void)
{
    numx_real_t a[] = {(numx_real_t)10.0,(numx_real_t)20.0,(numx_real_t)30.0,(numx_real_t)40.0,(numx_real_t)50.0};
    numx_real_t r;
    numx_stats_median(a, 5, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)30.0, r);
}

void test_stats_median_does_not_modify_input(void)
{
    numx_real_t a[] = {(numx_real_t)5.0,(numx_real_t)3.0,(numx_real_t)1.0};
    numx_real_t r;
    numx_stats_median(a, 3, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)5.0, a[0]);
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)3.0, a[1]);
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)1.0, a[2]);
}

void test_stats_median_single_element(void)
{
    numx_real_t a[] = {(numx_real_t)7.0};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_stats_median(a, 1, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)7.0, r);
}

void test_stats_median_two_elements(void)
{
    numx_real_t a[] = {(numx_real_t)3.0,(numx_real_t)1.0};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_stats_median(a, 2, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)2.0, r);
}

void test_stats_median_null(void)
{
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_stats_median(NULL, 3, &r));
    numx_real_t a[] = {(numx_real_t)1.0};
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_stats_median(a, 1, NULL));
}

void test_stats_percentile_0_is_min(void)
{
    numx_real_t a[] = {(numx_real_t)5.0,(numx_real_t)3.0,(numx_real_t)1.0,(numx_real_t)4.0,(numx_real_t)2.0};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_stats_percentile(a, 5, (numx_real_t)0.0, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)1.0, r);
}

void test_stats_percentile_100_is_max(void)
{
    numx_real_t a[] = {(numx_real_t)5.0,(numx_real_t)3.0,(numx_real_t)1.0,(numx_real_t)4.0,(numx_real_t)2.0};
    numx_real_t r;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_stats_percentile(a, 5, (numx_real_t)100.0, &r));
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)5.0, r);
}

void test_stats_percentile_does_not_modify_input(void)
{
    numx_real_t a[] = {(numx_real_t)3.0,(numx_real_t)1.0,(numx_real_t)2.0};
    numx_real_t r;
    numx_stats_percentile(a, 3, (numx_real_t)50.0, &r);
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)3.0, a[0]);
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)1.0, a[1]);
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)2.0, a[2]);
}

void test_stats_percentile_null(void)
{
    numx_real_t a[] = {(numx_real_t)1.0,(numx_real_t)2.0}, r;
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_stats_percentile(NULL, 2, (numx_real_t)50.0, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_stats_percentile(a, 2, (numx_real_t)50.0, NULL));
}

void test_stats_percentile_out_of_range(void)
{
    numx_real_t a[] = {(numx_real_t)1.0,(numx_real_t)2.0,(numx_real_t)3.0}, r;
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_stats_percentile(a, 3, -(numx_real_t)1.0, &r));
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_stats_percentile(a, 3, (numx_real_t)101.0, &r));
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
