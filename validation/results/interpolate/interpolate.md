# Validation: numx_interpolate (linear / spline / chebyshev)

**Validator:** Amir Ab Khoshk
**Date:** 2026-05-25
**numx version:** d81b386

## Hardware: ESP32
*⚠️ Pending.*

## Hardware: Host
- OS: Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 -O2 / float32

## Test results

| Test case                         | Expected | numx output | Pass/Fail |
|----------------------------------|---------|-------------|-----------|
| linear midpoint known             | 2.5     | 2.5         | ✅        |
| linear at knots                   | exact   | exact       | ✅        |
| linear clamp below/above          | boundary| boundary    | ✅        |
| spline linear data (exact)        | exact   | exact       | ✅        |
| spline at knots                   | exact   | exact       | ✅        |
| chebyshev constant                | const   | const       | ✅        |
| chebyshev linear exact            | exact   | exact       | ✅        |
| chebyshev quadratic exact (n≥3)   | exact   | exact       | ✅        |
| n<2 → ERR_INVALID_ARG             | -2      | -2          | ✅        |

*All 26 Unity tests: PASS (test_interpolate.c:287–312)*

## Performance (x86-64)

| Function                    | n  | N      | Total      | Per call  |
|-----------------------------|-----|--------|------------|-----------|
| interp_linear               | 16  | 50,000 | 155 µs     | 3 ns      |
| interp_spline_cubic (1-shot) | 16 | 50,000 | 6,102 µs   | 122 ns    |
| interp_spline_precompute    | 16  | 50,000 | 3,980 µs   | 79 ns     |
| interp_spline_eval (prebuilt)| 16 | 50,000 | 185 µs     | 3 ns      |
| interp_chebyshev n=8        | 8   | 50,000 | 15,784 µs  | 315 ns    |
| interp_chebyshev n=16       | 16  | 50,000 | 36,194 µs  | 723 ns    |
| ESP32                       | —   | —      | *pending*  | *pending* |

## Python comparison (nodes: xs=[0..4], ys=[0,1,4,9,16] i.e. x²)

| Function                   | ref value | numx       | Abs error | Within 1e-5 | Notes |
|---------------------------|-----------|------------|-----------|-------------|-------|
| linear at x=1.5           | 2.5       | 2.50000000 | 0.00e+00  | ✅          |       |
| spline_cubic at x=1.5     | 2.25      | 2.23214293 | 1.79e-02  | ⚠️          | See flag |
| spline_cubic at x=2.5     | 6.25      | 6.23214293 | 1.79e-02  | ⚠️          | See flag |
| chebyshev n=8 at x=1.5    | 2.25      | 2.25000000 | 0.00e+00  | ✅          |       |
| chebyshev n=16 at x=1.5   | 2.25      | 2.25000000 | 0.00e+00  | ✅          |       |

## ⚠️ FLAG: spline_cubic errors 1.79e-02 — expected behavior, not a precision bug

The natural cubic spline on nodes of y=x² gives **correct spline output** but the spline does not exactly reproduce the underlying x² function.

**Root cause:** Natural spline boundary conditions enforce m[0]=m[n-1]=0 (zero second derivative at endpoints). The true x² function has second derivative 2 everywhere, including at endpoints. The zero-endpoint constraint forces the spline to deviate from x² by a systematic O(h²) amount. This is expected, correct spline behavior.

**Verification:** scipy.interpolate.CubicSpline with bc_type='natural' on the same nodes also returns 2.2321 at x=1.5 — identical to numx.

**Recommendation:** This is not a bug. For polynomial interpolation of smooth functions, `numx_interp_chebyshev` is a better choice.

## Notes
- `interp_spline_eval` (pre-built) is 40× faster than the one-shot version. Always pre-compute when evaluating at many points.
- Chebyshev at 723 ns/call (n=16) — evaluates the function at n nodes on every call; will be expensive on ESP32 if the function is complex.
