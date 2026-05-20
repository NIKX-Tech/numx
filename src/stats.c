/**
 * @file stats.c
 * @brief Descriptive statistics implementation for numx.
 */

#include "numx/stats.h"

/* ── Private helpers ───────────────────────────────────────────────── */

static numx_size_t priv_partition(numx_real_t *a, numx_size_t lo, numx_size_t hi)
{
    numx_real_t pivot = a[hi];
    numx_real_t tmp;
    numx_size_t i = lo, j;
    for (j = lo; j < hi; j++)
    {
        if (a[j] <= pivot)
        {
            tmp = a[i];
            a[i] = a[j];
            a[j] = tmp;
            i++;
        }
    }
    tmp = a[i];
    a[i] = a[hi];
    a[hi] = tmp;
    return i;
}

/* Quickselect: rearranges a[lo..hi] and returns a[k] as if sorted. */
static numx_real_t priv_quickselect(numx_real_t *a, numx_size_t n, numx_size_t k)
{
    numx_size_t lo = 0, hi = n - 1, idx;
    while (lo < hi)
    {
        idx = priv_partition(a, lo, hi);
        if (idx == k)
            break;
        else if (idx < k)
            lo = idx + 1;
        else
            hi = idx - 1;
    }
    return a[k];
}

/* ── Public functions ──────────────────────────────────────────────── */

numx_status_t numx_stats_mean(
    const numx_real_t *a,
    numx_size_t n,
    numx_real_t *result)
{
    numx_size_t i;
    numx_real_t sum;

    if (!a || !result)
        return NUMX_ERR_NULL_PTR;
    if (n == 0 || n > NUMX_MAX_VEC_SIZE)
        return NUMX_ERR_INVALID_ARG;

    sum = (numx_real_t)0.0;
    for (i = 0; i < n; i++)
        sum += a[i];
    *result = sum / (numx_real_t)n;
    return NUMX_OK;
}

numx_status_t numx_stats_variance(
    const numx_real_t *a,
    numx_size_t n,
    numx_var_type_t type,
    numx_real_t *result)
{
    numx_size_t i;
    numx_real_t mean, delta, delta2, M2;

    if (!a || !result)
        return NUMX_ERR_NULL_PTR;
    if (n == 0 || n > NUMX_MAX_VEC_SIZE)
        return NUMX_ERR_INVALID_ARG;
    if (type == NUMX_VAR_SAMPLE && n < 2)
        return NUMX_ERR_INVALID_ARG;

    /* Welford's online algorithm — numerically stable. */
    mean = (numx_real_t)0.0;
    M2 = (numx_real_t)0.0;
    for (i = 0; i < n; i++)
    {
        delta = a[i] - mean;
        mean += delta / (numx_real_t)(i + 1);
        delta2 = a[i] - mean;
        M2 += delta * delta2;
    }

    *result = (type == NUMX_VAR_SAMPLE)
                  ? M2 / (numx_real_t)(n - 1)
                  : M2 / (numx_real_t)n;
    return NUMX_OK;
}

numx_status_t numx_stats_median(
    const numx_real_t *a,
    numx_size_t n,
    numx_real_t *result)
{
    numx_real_t buf[NUMX_MAX_VEC_SIZE];
    numx_size_t i, mid;

    if (!a || !result)
        return NUMX_ERR_NULL_PTR;
    if (n == 0 || n > NUMX_MAX_VEC_SIZE)
        return NUMX_ERR_INVALID_ARG;

    for (i = 0; i < n; i++)
        buf[i] = a[i];

    mid = n / 2;
    if (n & 1u)
    {
        *result = priv_quickselect(buf, n, mid);
    }
    else
    {
        numx_real_t lo = priv_quickselect(buf, n, mid - 1);
        numx_real_t hi = priv_quickselect(buf, n, mid);
        *result = (lo + hi) * (numx_real_t)0.5;
    }
    return NUMX_OK;
}

numx_status_t numx_stats_percentile(
    const numx_real_t *a,
    numx_size_t n,
    numx_real_t p,
    numx_real_t *result)
{
    numx_real_t buf[NUMX_MAX_VEC_SIZE];
    numx_size_t i, k;

    if (!a || !result)
        return NUMX_ERR_NULL_PTR;
    if (n == 0 || n > NUMX_MAX_VEC_SIZE)
        return NUMX_ERR_INVALID_ARG;
    if (p < (numx_real_t)0.0 || p > (numx_real_t)100.0)
        return NUMX_ERR_INVALID_ARG;

    for (i = 0; i < n; i++)
        buf[i] = a[i];

    /* Nearest-rank method: k = ceil(p/100 * n) - 1, clamped. */
    k = (numx_size_t)(p * (numx_real_t)n / (numx_real_t)100.0);
    if (k >= n)
        k = n - 1;

    *result = priv_quickselect(buf, n, k);
    return NUMX_OK;
}
