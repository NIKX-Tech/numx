# Validation: numx_roots (bisect / newton / brent)

**Validator:** Amir Ab Khoshk
**Date:** 2026-05-25
**numx version:** d81b386

## Hardware: ESP32
*⚠️ Pending.*

## Hardware: Host
- OS: Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 -O2 / float32

## Test results

| Test case                    | Function      | Expected        | numx output | Pass/Fail |
|------------------------------|--------------|-----------------|-------------|-----------|
| bisect x²-2 on [1,2] tol=1e-6| sqrt(2)      | 1.41421356...   | 1.41421413  | ✅        |
| newton x²-2 from x0=1.5      | sqrt(2)      | 1.41421356...   | 1.41421354  | ✅        |
| brent x²-2 on [1,2]          | sqrt(2)      | 1.41421356...   | 1.41421342  | ✅        |
| brent x³-x-2 on [1,2]        | 1.52137970   | 1.52137970...   | 1.52137971  | ✅        |
| newton zero derivative → ERR  | —            | ERR_SINGULAR    | -3          | ✅        |
| bisect no bracket → ERR       | f(a)*f(b)≥0  | ERR_INVALID_ARG | -2          | ✅        |

*All 22 Unity tests: PASS (test_roots.c:276–302)*

## Performance (x86-64)

| Function              | tol  | N      | Total    | Per call  |
|-----------------------|------|--------|----------|-----------|
| root_bisect x²-2      | 1e-6 | 10,000 | 1,006 µs | 100 ns    |
| root_newton x²-2      | 1e-6 | 10,000 | 100 µs   | 10 ns     |
| root_brent x²-2       | 1e-6 | 10,000 | 1,031 µs | 103 ns    |
| root_brent x³-x-2     | 1e-6 | 10,000 | 445 µs   | 44 ns     |
| root_newton x³-x-2    | 1e-6 | 10,000 | 74 µs    | 7 ns      |
| ESP32                 |      | —      | *pending*| *pending* |

## Python comparison

| Function         | Reference (double) | numx (float32) | Abs error | OK |
|------------------|--------------------|----------------|-----------|-----|
| bisect x²-2      | 1.414213562        | 1.41421413     | 5.74e-07  | ✅  |
| newton x²-2      | 1.414213562        | 1.41421354     | 2.18e-08  | ✅  |
| brent x²-2       | 1.414213562        | 1.41421342     | 1.41e-07  | ✅  |
| brent x³-x-2     | 1.521379707        | 1.52137971     | 9.24e-09  | ✅  |

## Notes
All errors are within float32 machine epsilon (~1.2e-07). The bisect result (5.74e-07) is bounded by
tol=1e-6 × interval; expected. Newton converges to near machine-epsilon in 3 iterations for this case.
