/**
 * @file signal.h
 * @brief Signal processing functions for numx.
 *
 * Provides window functions, linear convolution, cross-correlation, FIR and
 * IIR (biquad) filtering, peak detection, and exponential moving average.
 * All operations are allocation-free and work on caller-supplied buffers.
 */

#ifndef NUMX_SIGNAL_H
#define NUMX_SIGNAL_H

#include "numx_config.h"
#include "numx_types.h"

/* ── Window functions ──────────────────────────────────────────────── */

/**
 * @brief Fill out[0..n-1] with a rectangular window (all ones).
 *
 * @param[in]  n    Number of samples. Must be >= 1.
 * @param[out] out  Output buffer of length n. Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if out is NULL.
 *         NUMX_ERR_INVALID_ARG if n == 0.
 */
numx_status_t numx_signal_window_rect(numx_size_t n, numx_real_t *out);

/**
 * @brief Fill out[0..n-1] with a Hann window.
 *
 * w[i] = 0.5 * (1 - cos(2*pi*i / (n-1)))
 *
 * Endpoints are exactly 0; centre is exactly 1.
 *
 * @param[in]  n    Number of samples. Must be >= 1.
 * @param[out] out  Output buffer of length n. Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if out is NULL.
 *         NUMX_ERR_INVALID_ARG if n == 0.
 */
numx_status_t numx_signal_window_hann(numx_size_t n, numx_real_t *out);

/**
 * @brief Fill out[0..n-1] with a Hamming window.
 *
 * w[i] = 0.54 - 0.46 * cos(2*pi*i / (n-1))
 *
 * @param[in]  n    Number of samples. Must be >= 1.
 * @param[out] out  Output buffer of length n. Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if out is NULL.
 *         NUMX_ERR_INVALID_ARG if n == 0.
 */
numx_status_t numx_signal_window_hamming(numx_size_t n, numx_real_t *out);

/**
 * @brief Fill out[0..n-1] with a Blackman window.
 *
 * w[i] = 0.42 - 0.5*cos(2*pi*i/(n-1)) + 0.08*cos(4*pi*i/(n-1))
 *
 * @param[in]  n    Number of samples. Must be >= 1.
 * @param[out] out  Output buffer of length n. Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if out is NULL.
 *         NUMX_ERR_INVALID_ARG if n == 0.
 */
numx_status_t numx_signal_window_blackman(numx_size_t n, numx_real_t *out);

/* ── Convolution and correlation ───────────────────────────────────── */

/**
 * @brief Linear convolution of x[xn] and h[hn].
 *
 * out[k] = sum_{j} x[j] * h[k-j], for k = 0..(xn+hn-2).
 * The caller must supply out with at least xn + hn - 1 elements.
 *
 * @param[in]  x    First signal of length xn. Must not be NULL.
 * @param[in]  xn   Length of x. Must be >= 1.
 * @param[in]  h    Second signal (kernel) of length hn. Must not be NULL.
 * @param[in]  hn   Length of h. Must be >= 1.
 * @param[out] out  Output buffer of length xn + hn - 1. Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if any pointer is NULL.
 *         NUMX_ERR_INVALID_ARG if xn == 0 or hn == 0.
 */
numx_status_t numx_signal_convolve(
    const numx_real_t *x, numx_size_t xn,
    const numx_real_t *h, numx_size_t hn,
    numx_real_t *out);

/**
 * @brief Cross-correlation of x[xn] with y[yn].
 *
 * out[k] = sum_{j} x[j] * y[j - (k - (yn-1))], for k = 0..(xn+yn-2).
 * Lag 0 is at index yn-1. Equivalent to convolving x with y time-reversed.
 * The caller must supply out with at least xn + yn - 1 elements.
 *
 * @param[in]  x    First signal of length xn. Must not be NULL.
 * @param[in]  xn   Length of x. Must be >= 1.
 * @param[in]  y    Second signal of length yn. Must not be NULL.
 * @param[in]  yn   Length of y. Must be >= 1.
 * @param[out] out  Output buffer of length xn + yn - 1. Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if any pointer is NULL.
 *         NUMX_ERR_INVALID_ARG if xn == 0 or yn == 0.
 */
numx_status_t numx_signal_correlate(
    const numx_real_t *x, numx_size_t xn,
    const numx_real_t *y, numx_size_t yn,
    numx_real_t *out);

/* ── FIR filter ────────────────────────────────────────────────────── */

/**
 * @brief Apply a causal FIR filter to x[xn].
 *
 * out[n] = sum_{k=0}^{ntaps-1} taps[k] * x[n-k], with x[n-k] = 0 for n < k.
 * Output has the same length as the input (xn samples).
 *
 * @param[in]  x      Input signal of length xn. Must not be NULL.
 * @param[in]  xn     Length of x. Must be >= 1.
 * @param[in]  taps   Filter coefficients, length ntaps. Must not be NULL.
 * @param[in]  ntaps  Number of taps. Must be in [1, NUMX_MAX_FIR_TAPS].
 * @param[out] out    Output buffer of length xn. Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if any pointer is NULL.
 *         NUMX_ERR_INVALID_ARG if xn == 0 or ntaps == 0 or ntaps > NUMX_MAX_FIR_TAPS.
 */
numx_status_t numx_signal_fir(
    const numx_real_t *x, numx_size_t xn,
    const numx_real_t *taps, numx_size_t ntaps,
    numx_real_t *out);

/* ── IIR biquad filter ─────────────────────────────────────────────── */

/**
 * @brief Apply a second-order IIR (biquad) filter using Direct Form II.
 *
 * Transfer function: H(z) = (b[0] + b[1]*z^-1 + b[2]*z^-2)
 *                          / (1   + a[0]*z^-1 + a[1]*z^-2)
 *
 * The denominator a[0] coefficient is normalised to 1.0 and excluded.
 * Initial state is zero (no warm-up).
 *
 * @param[in]  x   Input signal of length n. Must not be NULL.
 * @param[in]  n   Number of samples. Must be >= 1.
 * @param[in]  b   Numerator coefficients {b0, b1, b2}. Must not be NULL.
 * @param[in]  a   Denominator feedback coefficients {a1, a2}. Must not be NULL.
 * @param[out] out Output buffer of length n. Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if any pointer is NULL.
 *         NUMX_ERR_INVALID_ARG if n == 0.
 */
numx_status_t numx_signal_iir_biquad(
    const numx_real_t *x, numx_size_t n,
    const numx_real_t b[3],
    const numx_real_t a[2],
    numx_real_t *out);

/* ── Peak detection ────────────────────────────────────────────────── */

/**
 * @brief Find indices of strict local maxima in x[n].
 *
 * A sample x[i] is a peak if x[i] > x[i-1] and x[i] > x[i+1].
 * Endpoints (i=0 and i=n-1) are never peaks.
 *
 * @param[in]  x          Input signal of length n. Must not be NULL.
 * @param[in]  n          Number of samples. Signals shorter than 3 produce npeaks=0.
 * @param[out] peaks      Array to store peak indices. Must not be NULL.
 * @param[in]  max_peaks  Capacity of peaks[]. Must be >= 1.
 * @param[out] npeaks     Number of peaks found. Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if x, peaks, or npeaks is NULL.
 *         NUMX_ERR_BUFFER_SMALL if more than max_peaks peaks exist.
 */
numx_status_t numx_signal_peaks(
    const numx_real_t *x, numx_size_t n,
    numx_size_t *peaks, numx_size_t max_peaks,
    numx_size_t *npeaks);

/* ── Exponential moving average ────────────────────────────────────── */

/**
 * @brief Compute the exponential moving average of x[n].
 *
 * out[0] = x[0]
 * out[i] = alpha * x[i] + (1 - alpha) * out[i-1]
 *
 * @param[in]  x      Input signal of length n. Must not be NULL.
 * @param[in]  n      Number of samples. Must be >= 1.
 * @param[in]  alpha  Smoothing factor in [0, 1].
 * @param[out] out    Output buffer of length n. Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if x or out is NULL.
 *         NUMX_ERR_INVALID_ARG if n == 0 or alpha not in [0, 1].
 */
numx_status_t numx_signal_ema(
    const numx_real_t *x, numx_size_t n,
    numx_real_t alpha,
    numx_real_t *out);

#endif /* NUMX_SIGNAL_H */
