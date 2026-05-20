# numx

[![CI](https://img.shields.io/github/actions/workflow/status/NIKX-Tech/numx/ci.yml?branch=dev&style=flat-square&label=build)](https://github.com/NIKX-Tech/numx/actions/workflows/ci.yml) [![License](https://img.shields.io/github/license/NIKX-Tech/numx?style=flat-square&color=blue)](https://opensource.org/licenses/MIT) [![C Standard](https://img.shields.io/badge/standard-C99-00599C?style=flat-square&logo=c&logoColor=white)](https://github.com/NIKX-Tech/numx) [![Repo Size](https://img.shields.io/github/repo-size/NIKX-Tech/numx?style=flat-square)](https://github.com/NIKX-Tech/numx) [![GitHub Stars](https://img.shields.io/github/stars/NIKX-Tech/numx?style=flat-square&color=yellow)](https://github.com/NIKX-Tech/numx/stargazers) [![Platform: ESP32](https://img.shields.io/badge/platform-ESP32-E7352C?style=flat-square&logo=espressif&logoColor=white)](https://github.com/NIKX-Tech/numx) [![Platform: ARM](https://img.shields.io/badge/platform-ARM%20Cortex--M-0091BD?style=flat-square&logo=arm&logoColor=white)](https://github.com/NIKX-Tech/numx)

<!--
Uncomment each badge when the prerequisite is met:
  [![OpenSSF Scorecard](...)](...)       → add .github/workflows/scorecard.yml
  [![CodeQL](...)](...)                  → add .github/workflows/codeql.yml
  [![Latest Release](...)](...)          → publish first GitHub release
  [![Dependabot](...)](...)              → add .github/dependabot.yml
  [![Sponsor GitHub](...)](...)          → activate GitHub Sponsors for NIKX-Tech
  [![Sponsor Open Collective](...)](...)  → create Open Collective project page
  [![Website](...)](...)                 → launch numx.dev
  [![Discord](...)](...)                 → create Discord server

[![OpenSSF Scorecard](https://img.shields.io/ossf-scorecard/github.com/NIKX-Tech/numx?label=openssf%20scorecard&style=flat-square)](https://securityscorecards.dev/projects/github.com/NIKX-Tech/numx)
[![CodeQL](https://img.shields.io/github/actions/workflow/status/NIKX-Tech/numx/codeql.yml?branch=dev&label=codeql&style=flat-square)](https://github.com/NIKX-Tech/numx/actions/workflows/codeql.yml)
[![Latest Release](https://img.shields.io/github/v/release/NIKX-Tech/numx?style=flat-square)](https://github.com/NIKX-Tech/numx/releases)
[![Dependabot](https://img.shields.io/badge/dependabot-enabled-025e8a?logo=dependabot&style=flat-square)](https://github.com/NIKX-Tech/numx/blob/dev/.github/dependabot.yml)
[![Sponsor GitHub](https://img.shields.io/badge/sponsor-GitHub-EA4AAA?style=flat-square&logo=github-sponsors)](https://github.com/sponsors/NIKX-Tech)
[![Sponsor Open Collective](https://img.shields.io/badge/sponsor-Open%20Collective-00A0E0?style=flat-square&logo=opencollective)](https://opencollective.com/nikx-technologies/projects/numx)
[![Website](https://img.shields.io/badge/website-numx.dev-4F46E5?style=flat-square&logo=google-chrome&logoColor=white)](https://numx.dev)
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
| **sketch** | randomized SVD (Halko-Martinsson-Tropp) | 🔧 planned |
| **ntt** | Number Theoretic Transform (constant-time, Kyber/Dilithium params) | 🔧 planned |

---

## Quick start

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

*Results will be populated after Phase 4 benchmark runs on target hardware.*

| Function | ESP32 (240 MHz) | Cortex-M4 (168 MHz) | x86-64 |
|---|---|---|---|
| — | — | — | — |

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
