# numx

A portable, scientific-grade numerical computing library in pure C99 for resource-constrained embedded systems.

---

## Why numx

Most numerical libraries assume a heap, an OS, and a math coprocessor. numx assumes none of those things. It targets the processors where computation is hardest: ESP32, ARM Cortex-M/A, AVR, RISC-V — and it brings algorithms previously considered out of reach (automatic differentiation, compressed sensing, post-quantum NTT) to those platforms.

Every function is reentrant, allocation-free, and returns a typed status code. The entire precision of the library — float32 or float64 — is switchable with a single compile flag.

---

## Module status

| Module | Functions | Status |
|---|---|---|
| **linalg** | dot, norm, cross, mat_mul, transpose, det, LU | 🔧 planned |
| **stats** | mean, variance, median, percentile | 🔧 planned |
| **roots** | bisect, newton, brent | 🔧 planned |
| **integrate** | trap, simpson, gauss | 🔧 planned |
| **differentiate** | forward, central, richardson | 🔧 planned |
| **interpolate** | linear, cubic spline, chebyshev | 🔧 planned |
| **ode** | rk4, rk45 (adaptive) | 🔧 planned |
| **signal** | windows, FIR, IIR, convolve, correlate, peaks, EMA | 🔧 planned |
| **fft** | Cooley-Tukey f32, Q15, IFFT | 🔧 planned |
| **autodiff** | forward-mode (dual numbers), reverse-mode (static tape) | 🔧 planned |
| **compressed_sensing** | OMP, ISTA | 🔧 planned |
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
