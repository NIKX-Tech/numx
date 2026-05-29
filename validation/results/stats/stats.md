# Validation: numx_stats (mean / variance / median / percentile)

**Validator:** Amir Ab Khoshk
**Date:** 2026-05-25
**numx version:** d81b386

## Hardware: ESP32
*⚠️ Pending.*

## Hardware: Host
- OS: Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 -O2 / float32

## Test results

| Test case                       | Input          | Expected    | numx output  | Pass/Fail |
|---------------------------------|---------------|-------------|--------------|-----------|
| mean known values               | [2,4,4,4,5,5,7,9] | 5.0     | 5.00000000   | ✅        |
| variance population             | same          | 4.0         | 4.00000000   | ✅        |
| variance sample                 | same          | 4.571429    | 4.57142878   | ✅        |
| median even-n                   | same          | 4.5         | 4.50000000   | ✅        |
| percentile p0 = min             | same          | 2.0         | 2.00000000   | ✅        |
| percentile p100 = max           | same          | 9.0         | 9.00000000   | ✅        |
| median does not modify input    | —             | input unchanged | ✅       | ✅        |
| sample n<2 rejected             | n=1, sample   | ERR_INVALID_ARG | -2       | ✅        |

*All 28 Unity tests: PASS (test_stats.c:267–295)*

## Performance (x86-64)

| Function         | n   | N        | Total      | Per call   |
|------------------|-----|----------|------------|------------|
| stats_mean       | 128 | 100,000  | 3,726 µs   | 37 ns      |
| stats_variance pop| 128 | 100,000 | 39,527 µs  | 395 ns     |
| stats_variance samp| 128| 100,000 | 40,174 µs  | 401 ns     |
| stats_median     | 128 | 100,000  | 662,515 µs | **6,625 ns** ⚠️ |
| stats_percentile p50| 128| 100,000| 329,761 µs | **3,297 ns** ⚠️ |
| stats_percentile p95| 128| 100,000| 44,836 µs  | 448 ns     |
| ESP32            | —   | —        | *pending*  | *pending*  |

## Python comparison (numpy, data=[2,4,4,4,5,5,7,9])

| Function         | numpy ref  | numx       | Error    | OK |
|------------------|-----------|------------|----------|----|
| mean             | 5.0       | 5.0        | 0.00e+00 | ✅ |
| variance pop     | 4.0       | 4.0        | 0.00e+00 | ✅ |
| variance sample  | 4.5714286 | 4.5714288  | 1.76e-07 | ✅ |
| median           | 4.5       | 4.5        | 0.00e+00 | ✅ |
| percentile p0    | 2.0       | 2.0        | 0.00e+00 | ✅ |
| percentile p100  | 9.0       | 9.0        | 0.00e+00 | ✅ |

## Notes
⚠️ **Performance flag — stats_median at n=128:** 6,625 ns/call on x86. This is because the function copies all n elements to a stack buffer and runs quickselect on every call. On ESP32 (240 MHz, ~4× slower clock + no cache) this could reach **25–30 µs per call** — potentially too slow for real-time embedded use. Consider caching a sorted copy if the dataset is static.

⚠️ **stats_variance (Welford):** 395 ns/call on x86 is ~10× slower than stats_mean for the same n. Welford's algorithm involves a division per element which is expensive on float32. Acceptable but worth documenting for embedded contexts.
