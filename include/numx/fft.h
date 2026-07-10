/**
 * @file fft.h
 * @brief Fast Fourier Transform for numx.
 *
 * Provides a radix-2 Cooley-Tukey FFT/IFFT for float32 complex data and a
 * scaled Q15 fixed-point FFT. All transforms operate in-place on interleaved
 * complex buffers [re0, im0, re1, im1, ...] of length 2*n.
 *
 * Constraints:
 *   - n must be a power of two and <= NUMX_MAX_FFT_SIZE.
 *   - The Q15 FFT applies a right-shift of 1 at every butterfly stage, so the
 *     output is scaled by 1/n relative to the float32 FFT.
 */

#ifndef NUMX_FFT_H
#define NUMX_FFT_H

#include "numx_config.h"
#include "numx_types.h"

/**
 * @brief In-place float32 FFT.
 *
 * Computes the DFT X[k] = sum_{n=0}^{N-1} x[n] * exp(-2*pi*i*k*n/N).
 * Input and output are interleaved complex: inout = [re0, im0, re1, im1, ...].
 *
 * @param[in,out] inout  Complex buffer of length 2*n. Must not be NULL.
 * @param[in]     n      Transform size. Must be a power of two, 1 <= n <= NUMX_MAX_FFT_SIZE.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if inout is NULL.
 *         NUMX_ERR_INVALID_ARG if n == 0, n > NUMX_MAX_FFT_SIZE, or n is not a power of two.
 */
numx_status_t numx_fft_f32(numx_real_t *inout, numx_size_t n);

/**
 * @brief In-place float32 IFFT.
 *
 * Computes the inverse DFT x[n] = (1/N) * sum_{k=0}^{N-1} X[k] * exp(2*pi*i*k*n/N).
 * Input and output are interleaved complex: inout = [re0, im0, re1, im1, ...].
 *
 * @param[in,out] inout  Complex buffer of length 2*n. Must not be NULL.
 * @param[in]     n      Transform size. Must be a power of two, 1 <= n <= NUMX_MAX_FFT_SIZE.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if inout is NULL.
 *         NUMX_ERR_INVALID_ARG if n == 0, n > NUMX_MAX_FFT_SIZE, or n is not a power of two.
 */
numx_status_t numx_ifft_f32(numx_real_t *inout, numx_size_t n);

/**
 * @brief In-place Q15 fixed-point FFT (scaled).
 *
 * Computes the DFT in Q1.15 fixed-point arithmetic. A right-shift by 1 is
 * applied at each butterfly stage to prevent overflow, so the output is
 * scaled by 1/n compared to the mathematical DFT. Input and output are
 * interleaved Q15 complex pairs: inout = [re0, im0, re1, im1, ...].
 *
 * @param[in,out] inout  Q15 complex buffer of length 2*n. Must not be NULL.
 * @param[in]     n      Transform size. Must be a power of two, 1 <= n <= NUMX_MAX_FFT_SIZE.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if inout is NULL.
 *         NUMX_ERR_INVALID_ARG if n == 0, n > NUMX_MAX_FFT_SIZE, or n is not a power of two.
 */
numx_status_t numx_fft_q15(numx_q15_t *inout, numx_size_t n);

/**
 * @brief Compute the magnitude spectrum from FFT output.
 *
 * mag[k] = sqrt(re[k]^2 + im[k]^2) for k = 0 .. n/2.
 * Produces n/2 + 1 values covering DC to Nyquist (single-sided).
 * The caller must supply mag with at least n/2 + 1 elements.
 *
 * @param[in]  fft_out  Interleaved complex FFT output, length 2*n. Must not be NULL.
 * @param[in]  n        Transform size. Must be a power of two, 2 <= n <= NUMX_MAX_FFT_SIZE.
 * @param[out] mag      Output buffer of length n/2 + 1. Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if fft_out or mag is NULL.
 *         NUMX_ERR_INVALID_ARG if n < 2, n > NUMX_MAX_FFT_SIZE, or n is not a power of two.
 */
numx_status_t numx_fft_magnitude(
    const numx_real_t *fft_out, numx_size_t n,
    numx_real_t *mag);

#endif /* NUMX_FFT_H */
