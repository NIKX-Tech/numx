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

## ESP32-S3
**Status:** ⚠️ Pending
