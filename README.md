# numx

[![CI](https://img.shields.io/github/actions/workflow/status/NIKX-Tech/numx/ci.yml?branch=main&style=flat-square&label=build)](https://github.com/NIKX-Tech/numx/actions/workflows/ci.yml) [![License](https://img.shields.io/github/license/NIKX-Tech/numx?style=flat-square&color=blue)](https://opensource.org/licenses/MIT) [![C Standard](https://img.shields.io/badge/standard-C99-00599C?style=flat-square&logo=c&logoColor=white)](https://github.com/NIKX-Tech/numx) [![Repo Size](https://img.shields.io/github/repo-size/NIKX-Tech/numx?style=flat-square)](https://github.com/NIKX-Tech/numx) [![GitHub Stars](https://img.shields.io/github/stars/NIKX-Tech/numx?style=flat-square&color=yellow)](https://github.com/NIKX-Tech/numx/stargazers) [![Platform: ESP32](https://img.shields.io/badge/platform-ESP32-E7352C?style=flat-square&logo=espressif&logoColor=white)](https://github.com/NIKX-Tech/numx) [![Platform: ARM Cortex-M/A](https://img.shields.io/badge/platform-ARM%20Cortex--M%2FA-0091BD?style=flat-square&logo=arm&logoColor=white)](https://github.com/NIKX-Tech/numx)

[![CodeQL](https://img.shields.io/github/actions/workflow/status/NIKX-Tech/numx/codeql.yml?branch=main&label=codeql&style=flat-square)](https://github.com/NIKX-Tech/numx/actions/workflows/codeql.yml)
[![Latest Release](https://img.shields.io/github/v/release/NIKX-Tech/numx?style=flat-square)](https://github.com/NIKX-Tech/numx/releases)
[![Dependabot](https://img.shields.io/badge/dependabot-enabled-025e8a?logo=dependabot&style=flat-square)](https://github.com/NIKX-Tech/numx/blob/main/.github/dependabot.yml)
[![Website](https://img.shields.io/badge/website-numx.dev-4F46E5?style=flat-square&logo=google-chrome&logoColor=white)](https://numx.dev)

<!--
Uncomment when ready:
  [![OpenSSF Scorecard](...)](...)       → add .github/workflows/scorecard.yml
  [![Sponsor GitHub](...)](...)          → activate GitHub Sponsors for NIKX-Tech
  [![Sponsor Open Collective](...)](...)  → create Open Collective project page
  [![Discord](...)](...)                 → create Discord server

[![OpenSSF Scorecard](https://img.shields.io/ossf-scorecard/github.com/NIKX-Tech/numx?label=openssf%20scorecard&style=flat-square)](https://securityscorecards.dev/projects/github.com/NIKX-Tech/numx)
[![Sponsor GitHub](https://img.shields.io/badge/sponsor-GitHub-EA4AAA?style=flat-square&logo=github-sponsors)](https://github.com/sponsors/NIKX-Tech)
[![Sponsor Open Collective](https://img.shields.io/badge/sponsor-Open%20Collective-00A0E0?style=flat-square&logo=opencollective)](https://opencollective.com/nikx-technologies/projects/numx)
[![Discord](https://img.shields.io/badge/discord-join%20chat-5865F2?style=flat-square&logo=discord&logoColor=white)](https://discord.gg/NIKX)
-->

A portable, scientific-grade numerical computing library in pure C99 for resource-constrained embedded systems.

---

## Why numx

Most numerical libraries assume a heap, an OS, and a math coprocessor. numx assumes none of those things. It targets the processors where computation is hardest: ESP32, ARM Cortex-M/A, AVR, RISC-V — and it brings algorithms previously considered out of reach (automatic differentiation, compressed sensing, post-quantum NTT) to those platforms.

Every function is reentrant, allocation-free, and returns a typed status code. The entire precision of the library — float32 or float64 — is switchable with a single compile flag.

---

## Module status

| Module | Functions | Status |
|---|---|---|
| [**linalg**](docs/algorithms/linalg.md) | dot, norm, cross, mat_mul, transpose, det, LU | ✅ complete |
| [**stats**](docs/algorithms/stats.md) | mean, variance, median, percentile | ✅ complete |
| [**roots**](docs/algorithms/roots.md) | bisect, newton, brent | ✅ complete |
| [**integrate**](docs/algorithms/integrate.md) | trap, simpson, gauss | ✅ complete |
| [**differentiate**](docs/algorithms/differentiate.md) | forward, central, richardson | ✅ complete |
| [**interpolate**](docs/algorithms/interpolate.md) | linear, cubic spline, chebyshev | ✅ complete |
| [**poly**](docs/algorithms/poly.md) | eval (Horner), roots (Newton + deflation) | ✅ complete |
| [**ode**](docs/algorithms/ode.md) | rk4, rk45 (adaptive) | ✅ complete |
| [**signal**](docs/algorithms/signal.md) | windows, FIR, IIR, convolve, correlate, peaks, EMA | ✅ complete |
| [**fft**](docs/algorithms/fft.md) | Cooley-Tukey f32, Q15, IFFT, magnitude | ✅ complete |
| [**autodiff**](docs/algorithms/autodiff.md) | forward-mode (dual numbers), reverse-mode (static tape) | ✅ complete |
| [**compressed_sensing**](docs/algorithms/compressed_sensing.md) | OMP, ISTA | ✅ complete |
| [**sketch**](docs/algorithms/sketch.md) | randomized SVD (Halko-Martinsson-Tropp) | ✅ complete |
| **ntt** | Number Theoretic Transform (constant-time, Kyber/Dilithium params) | 🔧 planned |

---

## Hardware validation

All results are device-run, per-formula values with measured error margins — not simulation. Full tables: [`validation/results/`](validation/results/).

| Platform | Toolchain | Modules | Tests |
|---|---|---|---|
| x86-64 — Intel i7-13700H / Ubuntu 22.04 | gcc 11.4.0 -O2 / float32 | 13 / 13 | 300 / 300 ✅ |
| ARM64 — Apple M4 Pro / macOS 26.2 | Apple clang 21.0.0 -O2 / float32 | 13 / 13 | 300 / 300 ✅ |
| ARM64 — Apple M1 Pro / macOS 26.2 | Apple clang 17.0.0 -O2 / float32 | 13 / 13 | 300 / 300 ✅ |
| Windows x64 — MSVC 14.51 (VS 2026 Build Tools) | MSVC /O2 / float32 | 13 / 13 | 295 / 295 ✅ |
| Windows x64 — MSVC 14.51 (VS 2026 Build Tools) | MSVC /O2 / float64 | 13 / 13 | 294 / 294 ✅ |
| ESP32-S3 — Xtensa LX7 / ESP-IDF v5.5.2 | xtensa-esp32s3-elf-gcc -O2 / float32 | 13 / 13 | 548 / 550 ✅ |
| ARM64 — Raspberry Pi 4B / Raspbian GNU/Linux 13 | gcc 14.2.0 -O2 / float32 | 13 / 13 | 300 / 300 ✅ |

> ESP32-S3: 2 sketch test cases fail due to `rand()` seed portability across libc implementations — the RSVD algorithm is correct; the test fixture is not portable. See FLAG S-01 in [`validation/results/sketch/sketch.md`](validation/results/sketch/sketch.md).

---

## Quick start

**Option A — FetchContent (recommended)**

```cmake
include(FetchContent)
FetchContent_Declare(numx
  GIT_REPOSITORY https://github.com/NIKX-Tech/numx.git
  GIT_TAG        v0.1.0
)
FetchContent_MakeAvailable(numx)
target_link_libraries(my_target PRIVATE numx::numx)
```

**Option B — clone and build locally**

```bash
git clone https://github.com/NIKX-Tech/numx.git
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Switch to double precision:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DNUMX_USE_DOUBLE=1
```

**Option C — standalone platform test builds**

Self-contained CMake projects for per-platform float32/float64 validation:

```bash
# float32 Unity tests (x86)
cmake -S tests/x86 -B tests/x86/build && cmake --build tests/x86/build

# float64 Unity tests (x64)
cmake -S tests/x64 -B tests/x64/build && cmake --build tests/x64/build
```

For ESP32: see [`tests/esp32_tests/CMakeLists.txt`](tests/esp32_tests/CMakeLists.txt) (IDF component).

---

## Usage example

```c
#include "numx/numx.h"

int main(void) {
    /* Switch precision via -DNUMX_USE_DOUBLE at compile time.      */
    /* All functions return numx_status_t — always check it.        */
    numx_real_t result;
    numx_status_t s = numx_integrate_simpson(my_func, 0.0f, 1.0f, 100, &result);
    if (s != NUMX_OK) {
        /* handle error */
        return 1;
    }
    return 0;
}
```

---

## Benchmarks

Per-call averages measured on physical hardware. Full tables: [`validation/results/`](validation/results/). Platform-specific benchmark runners: [`benchmarks/bench_runner.c`](benchmarks/bench_runner.c) (Linux/macOS), [`benchmarks/bench_win.c`](benchmarks/bench_win.c) (Windows), [`benchmarks/esp32/`](benchmarks/esp32/) (ESP32 Phase 1 & 2).

| Function | x86-64 (gcc -O2) | ESP32-S3 (240 MHz) |
|---|---|---|
| `numx_vec_dot` n=64 | 15 ns | 5.5 µs |
| `numx_mat_mul` 4×4 | 26 ns | 11 µs |
| `numx_lu_decompose` 4×4 | 29 ns | 10 µs |
| `numx_integrate_gauss` npts=8 | 11 ns | 2.7 µs |
| `numx_root_brent` tol=1e-6 | 103 ns | 20 µs |
| `numx_ode_rk45` tol=1e-4 | 239 ns | 40 µs |
| `numx_stats_median` n=128 | 6.6 µs | 1.1 ms |
| `numx_fft_f32` N=64 | 3.6 µs | 2.6 ms |
| `numx_autodiff` fwd chain-10 | 20 ns | 358 ns |

---

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for the rules, templates, and the exact
prompt pattern to use with Claude Code when adding a new algorithm.

---

## License

MIT — see [LICENSE](LICENSE).

---

## Citation

Once the accompanying paper is published, cite as:

```bibtex
@software{numx2026,
  author  = {NIKX Technologies},
  title   = {numx: Scientific Numerical Computing for Embedded Systems},
  year    = {2026},
  url     = {https://github.com/NIKX-Tech/numx},
  license = {MIT}
}
```
