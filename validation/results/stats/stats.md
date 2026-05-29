# Validation: numx_stats (mean / variance / median / percentile)

**Validator:** Amir Ab Khoshk
**Date:** 2026-05-25
**numx version:** d81b386

## Hardware: ESP32
- Board: ESP32-S3-DevKitC-1
- CPU: Xtensa LX7 @ 160 MHz, HW FPU
- IDF: ESP-IDF v5.5.2
- numx commit: d81b386
- Compiler: xtensa-esp32s3-elf-gcc -O2 -std=gnu99
- Precision: float32
- Validator: Amir Ab Khoshk

## Hardware: Host
- OS: Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 -O2 / float32

## Test results

### ESP32-S3 hardware run — 35/35 PASS

| Test case                       | Input              | Expected            | numx output  | Pass/Fail |
|---------------------------------|--------------------|---------------------|--------------|-----------|
| mean rc                         | [1,2,3,4,5]        | NUMX_OK (0)         | rc=0         | ✅        |
| mean [1..5]=3                   | [1,2,3,4,5]        | 3.0                 | 3.0000000    | ✅        |
| mean negatives=0                | [-3,-1,1,3]        | 0.0                 | 0.0000000    | ✅        |
| mean constant=5                 | [5,5,5,5]          | 5.0                 | 5.0000000    | ✅        |
| mean n=1=42                     | [42]               | 42.0                | 42.0000000   | ✅        |
| mean null-a                     | a=NULL             | ERR_NULL_PTR (-1)   | rc=-1        | ✅        |
| mean null-out                   | out=NULL           | ERR_NULL_PTR (-1)   | rc=-1        | ✅        |
| mean n=0                        | n=0                | ERR_INVALID_ARG (-2)| rc=-2        | ✅        |
| var pop rc                      | [2,4,4,4,5,5,7,9]  | NUMX_OK (0)         | rc=0         | ✅        |
| var pop=4                       | same               | 4.0                 | 4.0000000    | ✅        |
| var samp rc                     | same               | NUMX_OK (0)         | rc=0         | ✅        |
| var sample=32/7                 | same               | 4.5714286           | 4.5714288    | ✅        |
| var constant=0                  | [7,7,7,7]          | 0.0                 | 0.0000000    | ✅        |
| var sample > population         | [1,2,3,4,5]        | rs > rp             | true         | ✅        |
| var sample n=1 rejected         | n=1, sample        | ERR_INVALID_ARG (-2)| rc=-2        | ✅        |
| var null-a                      | a=NULL             | ERR_NULL_PTR (-1)   | rc=-1        | ✅        |
| var null-out                    | out=NULL           | ERR_NULL_PTR (-1)   | rc=-1        | ✅        |
| median odd rc                   | [5,3,1,4,2]        | NUMX_OK (0)         | rc=0         | ✅        |
| median odd=3                    | [5,3,1,4,2]        | 3.0                 | 3.0000000    | ✅        |
| median even=2.5                 | [4,1,3,2]          | 2.5                 | 2.5000000    | ✅        |
| median no-modify [0]            | [5,3,1]            | inp[0]=5 unchanged  | 5.0000000    | ✅        |
| median no-modify [1]            | [5,3,1]            | inp[1]=3 unchanged  | 3.0000000    | ✅        |
| median no-modify [2]            | [5,3,1]            | inp[2]=1 unchanged  | 1.0000000    | ✅        |
| median n=1=7                    | [7]                | 7.0                 | 7.0000000    | ✅        |
| median n=2=2                    | [3,1]              | 2.0                 | 2.0000000    | ✅        |
| median null-a                   | a=NULL             | ERR_NULL_PTR (-1)   | rc=-1        | ✅        |
| median null-out                 | out=NULL           | ERR_NULL_PTR (-1)   | rc=-1        | ✅        |
| pct p0 rc                       | p=0                | NUMX_OK (0)         | rc=0         | ✅        |
| pct p0=min=1                    | [5,3,1,4,2], p=0   | 1.0                 | 1.0000000    | ✅        |
| pct p100=max=5                  | same, p=100        | 5.0                 | 5.0000000    | ✅        |
| pct no-modify [0]               | [3,1,2]            | inp[0]=3 unchanged  | 3.0000000    | ✅        |
| pct null-a                      | a=NULL             | ERR_NULL_PTR (-1)   | rc=-1        | ✅        |
| pct null-out                    | out=NULL           | ERR_NULL_PTR (-1)   | rc=-1        | ✅        |
| pct p<0                         | p=-1               | ERR_INVALID_ARG (-2)| rc=-2        | ✅        |
| pct p>100                       | p=101              | ERR_INVALID_ARG (-2)| rc=-2        | ✅        |

**ESP32-S3 result: 35 PASS / 0 FAIL / 35 TOTAL**

*All 28 Unity tests (host): PASS (test_runner.c:267–295)*

## Performance (x86-64)

| Function         | n   | N        | Total      | Per call   |
|------------------|-----|----------|------------|------------|
| stats_mean       | 128 | 100,000  | 3,726 µs   | 37 ns      |
| stats_variance pop| 128 | 100,000 | 39,527 µs  | 395 ns     |
| stats_variance samp| 128| 100,000 | 40,174 µs  | 401 ns     |
| stats_median     | 128 | 100,000  | 662,515 µs | **6,625 ns** ⚠️ |
| stats_percentile p50| 128| 100,000| 329,761 µs | **3,297 ns** ⚠️ |
| stats_percentile p95| 128| 100,000| 44,836 µs  | 448 ns     |
| ESP32-S3         | —   | —        | *run `run_benchmarks()` to collect* | — |

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
