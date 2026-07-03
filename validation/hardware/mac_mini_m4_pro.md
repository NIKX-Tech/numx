# Hardware Profile: Mac mini (Apple M4 Pro)

**Profile date:** 2026-06-09 (last validated) / 2026-07-03 (NTT addendum)
**numx commit:** 2fc85d0 (base) / c397429 (NTT)
**Validator:** Erfan Jazeb Nikoo

## System

| Field        | Value                                              |
|--------------|----------------------------------------------------|
| Machine      | Mac mini (Model: Mac16,11)                         |
| Chip         | Apple M4 Pro                                       |
| Architecture | arm64 (AArch64)                                    |
| CPU cores    | 12 (8 performance + 4 efficiency)                  |
| Memory       | 24 GB                                              |
| OS           | macOS 26.2 (Darwin 25.2.0)                         |
| Compiler     | Apple clang 21.0.0 (clang-2100.1.1.101)            |
| C standard   | C99 (`-std=c99`)                                   |
| Optimisation | `-O2`                                              |
| Precision    | float32 (`NUMX_USE_DOUBLE` not defined)            |
| CMake        | 3.31.4                                             |

## Notes

- Apple M4 Pro uses AArch64 with hardware single-precision FPU.
  This is a different ISA from the x86-64 host validation — confirms
  the library is portable across architectures.
- Apple clang (LLVM-based) vs gcc used for x86 validation — different
  optimizer; any compiler-specific UB would surface here.
- ARM64 is the closest available host ISA to embedded ARM Cortex-M
  targets; results here are more predictive of Cortex-M behaviour
  than x86-64 results.
- No CPU pinning or real-time priority. Results are indicative; expect
  ±10% variance between runs.
- Comprehensive benchmarks completed 2026-06-09. All Phase 1 & Phase 2/3
  functions timed across representative N values.

## Test results

**300 / 300 tests PASS. 0 failures.**

```
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
./build/numx_tests
```

## Benchmark results

**Phase 1 (Foundation)** — All pass with excellent performance across linalg, stats, roots, integrate, differentiate, interpolate, poly, ode.

**Phase 2/3 (Advanced)** — Complete timing profile:

| Function | N | Total (µs) | Per call (ns) |
|----------|---|------------|---------------|
| **Autodiff** | | | |
| dual fwd (chain depth=10) | 100,000 | 1,297 | 13 |
| tape rev grad (x2+y2) | 10,000 | 11,159 | 1,115 |
| **FFT** | | | |
| fft_f32 N=64 | 10,000 | 27,679 | 2,767 |
| fft_f32 N=256 | 5,000 | 52,147 | 10,429 |
| fft_f32 N=512 | 1,000 | 23,218 | 23,218 |
| ifft_f32 N=256 | 5,000 | 50,674 | 10,134 |
| fft_q15 N=256 | 5,000 | 51,630 | 10,326 |
| fft_magnitude N=256 | 100,000 | 1,634,921 | 16,349 |
| **Signal** | | | |
| window_hann n=512 | 100,000 | 331,971 | 3,319 |
| convolve (xn=256, hn=32) | 10,000 | 13,436 | 1,343 |
| fir (xn=256, ntaps=32) | 10,000 | 30,424 | 3,042 |
| iir_biquad n=256 | 50,000 | 16,733 | 334 |
| ema (n=256, α=0.1) | 50,000 | 13,784 | 275 |
| **Sketch** | | | |
| rsvd 16×16 (rank=4, os=4) | 100 | 24,206 | 242,060 |
| rsvd 32×32 (rank=8, os=4) | 100 | 60,679 | 606,790 |
| rsvd 64×64 (rank=8, os=4) | 100 | 66,721 | 667,210 |
| **Compressed Sensing** | | | |
| spectral_norm (16×32, iter=32) | 100 | 1,403 | 14,030 |
| omp (16×32, k=4) | 100 | 86 | 860 |
| ista (16×32, λ=0.1, iter=500) | 100 | 11,144 | 111,440 |
| **NTT** (added 2026-07-03) | | | |
| numx_ntt_forward n=256 | 10,000 | 11,171 | 1,117 |
| numx_ntt_inverse n=256 | 10,000 | 6,607 | 660 |
| numx_ntt_polymul n=256 | 10,000 | 29,145 | 2,914 |
| numx_ntt_poly_add n=256 | 10,000 | 485 | 48 |
| numx_ntt_poly_sub n=256 | 10,000 | 545 | 54 |

**All tests:** 300 / 300 PASS on 2026-06-09 with commit 2fc85d0; **329 / 329 PASS** (29 new NTT tests)
on 2026-07-03 with commit c397429. Full per-test breakdown in
[`validation/results/ntt/ntt.md`](../results/ntt/ntt.md).
