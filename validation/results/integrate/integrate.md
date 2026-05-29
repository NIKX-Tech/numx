# Validation: numx_integrate (trap / simpson / gauss)

**Validator:** Amir Ab Khoshk
**Date:** 2026-05-25
**numx version:** d81b386

## Hardware: ESP32
*⚠️ Pending.*

## Hardware: Host
- OS: Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 -O2 / float32

## Test results

| Test case                    | Expected | numx output  | Pass/Fail |
|------------------------------|---------|--------------|-----------|
| trap constant f=1 on [0,1]   | 1.0     | 1.00000000   | ✅        |
| trap linear exact             | exact   | exact        | ✅        |
| trap quadratic (O(h²) error)  | ≈exact  | ≈exact       | ✅        |
| simpson linear exact          | exact   | exact        | ✅        |
| simpson cubic exact           | exact   | exact        | ✅        |
| gauss-2 linear exact          | exact   | exact        | ✅        |
| gauss-4 quadratic exact       | exact   | exact        | ✅        |
| gauss-8 degree-15 exact       | exact   | exact        | ✅        |
| odd n for simpson → rejected  | ERR     | -2           | ✅        |

*All 24 Unity tests: PASS (test_integrate.c:232–256)*

## Performance (x86-64)

| Function                | n    | N      | Total     | Per call  |
|-------------------------|------|--------|-----------|-----------|
| integrate_trap n=100    | 100  | 50,000 | 6,999 µs  | 139 ns    |
| integrate_trap n=1000   | 1000 | 5,000  | 8,619 µs  | 1,723 ns  |
| integrate_simpson n=100 | 100  | 50,000 | 9,167 µs  | 183 ns    |
| integrate_simpson n=1000| 1000 | 5,000  | 8,527 µs  | 1,705 ns  |
| integrate_gauss npts=2  | 2    | 50,000 | 240 µs    | 4 ns      |
| integrate_gauss npts=4  | 4    | 50,000 | 310 µs    | 6 ns      |
| integrate_gauss npts=8  | 8    | 50,000 | 570 µs    | 11 ns     |
| ESP32                   |      | —      | *pending* | *pending* |

## Python comparison (integrand: f(x)=x³+1 on [0,1], exact=1.25)

| Function         | numpy ref    | numx         | Abs error | Within tol | Notes |
|------------------|-------------|--------------|-----------|------------|-------|
| trap n=100       | 1.2500250000 | 1.25002503   | 2.50e-05  | ⚠️ > 1e-5 | Expected — method truncation error, not numx error |
| trap n=1000      | 1.2500002500 | 1.25000095   | 9.54e-07  | ✅         |       |
| simpson n=100    | 1.2500000000 | 1.25000012   | 1.19e-07  | ✅         |       |
| gauss npts=2     | 1.2500000000 | 1.25000000   | 0.00e+00  | ✅         |       |
| gauss npts=4     | 1.2500000000 | 1.25000000   | 0.00e+00  | ✅         |       |
| gauss npts=8     | 1.2500000000 | 1.25000000   | 0.00e+00  | ✅         |       |

## Notes
⚠️ **trap n=100 error (2.50e-05) exceeds 1e-5** — this is the inherent O(h²) truncation error of the trapezoidal rule at n=100, **not a numx precision bug**. numx's output exactly matches the theoretical trapezoidal approximation. Gauss quadrature achieves machine-epsilon accuracy for polynomial integrands — recommended for embedded use.
