/**
 * @file ntt.c
 * @brief NTT implementation for numx (Kyber/Dilithium parameters).
 *
 * Ring: Z_3329[x]/(x^256+1). Generator: omega=17 (primitive 256th root,
 * 17^128 = -1 mod 3329). Seven CT butterfly stages forward; seven GS stages
 * inverse. Barrett reduction throughout; no heap; no division.
 *
 * Twiddle tables are precomputed (omega^{brv7(k)} mod 3329, k=0..127):
 *   priv_zetas     — forward CT twiddles,  ω^{brv7(k)} in [0, q-1]
 *   priv_inv_zetas — inverse GS twiddles,  (ω^{brv7(k)})^{-1} in [0, q-1]
 *   priv_basemul   — pointwise-mul factors, NTT(x^2)[2i] in [0, q-1]
 *
 * Barrett constants: v = ceil(2^26/3329) = 20159; shift = 26.
 * Normalization:     n_inv = 128^{-1} mod 3329 = 3303.
 */

#include "numx/ntt.h"

/* ── Domain constants ──────────────────────────────────────────────── */

#define NTT_Q       3329
#define NTT_N       256
#define NTT_N_INV   3303   /* 128^{-1} mod 3329 */
#define NTT_BVAL    20159  /* ceil(2^26 / 3329) for Barrett reduction  */
#define NTT_BSHIFT  26

/* ── Precomputed twiddle tables ────────────────────────────────────── */

/* Forward NTT twiddles: omega^{brv7(k)} mod 3329, k = 0..127 */
static const int16_t priv_zetas[128] = {
        1, 1729, 2580, 3289, 2642,  630, 1897,  848,
     1062, 1919,  193,  797, 2786, 3260,  569, 1746,
      296, 2447, 1339, 1476, 3046,   56, 2240, 1333,
     1426, 2094,  535, 2882, 2393, 2879, 1974,  821,
      289,  331, 3253, 1756, 1197, 2304, 2277, 2055,
      650, 1977, 2513,  632, 2865,   33, 1320, 1915,
     2319, 1435,  807,  452, 1438, 2868, 1534, 2402,
     2647, 2617, 1481,  648, 2474, 3110, 1227,  910,
       17, 2761,  583, 2649, 1637,  723, 2288, 1100,
     1409, 2662, 3281,  233,  756, 2156, 3015, 3050,
     1703, 1651, 2789, 1789, 1847,  952, 1461, 2687,
      939, 2308, 2437, 2388,  733, 2337,  268,  641,
     1584, 2298, 2037, 3220,  375, 2549, 2090, 1645,
     1063,  319, 2773,  757, 2099,  561, 2466, 2594,
     2804, 1092,  403, 1026, 1143, 2150, 2775,  886,
     1722, 1212, 1874, 1029, 2110, 2935,  885, 2154,
};

/* Inverse NTT twiddles: (omega^{brv7(k)})^{-1} mod 3329, k = 0..127 */
static const int16_t priv_inv_zetas[128] = {
        1, 1600,   40,  749, 2481, 1432, 2699,  687,
     1583, 2760,   69,  543, 2532, 3136, 1410, 2267,
     2508, 1355,  450,  936,  447, 2794, 1235, 1903,
     1996, 1089, 3273,  283, 1853, 1990,  882, 3033,
     2419, 2102,  219,  855, 2681, 1848,  712,  682,
      927, 1795,  461, 1891, 2877, 2522, 1894, 1010,
     1414, 2009, 3296,  464, 2697,  816, 1352, 2679,
     1274, 1052, 1025, 2132, 1573,   76, 2998, 3040,
     1175, 2444,  394, 1219, 2300, 1455, 2117, 1607,
     2443,  554, 1179, 2186, 2303, 2926, 2237,  525,
      735,  863, 2768, 1230, 2572,  556, 3010, 2266,
     1684, 1239,  780, 2954,  109, 1292, 1031, 1745,
     2688, 3061,  992, 2596,  941,  892, 1021, 2390,
      642, 1868, 2377, 1482, 1540,  540, 1678, 1626,
      279,  314, 1173, 2573, 3096,   48,  667, 1920,
     2229, 1041, 2606, 1692,  680, 2746,  568, 3312,
};

/* Pointwise-mul factors: NTT(x^2)[2i] in [0, q-1], i = 0..127 */
static const int16_t priv_basemul[128] = {
       17, 3312, 2761,  568,  583, 2746, 2649,  680,
     1637, 1692,  723, 2606, 2288, 1041, 1100, 2229,
     1409, 1920, 2662,  667, 3281,   48,  233, 3096,
      756, 2573, 2156, 1173, 3015,  314, 3050,  279,
     1703, 1626, 1651, 1678, 2789,  540, 1789, 1540,
     1847, 1482,  952, 2377, 1461, 1868, 2687,  642,
      939, 2390, 2308, 1021, 2437,  892, 2388,  941,
      733, 2596, 2337,  992,  268, 3061,  641, 2688,
     1584, 1745, 2298, 1031, 2037, 1292, 3220,  109,
      375, 2954, 2549,  780, 2090, 1239, 1645, 1684,
     1063, 2266,  319, 3010, 2773,  556,  757, 2572,
     2099, 1230,  561, 2768, 2466,  863, 2594,  735,
     2804,  525, 1092, 2237,  403, 2926, 1026, 2303,
     1143, 2186, 2150, 1179, 2775,  554,  886, 2443,
     1722, 1607, 1212, 2117, 1874, 1455, 1029, 2300,
     2110, 1219, 2935,  394,  885, 2444, 2154, 1175,
};

/* ── Private helpers ───────────────────────────────────────────────── */

/*
 * Barrett reduction mod 3329 for inputs in [0, 2*q^2].
 * v = ceil(2^26/3329) = 20159.
 */
static int16_t priv_barrett(int32_t a)
{
    int32_t t;
    int16_t r;
    t = (int32_t)(((uint64_t)(uint32_t)a * 20159u) >> 26u);
    r = (int16_t)(a - t * 3329);
    if (r < 0)     r = (int16_t)(r + 3329);
    if (r >= 3329) r = (int16_t)(r - 3329);
    return r;
}

/* Single field multiplication reduced to [0, q-1]. */
static int16_t priv_fqmul(int16_t a, int16_t b)
{
    return priv_barrett((int32_t)a * (int32_t)b);
}

/* ── Forward NTT ───────────────────────────────────────────────────── */

numx_status_t numx_ntt_forward(numx_q15_t *f)
{
    numx_size_t len, start, j;
    int k;
    int16_t zeta, t;

    if (!f)
        return NUMX_ERR_NULL_PTR;

    k = 1;
    for (len = 128; len >= 2; len >>= 1)
    {
        for (start = 0; start < (numx_size_t)NTT_N; start += 2u * len)
        {
            zeta = priv_zetas[k++];
            for (j = start; j < start + len; j++)
            {
                t       = priv_fqmul(zeta, f[j + len]);
                f[j + len] = priv_barrett((int32_t)f[j] - t + NTT_Q);
                f[j]       = priv_barrett((int32_t)f[j] + t);
            }
        }
    }
    return NUMX_OK;
}

/* ── Inverse NTT ───────────────────────────────────────────────────── */

numx_status_t numx_ntt_inverse(numx_q15_t *f)
{
    numx_size_t len, start, j;
    int k;
    int16_t w_inv, t;

    if (!f)
        return NUMX_ERR_NULL_PTR;

    /*
     * Stages are processed in reverse order (len = 2, 4, ..., 128).
     * Within each stage, k indices go FORWARD (same as forward NTT)
     * so that each butterfly group uses its original twiddle's inverse.
     *
     * The k range for each len:
     *   len=128: k=1        len=64:  k=2..3
     *   len=32:  k=4..7     len=16:  k=8..15
     *   len=8:   k=16..31   len=4:   k=32..63
     *   len=2:   k=64..127
     *
     * k_start = 128 / len (1, 2, 4, 8, 16, 32, 64 for len=128..2).
     */
    for (len = 2; len <= 128; len <<= 1)
    {
        k = (int)(128u / len);   /* k_start for this stage */
        for (start = 0; start < (numx_size_t)NTT_N; start += 2u * len)
        {
            w_inv = priv_inv_zetas[k++];
            for (j = start; j < start + len; j++)
            {
                t          = f[j];
                f[j]       = priv_barrett((int32_t)t + f[j + len]);
                f[j + len] = priv_fqmul(w_inv, (int16_t)((int32_t)t - f[j + len] + NTT_Q));
            }
        }
    }

    /* Normalization: multiply by 128^{-1} = 3303 mod 3329 */
    for (j = 0; j < (numx_size_t)NTT_N; j++)
        f[j] = priv_fqmul((int16_t)NTT_N_INV, f[j]);

    return NUMX_OK;
}

/* ── Pointwise multiplication ──────────────────────────────────────── */

numx_status_t numx_ntt_pointwise_mul(
    const numx_q15_t *a,
    const numx_q15_t *b,
    numx_q15_t       *out
)
{
    numx_size_t i;
    int16_t a0, a1, b0, b1, c, t;

    if (!a || !b || !out)
        return NUMX_ERR_NULL_PTR;

    for (i = 0; i < 128u; i++)
    {
        a0 = a[2u * i];  a1 = a[2u * i + 1u];
        b0 = b[2u * i];  b1 = b[2u * i + 1u];
        c  = priv_basemul[i];

        /* basemul in Z_q[x]/(x^2 - c_i):
         *   out[2i]   = a0*b0 + c*a1*b1
         *   out[2i+1] = a0*b1 + a1*b0      */
        t              = priv_fqmul(a1, b1);
        out[2u * i]     = priv_barrett((int32_t)priv_fqmul(a0, b0) + (int32_t)c * t);
        out[2u * i + 1u] = priv_barrett((int32_t)priv_fqmul(a0, b1) + (int32_t)priv_fqmul(a1, b0));
    }
    return NUMX_OK;
}

/* ── Full polynomial multiplication ───────────────────────────────── */

numx_status_t numx_ntt_polymul(
    const numx_q15_t *a,
    const numx_q15_t *b,
    numx_q15_t       *out
)
{
    numx_q15_t ah[NTT_N], bh[NTT_N];
    numx_size_t i;
    numx_status_t s;

    if (!a || !b || !out)
        return NUMX_ERR_NULL_PTR;

    for (i = 0; i < (numx_size_t)NTT_N; i++) { ah[i] = a[i]; bh[i] = b[i]; }

    s = numx_ntt_forward(ah);
    if (s != NUMX_OK) return s;

    s = numx_ntt_forward(bh);
    if (s != NUMX_OK) return s;

    s = numx_ntt_pointwise_mul(ah, bh, out);
    if (s != NUMX_OK) return s;

    return numx_ntt_inverse(out);
}

/* ── Coefficient reduction ─────────────────────────────────────────── */

numx_status_t numx_ntt_reduce(numx_q15_t *f)
{
    numx_size_t i;

    if (!f)
        return NUMX_ERR_NULL_PTR;

    for (i = 0; i < (numx_size_t)NTT_N; i++)
        f[i] = priv_barrett((int32_t)f[i]);

    return NUMX_OK;
}

/* ── Polynomial addition ───────────────────────────────────────────── */

numx_status_t numx_ntt_poly_add(
    const numx_q15_t *a,
    const numx_q15_t *b,
    numx_q15_t       *out
)
{
    numx_size_t i;

    if (!a || !b || !out)
        return NUMX_ERR_NULL_PTR;

    for (i = 0; i < (numx_size_t)NTT_N; i++)
        out[i] = priv_barrett((int32_t)a[i] + b[i]);

    return NUMX_OK;
}

/* ── Polynomial subtraction ────────────────────────────────────────── */

numx_status_t numx_ntt_poly_sub(
    const numx_q15_t *a,
    const numx_q15_t *b,
    numx_q15_t       *out
)
{
    numx_size_t i;

    if (!a || !b || !out)
        return NUMX_ERR_NULL_PTR;

    for (i = 0; i < (numx_size_t)NTT_N; i++)
        out[i] = priv_barrett((int32_t)a[i] - b[i] + NTT_Q);

    return NUMX_OK;
}
