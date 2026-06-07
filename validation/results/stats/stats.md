# Validation: numx_stats (mean / variance / median / percentile)

---

## x86-64 — Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-25 | **Commit:** d81b386

### Test cases — dataset [2, 4, 4, 4, 5, 5, 7, 9]

| Function | Expected | Computed | Error | Pass |
|----------|----------|----------|-------|------|
| mean | 5.0 | 5.00000000 | 0.00e+00 | ✅ |
| variance (population) | 4.0 | 4.00000000 | 0.00e+00 | ✅ |
| variance (sample) | 4.571429 | 4.57142878 | 1.76e-07 | ✅ |
| median | 4.5 | 4.50000000 | 0.00e+00 | ✅ |
| percentile p0 | 2.0 | 2.00000000 | 0.00e+00 | ✅ |
| percentile p100 | 9.0 | 9.00000000 | 0.00e+00 | ✅ |
| median does not modify input | — | input unchanged | — | ✅ |
| sample n<2 → ERR_INVALID_ARG | -2 | -2 | — | ✅ |

*All 28 Unity tests: PASS (test_stats.c:267–295)*

### Performance

| Function | n | N | Total | Per call |
|----------|---|---|-------|----------|
| stats_mean | 128 | 100,000 | 3,726 µs | 37 ns |
| stats_variance (pop) | 128 | 100,000 | 39,527 µs | 395 ns |
| stats_variance (samp) | 128 | 100,000 | 40,174 µs | 401 ns |
| stats_median | 128 | 100,000 | 662,515 µs | **6,625 ns** ⚠️ |
| stats_percentile p50 | 128 | 100,000 | 329,761 µs | **3,297 ns** ⚠️ |

### Precision vs numpy reference

| Function | numpy | numx | Error |
|----------|-------|------|-------|
| mean | 5.0 | 5.0 | 0.00e+00 |
| variance pop | 4.0 | 4.0 | 0.00e+00 |
| variance samp | 4.5714286 | 4.5714288 | 1.76e-07 |
| median | 4.5 | 4.5 | 0.00e+00 |
| percentile p0 | 2.0 | 2.0 | 0.00e+00 |
| percentile p100 | 9.0 | 9.0 | 0.00e+00 |

⚠️ **stats_median n=128: 6,625 ns/call** — copy-to-stack + quickselect every call. On ESP32 ~25–30 µs; may be too slow for RT loops.

⚠️ **stats_variance (Welford): ~400 ns/call** — one division per element; expensive on float32.

---

## ARM64 — macOS 26.2 / Apple M4 Pro / Apple clang 21.0.0 / float32
**Validator:** Erfan Jazeb Nikoo | **Date:** 2026-05-29 | **Commit:** 37e581f

### Test cases — dataset [2, 4, 4, 4, 5, 5, 7, 9]

| Function | Expected | Computed | Error | Pass |
|----------|----------|----------|-------|------|
| mean | 5.0 | 5.00000000 | 0.00e+00 | ✅ |
| variance (population) | 4.0 | 4.00000000 | 0.00e+00 | ✅ |
| variance (sample) | 4.571429 | 4.57142878 | 0.00e+00 | ✅ |
| median | 4.5 | 4.50000000 | 0.00e+00 | ✅ |
| percentile p0 | 2.0 | 2.00000000 | 0.00e+00 | ✅ |
| percentile p25 | 4.0 | 4.00000000 | 0.00e+00 | ✅ |
| percentile p50 | 5.0 | 5.00000000 | 0.00e+00 | ✅ |
| percentile p75 | 7.0 | 7.00000000 | 0.00e+00 | ✅ |
| percentile p100 | 9.0 | 9.00000000 | 0.00e+00 | ✅ |

**Note on percentile formula:** numx uses `k = (int)(p/100 * n)` (floor/Type-1/inverted-CDF). The internal comment says "nearest-rank ceil-1" but the code implements floor — they agree for non-exact multiples but diverge at exact multiples (e.g. p=50, n=8 → floor gives k=4, nearest-rank gives k=3). Behaviour is consistent and matches numpy `method='inverted_cdf'`.

*300 / 300 Unity tests PASS*

### Performance (n=8 validation dataset)

| Function | N | Total | Per call |
|----------|---|-------|----------|
| stats_mean n=8 | 100,000 | 298 µs | 2 ns |
| stats_variance (pop) n=8 | 100,000 | 1,170 µs | 11 ns |
| stats_median n=8 | 100,000 | 6,791 µs | 67 ns |
| stats_percentile p50 n=8 | 100,000 | 3,232 µs | 32 ns |

### Precision vs numpy reference

| Function | numpy | numx | Error |
|----------|-------|------|-------|
| mean | 5.0 | 5.00000000 | 0.00e+00 |
| variance pop | 4.0 | 4.00000000 | 0.00e+00 |
| variance samp | 4.5714286 | 4.57142878 | 0.00e+00 |
| median | 4.5 | 4.50000000 | 0.00e+00 |

---

## ESP32-S3 — ESP-IDF v5.5.2 / Xtensa LX7 / xtensa-esp32s3-elf-gcc / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-29 | **Commit:** d81b386

### mean

| Test case | Expected | Computed | Error | Pass |
|-----------|----------|----------|-------|------|
| [1..5] = 3 | 3.0 | 3.0000000 | 0.00e+00 | ✅ |
| negatives = 0 | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| constant = 5 | 5.0 | 5.0000000 | 0.00e+00 | ✅ |
| arith-seq [1..7] = 4 | 4.0 | 4.0000000 | 0.00e+00 | ✅ |
| n=1 = 42 | 42.0 | 42.0000000 | 0.00e+00 | ✅ |
| null array ptr | rc=-1 | rc=-1 | — | ✅ |
| null output ptr | rc=-1 | rc=-1 | — | ✅ |
| n=0 | rc=-2 | rc=-2 | — | ✅ |

### variance

| Test case | Expected | Computed | Error | Pass |
|-----------|----------|----------|-------|------|
| population = 4 | 4.0 | 4.0000000 | 0.00e+00 | ✅ |
| sample = 32/7 | 4.571429 | 4.5714288 | 0.00e+00 | ✅ |
| constant = 0 | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| sample > population | samp > pop | confirmed | — | ✅ |
| sample n=1 rejected | rc=-2 | rc=-2 | — | ✅ |
| null array ptr | rc=-1 | rc=-1 | — | ✅ |
| null output ptr | rc=-1 | rc=-1 | — | ✅ |

### median

| Test case | Expected | Computed | Error | Pass |
|-----------|----------|----------|-------|------|
| odd count = 3 | 3.0 | 3.0000000 | 0.00e+00 | ✅ |
| even count = 2.5 | 2.5 | 2.5000000 | 0.00e+00 | ✅ |
| no-modify input[0] | 5.0 | 5.0000000 | 0.00e+00 | ✅ |
| no-modify input[1] | 3.0 | 3.0000000 | 0.00e+00 | ✅ |
| no-modify input[2] | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| sorted-mid = 30 | 30.0 | 30.0000000 | 0.00e+00 | ✅ |
| n=1 = 7 | 7.0 | 7.0000000 | 0.00e+00 | ✅ |
| n=2 = 2 | 2.0 | 2.0000000 | 0.00e+00 | ✅ |
| null array ptr | rc=-1 | rc=-1 | — | ✅ |
| null output ptr | rc=-1 | rc=-1 | — | ✅ |

### percentile

| Test case | Expected | Computed | Error | Pass |
|-----------|----------|----------|-------|------|
| p0 = min = 1 | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| p100 = max = 5 | 5.0 | 5.0000000 | 0.00e+00 | ✅ |
| no-modify input[0] | 3.0 | 3.0000000 | 0.00e+00 | ✅ |
| null array ptr | rc=-1 | rc=-1 | — | ✅ |
| null output ptr | rc=-1 | rc=-1 | — | ✅ |
| p < 0 | rc=-2 | rc=-2 | — | ✅ |
| p > 100 | rc=-2 | rc=-2 | — | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| stats_mean n=128 | 10,000 | 61,944 µs | 6,194 ns |
| stats_variance pop n=128 | 5,000 | 347,803 µs | 69,560 ns |
| stats_variance sample n=128 | 5,000 | 347,657 µs | 69,531 ns |
| stats_median n=128 | 1,000 | 1,109,766 µs | 1,109,766 ns |
| stats_percentile p50 n=128 | 1,000 | 555,790 µs | 555,790 ns |
| stats_percentile p95 n=128 | 1,000 | 84,711 µs | 84,711 ns |

**RESULTS: 37 PASS / 0 FAIL / 37 TOTAL**

---

## Windows x86 — Windows 11 / MSVC 14.51 (VS 2026 Build Tools) / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-06-05 | **Commit:** 4c4c0f0

### Test cases

| Test | Result |
|------|--------|
| test_stats_mean_known | ✅ |
| test_stats_mean_negative_values | ✅ |
| test_stats_mean_arithmetic_sequence | ✅ |
| test_stats_mean_constant_array | ✅ |
| test_stats_mean_single_element | ✅ |
| test_stats_mean_null_a | ✅ |
| test_stats_mean_null_result | ✅ |
| test_stats_mean_n_zero | ✅ |
| test_stats_variance_pop_known | ✅ |
| test_stats_variance_sample_known | ✅ |
| test_stats_variance_constant_is_zero | ✅ |
| test_stats_variance_sample_ge_population | ✅ |
| test_stats_variance_sample_n1_rejected | ✅ |
| test_stats_variance_null | ✅ |
| test_stats_median_odd_known | ✅ |
| test_stats_median_even_known | ✅ |
| test_stats_median_sorted_middle | ✅ |
| test_stats_median_does_not_modify_input | ✅ |
| test_stats_median_single_element | ✅ |
| test_stats_median_two_elements | ✅ |
| test_stats_median_null | ✅ |
| test_stats_percentile_0_is_min | ✅ |
| test_stats_percentile_100_is_max | ✅ |
| test_stats_percentile_does_not_modify_input | ✅ |
| test_stats_percentile_null | ✅ |
| test_stats_percentile_out_of_range | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| stats_mean n=128 | 100,000 | 19,776 µs | 197 ns |
| stats_variance pop n=128 | 50,000 | 52,740 µs | 1,054 ns |
| stats_variance sample n=128 | 50,000 | 51,473 µs | 1,029 ns |
| stats_median n=128 | 10,000 | 185,883 µs | 18,588 ns |
| stats_percentile p50 n=128 | 10,000 | 93,902 µs | 9,390 ns |
| stats_percentile p95 n=128 | 10,000 | 13,668 µs | 1,366 ns |

**RESULTS: 26 PASS / 0 FAIL / 26 TOTAL**

---

## Windows x64 — Windows 11 / MSVC 14.51 (VS 2026 Build Tools) / float64
**Validator:** Amir Ab Khoshk | **Date:** 2026-06-06 | **Commit:** 1bba399

### Test cases

| Test | Result |
|------|--------|
| test_stats_mean_known | ✅ |
| test_stats_mean_negative_values | ✅ |
| test_stats_mean_arithmetic_sequence | ✅ |
| test_stats_mean_constant_array | ✅ |
| test_stats_mean_single_element | ✅ |
| test_stats_mean_null_a | ✅ |
| test_stats_mean_null_result | ✅ |
| test_stats_mean_n_zero | ✅ |
| test_stats_variance_pop_known | ✅ |
| test_stats_variance_sample_known | ✅ |
| test_stats_variance_constant_is_zero | ✅ |
| test_stats_variance_sample_ge_population | ✅ |
| test_stats_variance_sample_n1_rejected | ✅ |
| test_stats_variance_null | ✅ |
| test_stats_median_odd_known | ✅ |
| test_stats_median_even_known | ✅ |
| test_stats_median_sorted_middle | ✅ |
| test_stats_median_does_not_modify_input | ✅ |
| test_stats_median_single_element | ✅ |
| test_stats_median_two_elements | ✅ |
| test_stats_median_null | ✅ |
| test_stats_percentile_0_is_min | ✅ |
| test_stats_percentile_100_is_max | ✅ |
| test_stats_percentile_does_not_modify_input | ✅ |
| test_stats_percentile_null | ✅ |
| test_stats_percentile_out_of_range | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| stats_mean n=128 | 100,000 | 19,751 µs | 197 ns |
| stats_variance pop n=128 | 50,000 | 38,276 µs | 765 ns |
| stats_variance sample n=128 | 50,000 | 38,265 µs | 765 ns |
| stats_median n=128 | 10,000 | 187,347 µs | 18,734 ns |
| stats_percentile p50 n=128 | 10,000 | 91,969 µs | 9,196 ns |
| stats_percentile p95 n=128 | 10,000 | 13,707 µs | 1,370 ns |

**RESULTS: 26 PASS / 0 FAIL / 26 TOTAL**
