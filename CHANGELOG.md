# Changelog

All notable changes to numx are documented here.  
Format: [Keep a Changelog](https://keepachangelog.com/en/1.0.0/) — versioning: [SemVer](https://semver.org/spec/v2.0.0.html).

---

## [Unreleased]

### Added
- `numx_cholesky_decompose` — Cholesky-Banachiewicz factorization ($A = LL^T$) for
  symmetric positive-definite matrices in the `linalg` module. Contributed by
  Erfan Esmaeili ([#51](https://github.com/NIKX-Tech/numx/pull/51)). Validated on
  ARM64/Apple M4 Pro (float32 and float64, ASan/UBSan clean); full platform sweep
  pending.

### Security
- `ntt`: rewrote `priv_barrett`'s canonicalization step (`src/ntt.c`) to be
  branchless, resolving the module's one known constant-time gap flagged in
  `docs/algorithms/ntt.md`. The previous version's `if (r < 0) ... if (r >= q) ...`
  used data-dependent branches; the new version uses only unsigned arithmetic and
  a well-defined C99 boolean comparison. Fix contributed by u/robchroma
  (r/C_Programming); exhaustively verified against `a % q` for all 22,164,483
  valid inputs before adoption. No known data-dependent branches remain in the
  transform.

---

## [1.0.0] — 2026-07-03

First stable release. Completes the full 4-phase plan (foundation, signal
processing, unicorn algorithms, hardening) with the addition of the NTT
(post-quantum) module. 14 modules, 329 tests, validated across 10
platform/toolchain combinations.

### Added

**NTT (post-quantum)**
- `ntt` — negacyclic NTT over Z_3329[x]/(x^256+1) with Kyber/Dilithium parameters; forward NTT, inverse NTT (with normalization), pointwise multiplication (basemul), full polynomial multiplication, and Barrett coefficient reduction; data-independent butterfly network and table lookups (see docs/algorithms/ntt.md for a side-channel note on the reduction step); seven Cooley-Tukey / Gentleman-Sande butterfly stages; precomputed twiddle tables; zero heap allocation; 29 unit tests

**Hardware validation** (results in `validation/results/`)
- ARM64 Raspberry Pi 4B / Raspbian GNU/Linux 13 / gcc 14.2.0 / float32 — 300/300 tests ✅, Phase 1 & 2 benchmarks captured (see [`validation/hardware/raspberry_pi.md`](validation/hardware/raspberry_pi.md))
- **NTT validation (2026-07-03)** — 29 new NTT tests validated across 7 platform/toolchain combinations, 3 of them not previously covered by any validation run (see [`validation/results/ntt/ntt.md`](validation/results/ntt/ntt.md)):
  - ARM64 Apple M4 Pro / macOS / Apple clang 21.0.0 / float32 — 329/329 tests ✅
  - ARM64 Raspberry Pi 4B (addendum to the entry above) — 329/329 tests ✅
  - Windows x64 / MSVC 14.51 / float64 — 329/329 tests ✅ (supersedes the 294/294 figure in the `[0.1.0]` entry below, which was accurate for that release but predates later test growth and the NTT module)
  - Windows x86 (32-bit, i386) / MSVC 19.51 / float32 — 329/329 tests ✅ (new architecture)
  - Linux x86-64 / WSL2 Ubuntu 24.04 / gcc 13.3.0 / float64 — 329/329 tests ✅ (new environment)
  - Linux x86 (32-bit) / WSL2 Ubuntu 24.04 / gcc 13.3.0 / float32 — 329/329 tests ✅ (new environment)
  - ESP32-S3 — 29/29 NTT tests ✅ via a standalone example project ([`examples/esp32_ntt_test/`](examples/esp32_ntt_test/)), separate from the main test harness

**Benchmarks**
- `benchmarks/bench_win.c` — Windows benchmark suite using QueryPerformanceCounter; covers all 13 modules (Phase 1 & 2)
- `benchmarks/esp32/` — ESP32 benchmark suite (Phase 1 & 2): `bench_phase1.c`, `bench_phase2.c`, `bench_common.h`

**Test infrastructure**
- `tests/x86/` — standalone float32 Unity test build with own `CMakeLists.txt`; decoupled from top-level tree
- `tests/x64/` — standalone float64 Unity test build with own `CMakeLists.txt`; decoupled from top-level tree
- `tests/esp32_tests/` — custom test framework for ESP32 (IDF component); per-module test files

**Examples** (`examples/`)
- `06_ntt.c` — multiply two polynomials in the CRYSTALS-Kyber ring and demonstrate the ring's negacyclic wraparound

### Fixed
- `benchmarks/bench_ntt.c`: the "total" column of the benchmark printer divided by
  the iteration count twice, printing per-call-in-µs mislabeled as the total elapsed
  time. Corrected; "per call" was unaffected.
- `CHANGELOG.md`: NTT unit test count corrected from 20 to 29.
- `docs/algorithms/ntt.md`: Phase ID corrected from `P3.12` to `P3.04`, consistent
  with the `P3.01`–`P3.03` sequence (autodiff, compressed_sensing, sketch).

### Known gaps
- NTT is not wired into `tests/x86/`, `tests/x64/`, or `tests/esp32_tests/` — those
  standalone/native harnesses still only cover the 13 pre-NTT modules. The Windows
  x86 (32-bit) and Linux (WSL2) NTT validation runs above built from the root
  `CMakeLists.txt` directly rather than these standalone trees.

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

[1.0.0]: https://github.com/NIKX-Tech/numx/releases/tag/v1.0.0
[0.1.0]: https://github.com/NIKX-Tech/numx/releases/tag/v0.1.0
