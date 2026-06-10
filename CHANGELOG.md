# Changelog

All notable changes to numx are documented here.  
Format: [Keep a Changelog](https://keepachangelog.com/en/1.0.0/) — versioning: [SemVer](https://semver.org/spec/v2.0.0.html).

---

## [0.1.0] — 2026-06-10

Initial public release.

### Added

**Scaffolding**
- Pure C99 project structure: `include/numx/`, `src/`, `tests/`, `examples/`, `docs/`, `benchmarks/`, `validation/`
- `numx_types.h` — core typedefs (`numx_real_t`, `numx_status_t`, `numx_func1d_t`, Q15/Q31/Q63 fixed-point types, version macros)
- `numx_config.h` — all compile-time limits configurable via `-DNUMX_<NAME>=<value>`
- CMakeLists.txt with static library target, Unity test runner, benchmark runner, examples, and full CMake install/export (`find_package(numx)` + `FetchContent` support)
- Unity test framework (v2.6.0) vendored under `tests/unity/`
- CI: GitHub Actions matrix (ubuntu-latest, macos-latest, windows-latest × Debug/Release)
- CodeQL analysis workflow (C/C++, security-extended query set)
- Dependabot for GitHub Actions version tracking

**Phase 1 — Foundation modules** (300 unit tests, all passing on 6 hardware targets)
- `linalg` — vec_dot, vec_norm (L1/L2/Linf), vec_cross3, mat_mul, mat_transpose, mat_transpose_sq, mat_det, lu_decompose, lu_solve
- `stats` — mean, variance, std_dev, median, percentile, min, max
- `roots` — bisect, newton, brent
- `integrate` — trap, simpson, gauss (Gauss-Legendre 8-point)
- `differentiate` — forward, central, richardson (extrapolation)
- `interpolate` — linear, cubic spline, chebyshev
- `poly` — eval (Horner), derivative, roots (Newton + deflation)
- `ode` — rk4, rk45 (adaptive Runge-Kutta-Fehlberg)

**Phase 2 — Signal processing**
- `signal` — window functions (Hamming, Hann, Blackman, rectangular), FIR filter, IIR biquad, convolve, correlate, find_peaks, EMA
- `fft` — Cooley-Tukey radix-2 (float32 forward + inverse, Q15 fixed-point, magnitude spectrum)

**Phase 3 — Unicorn algorithms**
- `autodiff` — forward-mode (dual numbers: add, sub, mul, div, neg, sin, cos, exp, log, sqrt) + reverse-mode (static Wengert tape with backward pass)
- `compressed_sensing` — OMP (Orthogonal Matching Pursuit), ISTA (iterative shrinkage-thresholding), spectral norm via power iteration
- `sketch` — randomized SVD (Halko-Martinsson-Tropp algorithm)

**Hardware validation** (results in `validation/results/`)
- x86-64 Intel i7-13700H / Ubuntu 22.04 / gcc 11.4 — 300/300 tests ✅
- ARM64 Apple M4 Pro / macOS / Apple clang 21.0 — 300/300 tests ✅
- ARM64 Apple M1 Pro / macOS / Apple clang 17.0 — 300/300 tests ✅
- Windows x64 / MSVC 14.51 / float32 — 295/295 tests ✅
- Windows x64 / MSVC 14.51 / float64 — 294/294 tests ✅
- ESP32-S3 Xtensa LX7 / ESP-IDF v5.5.2 / float32 — 548/550 tests ✅ (2 sketch fixture portability failures, noted as FLAG S-01)

**Examples** (`examples/`)
- `01_linalg.c` — solve a 3×3 linear system via LU decomposition
- `02_fft.c` — detect the dominant frequency in a synthetic signal
- `03_ode.c` — simulate a simple pendulum with adaptive RK45
- `04_autodiff.c` — compute gradients with forward-mode and reverse-mode autodiff
- `05_compressed_sensing.c` — exact 2-sparse signal recovery via OMP

**Distribution**
- `library.properties` — Arduino Library Manager manifest
- `library.json` — PlatformIO Registry manifest
- `idf_component.yml` — Espressif Component Registry manifest
- `Doxyfile` — Doxygen configuration for local API reference generation

---

## [Unreleased]

- NTT (Number Theoretic Transform) — constant-time over Z_3329 for CRYSTALS-Kyber/Dilithium (planned for v0.2.0)

---

[0.1.0]: https://github.com/NIKX-Tech/numx/releases/tag/v0.1.0
