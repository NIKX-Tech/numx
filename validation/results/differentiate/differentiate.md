# Validation: numx_differentiate (forward / central / richardson)

**Validator:** Amir Ab Khoshk
**Date:** 2026-05-25
**numx version:** d81b386

## Hardware: ESP32
*⚠️ Pending.*

## Hardware: Host
- OS: Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 -O2 / float32

## Test results

| Test case                      | Expected | numx output | Pass/Fail |
|-------------------------------|---------|-------------|-----------|
| forward quadratic at 3        | 6.0     | ≈6.0        | ✅        |
| central quadratic at 3        | 6.0     | ≈6.0        | ✅        |
| richardson more accurate       | better  | better      | ✅        |
| constant function → 0         | 0.0     | 0.0         | ✅        |
| linear function → 1 (central) | 1.0     | exact       | ✅        |
| h≤0 → ERR_INVALID_ARG         | -2      | -2          | ✅        |

*All 18 Unity tests: PASS (test_differentiate.c:217–237)*

## Performance (x86-64)

| Function         | N       | Total   | Per call |
|------------------|---------|---------|----------|
| diff_forward     | 100,000 | 300 µs  | 3 ns     |
| diff_central     | 100,000 | 261 µs  | 2 ns     |
| diff_richardson  | 100,000 | 430 µs  | 4 ns     |
| ESP32            | —       | *pending*| *pending*|

## Python comparison (f(x)=x³ at x=2, exact f′=12.0, h=1e-4)

| Function     | double ref  | numx (float32) | Abs error | Within 1e-5 | Notes |
|--------------|------------|----------------|-----------|-------------|-------|
| forward      | 12.0006000 | 11.98768616    | 1.23e-02  | ❌          | See flags below |
| central      | 12.0000000 | 11.99483871    | 5.16e-03  | ❌          | See flags below |
| richardson   | 12.0000000 | 12.00437546    | 4.38e-03  | ❌          | See flags below |

## ⚠️ FLAG: Precision worse than 1e-5 — float32 cancellation at h=1e-4

All three methods report errors in the 1e-2 to 1e-3 range when using h=1e-4 with float32.
This is a **known float32 cancellation effect**, not a code bug.

**Root cause:** For f(x)=x³ at x=2.0:
- f(x)=8.0 and f(x+h)≈8.0012 differ in the 4th decimal place.
- float32 has only ~7 significant digits, so subtracting two nearly-equal values cancels ~3 digits of precision.
- h=1e-4 falls into the "too-small-for-float32" regime.

**Optimal h values for float32 (ε≈1.2e-7):**

| Method      | Optimal h       | Best achievable error |
|-------------|-----------------|----------------------|
| forward     | √ε ≈ 3.46e-4    | ~1e-4                |
| central     | ε^(1/3) ≈ 5e-3  | ~1e-4                |
| richardson  | ε^(1/4) ≈ 1.9e-2| ~1e-4                |

**Confirmed by Python float32 simulation:** best achievable accuracy with any h is ~1e-4 for these methods. 1e-5 is **not reachable** with single-precision input.

**Recommendation:** Document the h sensitivity in the API. If 1e-5 precision is required, use `NUMX_USE_DOUBLE`.
