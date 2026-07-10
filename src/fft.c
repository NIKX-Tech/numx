/**
 * @file fft.c
 * @brief FFT implementation for numx (radix-2 Cooley-Tukey, DIT).
 */

#include "numx/fft.h"

/* ── Private helpers ───────────────────────────────────────────────── */

/* Cosine for x in [0, pi] via Taylor series with pi/2 reflection. */
static numx_real_t priv_cos(numx_real_t x)
{
    numx_real_t sign, x2, term, sum;
    int k;
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

/* sin(x) = cos(pi/2 - x); valid for x in [0, pi]. */
static numx_real_t priv_sin(numx_real_t x)
{
    numx_real_t arg = NUMX_PI * (numx_real_t)0.5 - x;
    if (arg < (numx_real_t)0.0)
        arg = -arg;
    return priv_cos(arg);
}

/* Newton-Raphson square root. */
static numx_real_t priv_sqrt(numx_real_t x)
{
    numx_real_t r;
    int k;
    if (x <= (numx_real_t)0.0)
        return (numx_real_t)0.0;
    r = x;
    for (k = 0; k < 64; k++)
        r = (numx_real_t)0.5 * (r + x / r);
    return r;
}

/* In-place bit-reversal permutation for n complex samples (float32). */
static void priv_bit_reverse_f32(numx_real_t *x, numx_size_t n)
{
    numx_size_t i, j, bit;
    numx_real_t tr, ti;
    j = 0;
    for (i = 1; i < n; i++)
    {
        bit = n >> 1;
        while (j & bit)
        {
            j ^= bit;
            bit >>= 1;
        }
        j ^= bit;
        if (i < j)
        {
            tr = x[2 * i];
            ti = x[2 * i + 1];
            x[2 * i] = x[2 * j];
            x[2 * i + 1] = x[2 * j + 1];
            x[2 * j] = tr;
            x[2 * j + 1] = ti;
        }
    }
}

/* In-place bit-reversal permutation for n complex samples (Q15). */
static void priv_bit_reverse_q15(numx_q15_t *x, numx_size_t n)
{
    numx_size_t i, j, bit;
    numx_q15_t tr, ti;
    j = 0;
    for (i = 1; i < n; i++)
    {
        bit = n >> 1;
        while (j & bit)
        {
            j ^= bit;
            bit >>= 1;
        }
        j ^= bit;
        if (i < j)
        {
            tr = x[2 * i];
            ti = x[2 * i + 1];
            x[2 * i] = x[2 * j];
            x[2 * i + 1] = x[2 * j + 1];
            x[2 * j] = tr;
            x[2 * j + 1] = ti;
        }
    }
}

/* Q1.15 multiply with rounding: (a * b + 0.5) >> 15. */
static numx_q15_t priv_q15_mul(numx_q15_t a, numx_q15_t b)
{
    int32_t prod = (int32_t)a * (int32_t)b;
    return (numx_q15_t)((prod + (1 << 14)) >> 15);
}

/* Validate n: non-zero, power of two, within limit. */
static int priv_valid_n(numx_size_t n)
{
    return (n > 0) && (n <= NUMX_MAX_FFT_SIZE) && ((n & (n - 1)) == 0);
}

/* ── Float32 FFT ───────────────────────────────────────────────────── */

numx_status_t numx_fft_f32(numx_real_t *inout, numx_size_t n)
{
    numx_size_t s, m, half_m, k, j, u, v;
    numx_real_t base_angle, angle, w_re, w_im, t_re, t_im, u_re, u_im;
    if (!inout)
        return NUMX_ERR_NULL_PTR;
    if (!priv_valid_n(n))
        return NUMX_ERR_INVALID_ARG;

    priv_bit_reverse_f32(inout, n);

    for (s = 1; ((numx_size_t)1 << s) <= n; s++)
    {
        m = (numx_size_t)1 << s;
        half_m = m >> 1;
        base_angle = NUMX_PI / (numx_real_t)half_m;

        for (k = 0; k < n; k += m)
        {
            for (j = 0; j < half_m; j++)
            {
                angle = (numx_real_t)j * base_angle;
                w_re = priv_cos(angle);
                w_im = -priv_sin(angle);

                u = k + j;
                v = k + j + half_m;

                t_re = w_re * inout[2 * v] - w_im * inout[2 * v + 1];
                t_im = w_re * inout[2 * v + 1] + w_im * inout[2 * v];

                u_re = inout[2 * u];
                u_im = inout[2 * u + 1];

                inout[2 * v] = u_re - t_re;
                inout[2 * v + 1] = u_im - t_im;
                inout[2 * u] = u_re + t_re;
                inout[2 * u + 1] = u_im + t_im;
            }
        }
    }
    return NUMX_OK;
}

/* ── Float32 IFFT ──────────────────────────────────────────────────── */

numx_status_t numx_ifft_f32(numx_real_t *inout, numx_size_t n)
{
    numx_size_t i;
    numx_real_t inv_n;
    numx_status_t s;
    if (!inout)
        return NUMX_ERR_NULL_PTR;
    if (!priv_valid_n(n))
        return NUMX_ERR_INVALID_ARG;

    /* Conjugate input */
    for (i = 0; i < n; i++)
        inout[2 * i + 1] = -inout[2 * i + 1];

    /* Forward FFT */
    s = numx_fft_f32(inout, n);
    if (s != NUMX_OK)
        return s;

    /* Conjugate output and scale by 1/n */
    inv_n = (numx_real_t)1.0 / (numx_real_t)n;
    for (i = 0; i < n; i++)
    {
        inout[2 * i] = inout[2 * i] * inv_n;
        inout[2 * i + 1] = -inout[2 * i + 1] * inv_n;
    }
    return NUMX_OK;
}

/* ── Q15 FFT ───────────────────────────────────────────────────────── */

numx_status_t numx_fft_q15(numx_q15_t *inout, numx_size_t n)
{
    numx_size_t s, m, half_m, k, j, u, v;
    numx_real_t base_angle, angle;
    numx_q15_t w_re, w_im, t_re, t_im, u_re, u_im;
    if (!inout)
        return NUMX_ERR_NULL_PTR;
    if (!priv_valid_n(n))
        return NUMX_ERR_INVALID_ARG;

    priv_bit_reverse_q15(inout, n);

    for (s = 1; ((numx_size_t)1 << s) <= n; s++)
    {
        m = (numx_size_t)1 << s;
        half_m = m >> 1;
        base_angle = NUMX_PI / (numx_real_t)half_m;

        for (k = 0; k < n; k += m)
        {
            for (j = 0; j < half_m; j++)
            {
                angle = (numx_real_t)j * base_angle;
                w_re = (numx_q15_t)(priv_cos(angle) * (numx_real_t)32767.0);
                w_im = (numx_q15_t)(-priv_sin(angle) * (numx_real_t)32767.0);

                u = k + j;
                v = k + j + half_m;

                t_re = (numx_q15_t)((int32_t)priv_q15_mul(w_re, inout[2 * v]) - (int32_t)priv_q15_mul(w_im, inout[2 * v + 1]));
                t_im = (numx_q15_t)((int32_t)priv_q15_mul(w_re, inout[2 * v + 1]) + (int32_t)priv_q15_mul(w_im, inout[2 * v]));

                u_re = inout[2 * u];
                u_im = inout[2 * u + 1];

                /* Right-shift by 1 at each stage prevents overflow. */
                inout[2 * v] = (numx_q15_t)(((int32_t)u_re - (int32_t)t_re) >> 1);
                inout[2 * v + 1] = (numx_q15_t)(((int32_t)u_im - (int32_t)t_im) >> 1);
                inout[2 * u] = (numx_q15_t)(((int32_t)u_re + (int32_t)t_re) >> 1);
                inout[2 * u + 1] = (numx_q15_t)(((int32_t)u_im + (int32_t)t_im) >> 1);
            }
        }
    }
    return NUMX_OK;
}

/* ── Magnitude spectrum ────────────────────────────────────────────── */

numx_status_t numx_fft_magnitude(
    const numx_real_t *fft_out, numx_size_t n,
    numx_real_t *mag)
{
    numx_size_t k;
    numx_real_t re, im;
    if (!fft_out || !mag)
        return NUMX_ERR_NULL_PTR;
    if (n < 2 || !priv_valid_n(n))
        return NUMX_ERR_INVALID_ARG;
    for (k = 0; k <= n / 2; k++)
    {
        re = fft_out[2 * k];
        im = fft_out[2 * k + 1];
        mag[k] = priv_sqrt(re * re + im * im);
    }
    return NUMX_OK;
}
