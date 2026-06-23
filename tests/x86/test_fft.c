/**
 * @file test_fft.c
 * @brief FFT tests — float32 / x86 host build.
 *        numx_real_t = float  (NUMX_USE_DOUBLE not set)
 *
 * Twiddle-factor tolerance is loose ((numx_real_t)1e-3): priv_cos/sin uses
 * a Taylor approximation rather than a hardware FPU instruction.
 */

#include "unity.h"
#include "numx/fft.h"

#define TOL       (numx_real_t)1e-3
#define TOL_TIGHT (numx_real_t)1e-4

void test_fft_dc_n4(void)
{
    numx_real_t x[] = {(numx_real_t)1.0,(numx_real_t)0.0, (numx_real_t)1.0,(numx_real_t)0.0, (numx_real_t)1.0,(numx_real_t)0.0, (numx_real_t)1.0,(numx_real_t)0.0};
    TEST_ASSERT_EQUAL(NUMX_OK, numx_fft_f32(x, 4));
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, (numx_real_t)4.0, x[0]);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, (numx_real_t)0.0, x[1]);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, (numx_real_t)0.0, x[2]);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, (numx_real_t)0.0, x[4]);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, (numx_real_t)0.0, x[6]);
}

void test_fft_single_tone_n4(void)
{
    numx_real_t x[] = {(numx_real_t)1.0,(numx_real_t)0.0, (numx_real_t)0.0,(numx_real_t)1.0, -(numx_real_t)1.0,(numx_real_t)0.0, (numx_real_t)0.0,-(numx_real_t)1.0};
    TEST_ASSERT_EQUAL(NUMX_OK, numx_fft_f32(x, 4));
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)0.0, x[0]);
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)4.0, x[2]);
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)0.0, x[3]);
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)0.0, x[4]);
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)0.0, x[6]);
}

void test_fft_dc_n2(void)
{
    numx_real_t x[] = {(numx_real_t)1.0,(numx_real_t)0.0, (numx_real_t)1.0,(numx_real_t)0.0};
    TEST_ASSERT_EQUAL(NUMX_OK, numx_fft_f32(x, 2));
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, (numx_real_t)2.0, x[0]);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, (numx_real_t)0.0, x[1]);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, (numx_real_t)0.0, x[2]);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, (numx_real_t)0.0, x[3]);
}

void test_fft_n1_passthrough(void)
{
    numx_real_t x[] = {(numx_real_t)3.5, -(numx_real_t)1.2};
    TEST_ASSERT_EQUAL(NUMX_OK, numx_fft_f32(x, 1));
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT,  (numx_real_t)3.5, x[0]);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, -(numx_real_t)1.2, x[1]);
}

void test_fft_delta_at_0(void)
{
    numx_real_t x[] = {(numx_real_t)4.0,(numx_real_t)0.0, (numx_real_t)0.0,(numx_real_t)0.0, (numx_real_t)0.0,(numx_real_t)0.0, (numx_real_t)0.0,(numx_real_t)0.0};
    TEST_ASSERT_EQUAL(NUMX_OK, numx_fft_f32(x, 4));
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, (numx_real_t)4.0, x[0]);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, (numx_real_t)4.0, x[2]);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, (numx_real_t)4.0, x[4]);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, (numx_real_t)4.0, x[6]);
}

void test_fft_null_returns_error(void)
{
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_fft_f32(NULL, 4));
}

void test_fft_n_not_power_of_two(void)
{
    numx_real_t x[6] = {0};
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_fft_f32(x, 3));
}

void test_fft_n_exceeds_max(void)
{
    numx_real_t x[2] = {0};
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_fft_f32(x, NUMX_MAX_FFT_SIZE*2));
}

void test_ifft_roundtrip(void)
{
    numx_real_t orig[] = {(numx_real_t)1.0,(numx_real_t)0.0, (numx_real_t)2.0,(numx_real_t)0.0, (numx_real_t)3.0,(numx_real_t)0.0, (numx_real_t)0.0,(numx_real_t)0.0};
    numx_real_t x[]    = {(numx_real_t)1.0,(numx_real_t)0.0, (numx_real_t)2.0,(numx_real_t)0.0, (numx_real_t)3.0,(numx_real_t)0.0, (numx_real_t)0.0,(numx_real_t)0.0};
    numx_size_t i;
    TEST_ASSERT_EQUAL(NUMX_OK, numx_fft_f32(x, 4));
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ifft_f32(x, 4));
    for (i = 0; i < 8; i++) TEST_ASSERT_FLOAT_WITHIN(TOL, orig[i], x[i]);
}

void test_ifft_dc_spectrum(void)
{
    numx_real_t x[] = {(numx_real_t)4.0,(numx_real_t)0.0, (numx_real_t)0.0,(numx_real_t)0.0, (numx_real_t)0.0,(numx_real_t)0.0, (numx_real_t)0.0,(numx_real_t)0.0};
    TEST_ASSERT_EQUAL(NUMX_OK, numx_ifft_f32(x, 4));
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)1.0, x[0]);
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)0.0, x[1]);
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)1.0, x[2]);
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)1.0, x[4]);
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)1.0, x[6]);
}

void test_ifft_null_returns_error(void)
{
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_ifft_f32(NULL, 4));
}

void test_fft_q15_dc_n4(void)
{
    numx_q15_t x[] = {0x4000,0, 0x4000,0, 0x4000,0, 0x4000,0};
    TEST_ASSERT_EQUAL(NUMX_OK, numx_fft_q15(x, 4));
    TEST_ASSERT_INT16_WITHIN(2, 0x4000, x[0]);
    TEST_ASSERT_INT16_WITHIN(2, 0,      x[1]);
    TEST_ASSERT_INT16_WITHIN(2, 0, x[2]);
    TEST_ASSERT_INT16_WITHIN(2, 0, x[4]);
    TEST_ASSERT_INT16_WITHIN(2, 0, x[6]);
}

void test_fft_q15_null_returns_error(void)
{
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_fft_q15(NULL, 4));
}

void test_fft_q15_n_not_power_of_two(void)
{
    numx_q15_t x[6] = {0};
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_fft_q15(x, 3));
}

void test_fft_magnitude_dc(void)
{
    numx_real_t fft_out[] = {(numx_real_t)4.0,(numx_real_t)0.0, (numx_real_t)0.0,(numx_real_t)0.0, (numx_real_t)0.0,(numx_real_t)0.0, (numx_real_t)0.0,(numx_real_t)0.0};
    numx_real_t mag[3];
    TEST_ASSERT_EQUAL(NUMX_OK, numx_fft_magnitude(fft_out, 4, mag));
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, (numx_real_t)4.0, mag[0]);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, (numx_real_t)0.0, mag[1]);
    TEST_ASSERT_FLOAT_WITHIN(TOL_TIGHT, (numx_real_t)0.0, mag[2]);
}

void test_fft_magnitude_complex_bin(void)
{
    numx_real_t fft_out[] = {(numx_real_t)3.0,(numx_real_t)4.0, (numx_real_t)0.0,(numx_real_t)0.0, (numx_real_t)0.0,(numx_real_t)0.0};
    numx_real_t mag[2];
    TEST_ASSERT_EQUAL(NUMX_OK, numx_fft_magnitude(fft_out, 2, mag));
    TEST_ASSERT_FLOAT_WITHIN(TOL, (numx_real_t)5.0, mag[0]);
}

void test_fft_magnitude_null_returns_error(void)
{
    numx_real_t fft_out[4]={0}, mag[3]={0};
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_fft_magnitude(NULL, 4, mag));
    TEST_ASSERT_EQUAL(NUMX_ERR_NULL_PTR, numx_fft_magnitude(fft_out, 4, NULL));
}

void test_fft_magnitude_n1_invalid(void)
{
    numx_real_t fft_out[2]={0}, mag[1]={0};
    TEST_ASSERT_EQUAL(NUMX_ERR_INVALID_ARG, numx_fft_magnitude(fft_out, 1, mag));
}

void numx_test_fft(void)
{
    RUN_TEST(test_fft_dc_n4);
    RUN_TEST(test_fft_single_tone_n4);
    RUN_TEST(test_fft_dc_n2);
    RUN_TEST(test_fft_n1_passthrough);
    RUN_TEST(test_fft_delta_at_0);
    RUN_TEST(test_fft_null_returns_error);
    RUN_TEST(test_fft_n_not_power_of_two);
    RUN_TEST(test_fft_n_exceeds_max);
    RUN_TEST(test_ifft_roundtrip);
    RUN_TEST(test_ifft_dc_spectrum);
    RUN_TEST(test_ifft_null_returns_error);
    RUN_TEST(test_fft_q15_dc_n4);
    RUN_TEST(test_fft_q15_null_returns_error);
    RUN_TEST(test_fft_q15_n_not_power_of_two);
    RUN_TEST(test_fft_magnitude_dc);
    RUN_TEST(test_fft_magnitude_complex_bin);
    RUN_TEST(test_fft_magnitude_null_returns_error);
    RUN_TEST(test_fft_magnitude_n1_invalid);
}
