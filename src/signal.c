/**
 * @file signal.c
 * @brief Signal processing implementation for numx.
 */

#include "numx/signal.h"

/* ── Private helpers ───────────────────────────────────────────────── */

/* Cosine for any non-negative angle via period reduction + Taylor series.
 * Handles up to 4*pi (sufficient for Blackman window arguments). */
static numx_real_t priv_cos_full(numx_real_t x)
{
    numx_real_t two_pi = (numx_real_t)2.0 * NUMX_PI;
    numx_real_t sign, x2, term, sum;
    int k;
    while (x > two_pi)
        x -= two_pi;
    if (x > NUMX_PI)
        x = two_pi - x;
    sign = (numx_real_t)1.0;
    if (x > NUMX_PI * (numx_real_t)0.5)
    {
        x = NUMX_PI - x;
        sign = -(numx_real_t)1.0;
    }
    x2 = x * x;
    term = (numx_real_t)1.0;
    sum = term;
    for (k = 1; k <= 12; k++)
    {
        term *= -x2 / (numx_real_t)((2 * k - 1) * (2 * k));
        sum += term;
    }
    return sign * sum;
}

/* ── Window functions ──────────────────────────────────────────────── */

numx_status_t numx_signal_window_rect(numx_size_t n, numx_real_t *out)
{
    numx_size_t i;
    if (!out)
        return NUMX_ERR_NULL_PTR;
    if (n == 0)
        return NUMX_ERR_INVALID_ARG;
    for (i = 0; i < n; i++)
        out[i] = (numx_real_t)1.0;
    return NUMX_OK;
}

numx_status_t numx_signal_window_hann(numx_size_t n, numx_real_t *out)
{
    numx_size_t i;
    numx_real_t nm1, arg;
    if (!out)
        return NUMX_ERR_NULL_PTR;
    if (n == 0)
        return NUMX_ERR_INVALID_ARG;
    if (n == 1)
    {
        out[0] = (numx_real_t)1.0;
        return NUMX_OK;
    }
    nm1 = (numx_real_t)(n - 1);
    for (i = 0; i < n; i++)
    {
        arg = (numx_real_t)2.0 * NUMX_PI * (numx_real_t)i / nm1;
        out[i] = (numx_real_t)0.5 * ((numx_real_t)1.0 - priv_cos_full(arg));
    }
    return NUMX_OK;
}

numx_status_t numx_signal_window_hamming(numx_size_t n, numx_real_t *out)
{
    numx_size_t i;
    numx_real_t nm1, arg;
    if (!out)
        return NUMX_ERR_NULL_PTR;
    if (n == 0)
        return NUMX_ERR_INVALID_ARG;
    if (n == 1)
    {
        out[0] = (numx_real_t)1.0;
        return NUMX_OK;
    }
    nm1 = (numx_real_t)(n - 1);
    for (i = 0; i < n; i++)
    {
        arg = (numx_real_t)2.0 * NUMX_PI * (numx_real_t)i / nm1;
        out[i] = (numx_real_t)0.54 - (numx_real_t)0.46 * priv_cos_full(arg);
    }
    return NUMX_OK;
}

numx_status_t numx_signal_window_blackman(numx_size_t n, numx_real_t *out)
{
    numx_size_t i;
    numx_real_t nm1, arg1, arg2;
    if (!out)
        return NUMX_ERR_NULL_PTR;
    if (n == 0)
        return NUMX_ERR_INVALID_ARG;
    if (n == 1)
    {
        out[0] = (numx_real_t)1.0;
        return NUMX_OK;
    }
    nm1 = (numx_real_t)(n - 1);
    for (i = 0; i < n; i++)
    {
        arg1 = (numx_real_t)2.0 * NUMX_PI * (numx_real_t)i / nm1;
        arg2 = (numx_real_t)2.0 * arg1;
        out[i] = (numx_real_t)0.42 - (numx_real_t)0.5 * priv_cos_full(arg1) + (numx_real_t)0.08 * priv_cos_full(arg2);
    }
    return NUMX_OK;
}

/* ── Convolution ───────────────────────────────────────────────────── */

numx_status_t numx_signal_convolve(
    const numx_real_t *x, numx_size_t xn,
    const numx_real_t *h, numx_size_t hn,
    numx_real_t *out)
{
    numx_size_t k, j, jmin, jmax, out_n;
    numx_real_t sum, c, y, t;
    if (!x || !h || !out)
        return NUMX_ERR_NULL_PTR;
    if (xn == 0 || hn == 0)
        return NUMX_ERR_INVALID_ARG;
    out_n = xn + hn - 1;
    for (k = 0; k < out_n; k++)
    {
        /* Kahan (compensated) summation, the inner loop has no
         * compile-time cap, so accumulated rounding error can grow
         * unboundedly with naive summation for large xn/hn. */
        sum = (numx_real_t)0.0;
        c = (numx_real_t)0.0;
        jmin = (k >= hn - 1) ? k - (hn - 1) : 0;
        jmax = (k < xn) ? k : xn - 1;
        for (j = jmin; j <= jmax; j++)
        {
            y = x[j] * h[k - j] - c;
            t = sum + y;
            c = (t - sum) - y;
            sum = t;
        }
        out[k] = sum;
    }
    return NUMX_OK;
}

/* ── Cross-correlation ─────────────────────────────────────────────── */

numx_status_t numx_signal_correlate(
    const numx_real_t *x, numx_size_t xn,
    const numx_real_t *y, numx_size_t yn,
    numx_real_t *out)
{
    numx_size_t k, j, jmin, jmax, out_n;
    numx_real_t sum, c, term, t;
    if (!x || !y || !out)
        return NUMX_ERR_NULL_PTR;
    if (xn == 0 || yn == 0)
        return NUMX_ERR_INVALID_ARG;
    out_n = xn + yn - 1;
    for (k = 0; k < out_n; k++)
    {
        /* Kahan (compensated) summation, the inner loop has no
         * compile-time cap, so accumulated rounding error can grow
         * unboundedly with naive summation for large xn/yn. */
        sum = (numx_real_t)0.0;
        c = (numx_real_t)0.0;
        jmin = (k >= yn - 1) ? k - (yn - 1) : 0;
        jmax = (k < xn) ? k : xn - 1;
        for (j = jmin; j <= jmax; j++)
        {
            /* yi always in [0, yn-1] here */
            term = x[j] * y[j + yn - 1 - k] - c;
            t = sum + term;
            c = (t - sum) - term;
            sum = t;
        }
        out[k] = sum;
    }
    return NUMX_OK;
}

/* ── FIR filter ────────────────────────────────────────────────────── */

numx_status_t numx_signal_fir(
    const numx_real_t *x, numx_size_t xn,
    const numx_real_t *taps, numx_size_t ntaps,
    numx_real_t *out)
{
    numx_size_t n, k;
    numx_real_t acc, c, y, t;
    if (!x || !taps || !out)
        return NUMX_ERR_NULL_PTR;
    if (xn == 0 || ntaps == 0 || ntaps > NUMX_MAX_FIR_TAPS)
        return NUMX_ERR_INVALID_ARG;
    for (n = 0; n < xn; n++)
    {
        /* Kahan (compensated) summation, keeps accumulated rounding
         * error from growing as ntaps approaches NUMX_MAX_FIR_TAPS. */
        acc = (numx_real_t)0.0;
        c = (numx_real_t)0.0;
        for (k = 0; k < ntaps; k++)
        {
            if (n >= k)
            {
                y = taps[k] * x[n - k] - c;
                t = acc + y;
                c = (t - acc) - y;
                acc = t;
            }
        }
        out[n] = acc;
    }
    return NUMX_OK;
}

/* ── IIR biquad filter ─────────────────────────────────────────────── */

numx_status_t numx_signal_iir_biquad(
    const numx_real_t *x, numx_size_t n,
    const numx_real_t b[3],
    const numx_real_t a[2],
    numx_real_t *out)
{
    numx_size_t i;
    numx_real_t d0, d1, d2;
    if (!x || !b || !a || !out)
        return NUMX_ERR_NULL_PTR;
    if (n == 0)
        return NUMX_ERR_INVALID_ARG;
    d1 = (numx_real_t)0.0;
    d2 = (numx_real_t)0.0;
    for (i = 0; i < n; i++)
    {
        d0 = x[i] - a[0] * d1 - a[1] * d2;
        out[i] = b[0] * d0 + b[1] * d1 + b[2] * d2;
        d2 = d1;
        d1 = d0;
    }
    return NUMX_OK;
}

/* ── Peak detection ────────────────────────────────────────────────── */

numx_status_t numx_signal_peaks(
    const numx_real_t *x, numx_size_t n,
    numx_size_t *peaks, numx_size_t max_peaks,
    numx_size_t *npeaks)
{
    numx_size_t i;
    if (!x || !peaks || !npeaks)
        return NUMX_ERR_NULL_PTR;
    *npeaks = 0;
    if (n < 3)
        return NUMX_OK;
    for (i = 1; i < n - 1; i++)
    {
        if (x[i] > x[i - 1] && x[i] > x[i + 1])
        {
            if (*npeaks >= max_peaks)
                return NUMX_ERR_BUFFER_SMALL;
            peaks[(*npeaks)++] = i;
        }
    }
    return NUMX_OK;
}

/* ── Exponential moving average ────────────────────────────────────── */

numx_status_t numx_signal_ema(
    const numx_real_t *x, numx_size_t n,
    numx_real_t alpha,
    numx_real_t *out)
{
    numx_size_t i;
    numx_real_t one_minus;
    if (!x || !out)
        return NUMX_ERR_NULL_PTR;
    if (n == 0 || alpha < (numx_real_t)0.0 || alpha > (numx_real_t)1.0)
        return NUMX_ERR_INVALID_ARG;
    one_minus = (numx_real_t)1.0 - alpha;
    out[0] = x[0];
    for (i = 1; i < n; i++)
        out[i] = alpha * x[i] + one_minus * out[i - 1];
    return NUMX_OK;
}
