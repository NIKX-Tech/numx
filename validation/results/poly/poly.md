# Validation: numx_poly (poly_eval / poly_roots)

**Validator:** Amir Ab Khoshk
**Date:** 2026-05-25
**numx version:** d81b386

## Hardware: ESP32
*⚠️ Pending.*

## Hardware: Host
- OS: Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 -O2 / float32

## Test results

| Test case                       | Expected     | numx output  | Pass/Fail |
|--------------------------------|-------------|--------------|-----------|
| poly_eval quadratic at 3        | 9.0         | 9.0          | ✅        |
| poly_eval cubic at 2            | exact       | exact        | ✅        |
| poly_eval at zero               | coeffs[deg] | exact        | ✅        |
| poly_eval degree-0              | constant    | constant     | ✅        |
| poly_roots linear (1 root)      | known       | known        | ✅        |
| poly_roots quadratic 2 reals    | exact       | exact        | ✅        |
| poly_roots cubic 3 reals        | [1,2,3]     | [1,2,3]±ε   | ✅        |
| poly_roots degree-0 → rejected  | ERR         | -2           | ✅        |
| null args → ERR_NULL_PTR        | -1          | -1           | ✅        |

*All 17 Unity tests: PASS (test_poly.c:229–245)*

## Performance (x86-64)

| Function              | N       | Total   | Per call  |
|-----------------------|---------|---------|-----------|
| poly_eval degree=3    | 100,000 | 132 µs  | 1 ns      |
| poly_eval degree=8    | 100,000 | 211 µs  | 2 ns      |
| poly_roots degree=3   | 1,000   | 104 µs  | 104 ns    |
| ESP32                 | —       | *pending*| *pending*|

## Python comparison (p(x)=x³-6x²+11x-6, roots=[1,2,3])

| Input  | numpy p(x)   | numx p(x)    | Error    | OK |
|--------|-------------|--------------|----------|----|
| x=1.0  | 0.0         | 0.00000000   | 0.00e+00 | ✅ |
| x=2.0  | 0.0         | 0.00000000   | 0.00e+00 | ✅ |
| x=3.0  | 0.0         | 0.00000000   | 0.00e+00 | ✅ |
| x=2.5  | -0.375      | -0.37500000  | 0.00e+00 | ✅ |
| x=0.0  | -6.0        | -6.00000000  | 0.00e+00 | ✅ |

| Root | numpy ref | numx found   | Error    | OK |
|------|-----------|-------------|----------|----|
| 1    | 1.000000  | 1.000000    | 0.00e+00 | ✅ |
| 2    | 2.000000  | 2.000001    | 1.00e-06 | ✅ |
| 3    | 3.000000  | 2.999999    | 1.00e-06 | ✅ |

Degree-8 eval at x=1.5: numpy=12.44140625, numx=12.44140625, error=0.00e+00 ✅

## Notes
Poly_eval uses Horner's method — exact for integer coefficients and integer inputs (zero rounding). Root errors of ~1e-6 are within the requested tol=1e-6 and bounded by float32 representability.
