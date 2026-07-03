# Hardware Profile: Raspberry Pi 4 Model B

**Profile date:** 2026-06-13
**numx commit:** 14147a3
**Validator:** Amir Ab Khoshk

## System

| Field        | Value                                                  |
|--------------|--------------------------------------------------------|
| Board        | Raspberry Pi 4 Model B Rev 1.2                         |
| SoC          | Broadcom BCM2711                                       |
| CPU          | ARM Cortex-A72, 4 cores                                |
| Architecture | aarch64 (ARMv8-A)                                      |
| CPU max MHz  | 1500 MHz                                               |
| FPU          | ✅ Hardware VFPv4 + NEON (ARMv8-A mandatory)           |
| RAM          | 4 GB LPDDR4 (board-dependent)                          |
| OS           | Raspbian GNU/Linux 13 (trixie)                         |
| Kernel       | 6.12.75+rpt-rpi-v8                                     |
| Compiler     | gcc (Raspbian 14.2.0-19+rpi1) 14.2.0                  |
| CMake        | 3.31.6                                                 |
| C standard   | C99 (`-std=c99`)                                       |
| Optimisation | `-O2` (CMake Release)                                  |
| Timer        | `clock_gettime(CLOCK_MONOTONIC)` — ns resolution       |
| Precision    | float32 (NUMX_USE_DOUBLE not defined)                  |

## Notes

- Cortex-A72 is an ARMv8-A out-of-order core with a 3-wide issue window.
  FPU is fully pipelined for single-precision; hardware NEON SIMD available
  but not used by numx (scalar C99 only).
- At 1500 MHz the Pi 4 is roughly 2–5× slower per operation than Apple
  Silicon (M1/M4) at similar clock rates, but significantly faster than
  ESP32-S3 (240 MHz Xtensa LX7) for the same workloads.
- `clock_gettime(CLOCK_MONOTONIC)` provides nanosecond-resolution timing;
  sub-nanosecond operations are measured over N iterations and divided.
- Build used CMake Release (`-O2`); no link-time optimisation or NEON
  auto-vectorisation flags were added beyond the default compiler settings.

## Unity test summary

| Phase            | Tests | Failures | Date       |
|------------------|-------|----------|------------|
| Phase 1 (linalg, stats, roots, integrate, differentiate, interpolate, poly, ode) | 300 | 0 | 2026-06-13 |
| Phase 2 (signal, fft, autodiff, compressed_sensing, sketch) | included in 300 | 0 | 2026-06-13 |
| Phase 3 (ntt) | 29 | 0 | 2026-07-03 |

**300 / 300 Unity tests PASS** on Raspberry Pi 4 / aarch64 / gcc 14.2.0 / float32 (2026-06-13).
**329 / 329 Unity tests PASS** (29 new NTT tests) as of 2026-07-03, commit `daf5b9c`. Validator:
Amir Ab Khoshk. Full per-test breakdown in
[`validation/results/ntt/ntt.md`](../results/ntt/ntt.md).

## Performance summary

| Function / benchmark              | N       | Total (µs) | Per call (ns) |
|-----------------------------------|---------|------------|---------------|
| vec_dot n=4                       | 100,000 | 1,534      | 15            |
| vec_norm L2 n=2                   | 100,000 | 7,530      | 75            |
| vec_cross3                        | 100,000 | 934        | 9             |
| mat_mul 2×2                       | 100,000 | 6,259      | 62            |
| mat_det 4×4                       | 100,000 | 2,500      | 25            |
| lu_decompose 4×4                  | 100,000 | 30,093     | 300           |
| stats_mean n=8                    | 100,000 | 2,000      | 20            |
| stats_variance_pop n=8            | 100,000 | 10,201     | 102           |
| stats_median n=8                  | 100,000 | 20,405     | 204           |
| stats_percentile p50 n=8          | 100,000 | 12,868     | 128           |
| root_bisect x²-2                  | 10,000  | 5,022      | 502           |
| root_newton x²-2                  | 10,000  | 730        | 73            |
| root_brent x²-2                   | 10,000  | 9,499      | 949           |
| integrate_trap n=100              | 50,000  | 30,177     | 603           |
| integrate_simpson n=100           | 50,000  | 34,308     | 686           |
| integrate_gauss npts=2            | 50,000  | 1,141      | 22            |
| diff_forward                      | 100,000 | 1,245      | 12            |
| diff_central                      | 100,000 | 1,200      | 12            |
| diff_richardson                   | 100,000 | 2,180      | 21            |
| interp_linear n=5                 | 50,000  | 1,267      | 25            |
| interp_spline_cubic n=5 (oneshot) | 50,000  | 6,141      | 122           |
| interp_spline_eval n=5 (prebuilt) | 50,000  | 1,634      | 32            |
| interp_chebyshev n=8              | 50,000  | 46,184     | 923           |
| poly_eval degree=3                | 100,000 | 1,223      | 12            |
| poly_roots degree=3               | 1,000   | 443        | 443           |
| rk4 decay n=1 100 steps           | 10,000  | 80,072     | 8,007         |
| rk45 decay tol=1e-4               | 10,000  | 13,379     | 1,337         |
| dual fwd mul-chain depth=10       | 100,000 | 10,237     | 102           |
| tape rev grad x²+y²               | 10,000  | 57,504     | 5,750         |
| tape rev grad x²+y²+z²            | 10,000  | 58,448     | 5,844         |
| fft_f32 N=64                      | 10,000  | 179,342    | 17,934        |
| fft_f32 N=256                     | 5,000   | 481,800    | 96,360        |
| fft_f32 N=512                     | 1,000   | 217,255    | 217,255       |
| ifft_f32 N=256                    | 5,000   | 486,722    | 97,344        |
| fft_q15 N=256                     | 5,000   | 456,644    | 91,328        |
| fft_magnitude N=256               | 100,000 | 9,889,134  | 98,891        |
| signal_window_hann n=512          | 100,000 | 3,110,695  | 31,106        |
| signal_convolve xn=256 hn=32      | 10,000  | 146,464    | 14,646        |
| signal_fir xn=256 ntaps=32        | 10,000  | 131,097    | 13,109        |
| signal_iir_biquad n=256           | 50,000  | 117,058    | 2,341         |
| signal_ema n=256 alpha=0.1        | 50,000  | 67,187     | 1,343         |
| sketch_rsvd 16×16 rank=4 os=4     | 100     | 93,225     | 932,255       |
| sketch_rsvd 32×32 rank=8 os=4     | 100     | 238,418    | 2,384,185     |
| sketch_rsvd 64×64 rank=8 os=4     | 100     | 299,294    | 2,992,946     |
| cs_spectral_norm 16×32 iter=32    | 100     | 9,676      | 96,769        |
| cs_omp 16×32 k=4                  | 100     | 644        | 6,443         |
| cs_ista 16×32 lam=0.1 iter=500    | 100     | 117,119    | 1,171,190     |
| numx_ntt_forward n=256 (2026-07-03) | 10,000  | 265,890    | 26,589        |
| numx_ntt_inverse n=256 (2026-07-03) | 10,000  | 219,660    | 21,966        |
| numx_ntt_polymul n=256 (2026-07-03) | 10,000  | 813,320    | 81,332        |
| numx_ntt_poly_add n=256 (2026-07-03)| 10,000  | 24,100     | 2,410         |
| numx_ntt_poly_sub n=256 (2026-07-03)| 10,000  | 27,370     | 2,737         |
