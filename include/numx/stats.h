/**
 * @file stats.h
 * @brief Descriptive statistics for numx.
 *
 * All functions operate on caller-supplied flat arrays. Median and
 * percentile copy the input to a stack buffer (max NUMX_MAX_VEC_SIZE
 * elements) to avoid modifying the caller's data.
 */

#ifndef NUMX_STATS_H
#define NUMX_STATS_H

#include "numx_types.h"
#include "numx_config.h"

/* ── Variance type ─────────────────────────────────────────────────── */

typedef enum
{
    NUMX_VAR_POPULATION = 0, /* divide by n     */
    NUMX_VAR_SAMPLE = 1      /* divide by n - 1 */
} numx_var_type_t;

/* ── Functions ─────────────────────────────────────────────────────── */

/**
 * @brief Compute the arithmetic mean of a vector.
 *
 * Mathematical definition:
 *   mean = (1/n) * sum_i a[i]
 *
 * @param[in]  a       Input vector. Must not be NULL.
 * @param[in]  n       Number of elements. Must be > 0 and <= NUMX_MAX_VEC_SIZE.
 * @param[out] result  Pointer to store the mean. Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if a or result is NULL.
 *         NUMX_ERR_INVALID_ARG if n == 0 or n > NUMX_MAX_VEC_SIZE.
 */
numx_status_t numx_stats_mean(
    const numx_real_t *a,
    numx_size_t n,
    numx_real_t *result);

/**
 * @brief Compute population or sample variance using Welford's algorithm.
 *
 * Welford's online algorithm is numerically stable even for large n or
 * large values, unlike the naive sum-of-squares approach.
 *
 * Mathematical definition:
 *   variance_pop  = (1/n)   * sum_i (a[i] - mean)^2
 *   variance_samp = (1/n-1) * sum_i (a[i] - mean)^2
 *
 * @param[in]  a       Input vector. Must not be NULL.
 * @param[in]  n       Number of elements. Must be >= 2 for NUMX_VAR_SAMPLE.
 * @param[in]  type    NUMX_VAR_POPULATION or NUMX_VAR_SAMPLE.
 * @param[out] result  Pointer to store the variance. Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if a or result is NULL.
 *         NUMX_ERR_INVALID_ARG if n == 0, or n < 2 when type == NUMX_VAR_SAMPLE.
 */
numx_status_t numx_stats_variance(
    const numx_real_t *a,
    numx_size_t n,
    numx_var_type_t type,
    numx_real_t *result);

/**
 * @brief Compute the median of a vector.
 *
 * Copies input to a stack buffer and uses quickselect — O(n) average,
 * O(1) extra heap. For even n, returns the mean of the two middle values.
 *
 * Mathematical definition:
 *   median = sorted(a)[n/2]           for odd n
 *          = (sorted(a)[n/2-1] + sorted(a)[n/2]) / 2  for even n
 *
 * @param[in]  a       Input vector. Must not be NULL. Not modified.
 * @param[in]  n       Number of elements. Must be > 0 and <= NUMX_MAX_VEC_SIZE.
 * @param[out] result  Pointer to store the median. Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if a or result is NULL.
 *         NUMX_ERR_INVALID_ARG if n == 0 or n > NUMX_MAX_VEC_SIZE.
 */
numx_status_t numx_stats_median(
    const numx_real_t *a,
    numx_size_t n,
    numx_real_t *result);

/**
 * @brief Compute the p-th percentile of a vector (0 <= p <= 100).
 *
 * Uses nearest-rank method. p = 0 returns the minimum; p = 100 the maximum.
 * Copies input to a stack buffer — input is not modified.
 *
 * @param[in]  a       Input vector. Must not be NULL. Not modified.
 * @param[in]  n       Number of elements. Must be > 0 and <= NUMX_MAX_VEC_SIZE.
 * @param[in]  p       Percentile in [0, 100].
 * @param[out] result  Pointer to store the percentile value. Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR    if a or result is NULL.
 *         NUMX_ERR_INVALID_ARG if n == 0, n > NUMX_MAX_VEC_SIZE, or p not in [0,100].
 */
numx_status_t numx_stats_percentile(
    const numx_real_t *a,
    numx_size_t n,
    numx_real_t p,
    numx_real_t *result);

#endif /* NUMX_STATS_H */
