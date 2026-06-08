# Hardware Profile: MacBook Pro (Apple M1 Pro)

**Profile date:** 2026-06-08
**numx commit:** 1380ab1
**Validator:** Erfan Jazeb Nikoo

## System

| Field        | Value                                              |
|--------------|----------------------------------------------------|
| Machine      | MacBook Pro (Model: MacBookPro18,3)                |
| Chip         | Apple M1 Pro                                       |
| Architecture | arm64 (AArch64)                                    |
| CPU cores    | 10 (8 performance + 2 efficiency)                  |
| Memory       | 16 GB                                              |
| OS           | macOS 26.2 (Darwin 25.2.0)                         |
| Compiler     | Apple clang 17.0.0 (clang-1700.6.3.2)              |
| C standard   | C99 (`-std=c99`)                                   |
| Optimisation | `-O2`                                              |
| Precision    | float32 (`NUMX_USE_DOUBLE` not defined)            |
| CMake        | 4.3.2                                              |

## Notes

- Apple M1 Pro is an earlier-generation Apple Silicon (2021) compared to M4 Pro.
  Both use the same arm64 AArch64 ISA with hardware single-precision FPU.
  IEEE 754 float32 results are bit-identical between M1 Pro and M4 Pro — confirms
  deterministic behaviour across ARM64 microarchitecture generations.
- Apple clang 17.0.0 (LLVM-based) vs clang 21.0.0 used on the M4 Pro host.
  Different toolchain version; confirms library compiles cleanly across clang generations.
- ARM64 is the closest available host ISA to embedded ARM Cortex-M targets;
  results here are more predictive of Cortex-M behaviour than x86-64 results.
- System rebooted to clean idle state before benchmarking. No other user sessions
  or background processes active during timing runs.
- cmake installed via pip3 (`/Users/erfan/Library/Python/3.9/bin/cmake`).

## Test results

**300 / 300 tests PASS. 0 failures.**

```
export PATH="$PATH:/Users/erfan/Library/Python/3.9/bin"
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
./build/numx_tests
```

## Benchmark results

Benchmarks run via dedicated validation runners (`val_runner` — Phase 1 and
`val_bench_phase2` — Phase 2) on a clean rebooted system.

### Phase 1 — core functions

| Function | N | Total (µs) | Per call (ns) |
|----------|---|------------|---------------|
| vec_dot n=4 | 100,000 | 299 | 2 |
| vec_norm L2 n=2 | 100,000 | 767 | 7 |
| vec_cross3 | 100,000 | 131 | 1 |
| mat_mul 2×2 | 100,000 | 660 | 6 |
| mat_det 4×4 | 100,000 | 524 | 5 |
| lu_decompose 4×4 | 100,000 | 8,359 | 83 |
| stats_mean n=8 | 100,000 | 265 | 2 |
| stats_variance_pop n=8 | 100,000 | 807 | 8 |
| stats_median n=8 | 100,000 | 3,721 | 37 |
| stats_percentile p50 n=8 | 100,000 | 2,090 | 20 |
| root_bisect x²-2 | 10,000 | 1,083 | 108 |
| root_newton x²-2 | 10,000 | 93 | 9 |
| root_brent x²-2 | 10,000 | 706 | 70 |
| integrate_trap n=100 | 50,000 | 5,595 | 111 |
| integrate_simpson n=100 | 50,000 | 5,113 | 102 |
| integrate_gauss npts=2 | 50,000 | 217 | 4 |
| diff_forward | 100,000 | 360 | 3 |
| diff_central | 100,000 | 370 | 3 |
| diff_richardson | 100,000 | 404 | 4 |
| interp_linear n=5 | 50,000 | 124 | 2 |
| interp_spline_cubic n=5 (one-shot) | 50,000 | 836 | 16 |
| interp_spline_eval n=5 (pre-built) | 50,000 | 172 | 3 |
| interp_chebyshev n=8 | 50,000 | 5,942 | 118 |
| poly_eval degree=3 | 100,000 | 329 | 3 |
| poly_roots degree=3 | 1,000 | 78 | 78 |
| rk4 decay n=1 100 steps | 10,000 | 33,639 | 3,363 |
| rk45 decay tol=1e-4 | 10,000 | 4,703 | 470 |

### Phase 2 — autodiff, FFT, signal, sketch, compressed sensing

| Function | N | Total (µs) | Per call (ns) |
|----------|---|------------|---------------|
| dual fwd: mul-chain depth=10 | 100,000 | 821 | 8 |
| tape: rev grad x²+y² | 10,000 | 6,675 | 667 |
| tape: rev grad x²+y²+z² (3 vars) | 10,000 | 6,905 | 690 |
| fft_f32 N=64 | 10,000 | 25,478 | 2,547 |
| fft_f32 N=256 | 5,000 | 67,538 | 13,507 |
| fft_f32 N=512 | 1,000 | 30,423 | 30,423 |
| ifft_f32 N=256 | 5,000 | 67,353 | 13,470 |
| fft_q15 N=256 | 5,000 | 64,906 | 12,981 |
| fft_magnitude N=256 | 100,000 | 1,954,211 | 19,542 |
| signal_window_hann n=512 | 100,000 | 383,955 | 3,839 |
| signal_convolve xn=256 hn=32 | 10,000 | 23,107 | 2,310 |
| signal_fir xn=256 ntaps=32 | 10,000 | 48,926 | 4,892 |
| signal_iir_biquad n=256 | 50,000 | 38,541 | 770 |
| signal_ema n=256 alpha=0.1 | 50,000 | 27,009 | 540 |
| sketch_rsvd 16×16 rank=4 os=4 | 100 | 37,542 | 375,420 |
| sketch_rsvd 32×32 rank=8 os=4 | 100 | 95,309 | 953,090 |
| sketch_rsvd 64×64 rank=8 os=4 | 100 | 110,159 | 1,101,590 |
| cs_spectral_norm 16×32 iter=32 | 100 | 2,191 | 21,910 |
| cs_omp 16×32 k=4 | 100 | 117 | 1,170 |
| cs_ista 16×32 lam=0.1 iter=500 | 100 | 16,765 | 167,650 |
