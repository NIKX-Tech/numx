# Descriptive Statistics

**Module:** `numx/stats.h` | **Category:** Foundation | **Phase:** P1.09–P1.12

---

## What it solves

One-pass summary statistics on flat arrays: mean, variance (population or sample), median, and arbitrary percentiles. All algorithms avoid heap allocation; median and percentile copy data to a stack buffer so the caller's array is never modified.

---

## Mathematical formulations

### Arithmetic mean

$$\mu = \frac{1}{n} \sum_{i=0}^{n-1} a_i$$

### Variance — Welford's online algorithm

The naïve two-pass formula $\sigma^2 = \frac{1}{n}\sum (a_i - \mu)^2$ suffers catastrophic cancellation for large or close-valued inputs. Welford's single-pass recurrence avoids this:

$$\delta_i = a_i - \mu_{i-1}, \qquad \mu_i = \mu_{i-1} + \frac{\delta_i}{i}, \qquad M_i = M_{i-1} + \delta_i \,(a_i - \mu_i)$$

After $n$ steps:

$$\sigma^2_{\text{pop}} = \frac{M_n}{n}, \qquad s^2_{\text{sample}} = \frac{M_n}{n-1}$$

The sample formula ($n-1$ denominator, Bessel's correction) produces an unbiased estimator of the population variance when $a$ is drawn from a larger distribution.

### Median — quickselect

The median of a sequence sorted in non-decreasing order is:

$$\text{median} = \begin{cases} a_{\lfloor n/2 \rfloor} & n \text{ odd} \\ \dfrac{a_{n/2-1} + a_{n/2}}{2} & n \text{ even} \end{cases}$$

Quickselect finds the $k$-th order statistic in $O(n)$ average time without fully sorting. The implementation copies $a$ into a stack buffer before partitioning, so the original data is not modified. Worst-case is $O(n^2)$ for adversarial input; average over random permutations is $O(n)$.

### Percentile — nearest-rank method

The $p$-th percentile ($0 \leq p \leq 100$) maps to the element at rank:

$$k = \left\lfloor \frac{p \cdot n}{100} \right\rfloor, \quad k = \min(k,\, n-1)$$

Quickselect retrieves $a_{(k)}$ without sorting. $p = 0$ returns the minimum, $p = 100$ returns the maximum.

---

## Complexity

| Function | Time (average) | Stack (float32, `NUMX_MAX_VEC_SIZE = 256`) |
|---|---|---|
| `numx_stats_mean` | $O(n)$ | negligible |
| `numx_stats_variance` | $O(n)$ | negligible |
| `numx_stats_median` | $O(n)$ avg | ~1 KB (copy buffer) |
| `numx_stats_percentile` | $O(n)$ avg | ~1 KB (copy buffer) |

---

## Error and precision

Welford's algorithm is numerically stable: the error in $M_n$ grows as $O(n \varepsilon_{\text{mach}})$ rather than the $O(n^2 \varepsilon_{\text{mach}})$ of the naïve formula. For float32 with $n = 256$, variance error is at most $\approx 3 \times 10^{-5}$ relative.

Quickselect convergence is not affected by precision — it operates on comparisons only.

---

## When to use

- **Mean / variance**: IMU bias estimation, noise characterisation, sensor health monitoring.
- **Median**: Robust central tendency when outliers are expected (e.g. raw ADC samples with transient spikes).
- **Percentile**: Battery state-of-charge distribution, latency monitoring, threshold calibration.

## When NOT to use

- `numx_stats_variance` with `NUMX_VAR_SAMPLE` and $n = 1$ — returns `NUMX_ERR_INVALID_ARG`.
- Arrays longer than `NUMX_MAX_VEC_SIZE` — increase the limit in `numx_config.h` (affects stack usage).

---

## References

1. Welford, B. P. — "Note on a method for calculating corrected sums of squares and products", *Technometrics* 4(3):419–420, 1962.
2. Hoare, C. A. R. — "Algorithm 65: Find", *Communications of the ACM* 4(7):321–322, 1961. (Quickselect basis)

---

## Example

```c
#include "numx/stats.h"

numx_real_t sensor[] = { 3.1f, 3.4f, 2.9f, 10.2f, 3.0f };  /* outlier at index 3 */
numx_real_t m, v, med;

numx_stats_mean(sensor, 5, &m);         /* m  ≈ 4.52 (distorted by outlier) */
numx_stats_variance(sensor, 5, NUMX_VAR_POPULATION, &v);
numx_stats_median(sensor, 5, &med);     /* med ≈ 3.1 (robust to outlier)    */
```
