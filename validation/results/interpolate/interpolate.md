# Validation: numx_interpolate (linear / spline / chebyshev)

---

## x86-64 — Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-25 | **Commit:** d81b386

### Test cases

| Function | Input | Expected | Computed | Pass |
|----------|-------|----------|----------|------|
| linear midpoint | x=1.5 | 2.5 | 2.5 | ✅ |
| linear at knots | — | exact | exact | ✅ |
| linear clamp | outside range | boundary | boundary | ✅ |
| spline at knots | — | exact | exact | ✅ |
| chebyshev constant | — | const | const | ✅ |
| chebyshev linear | — | exact | exact | ✅ |
| n<2 → ERR_INVALID_ARG | — | -2 | -2 | ✅ |

*All 26 Unity tests: PASS (test_interpolate.c:287–312)*

### Performance

| Function | n | N | Total | Per call |
|----------|---|---|-------|----------|
| interp_linear | 16 | 50,000 | 155 µs | 3 ns |
| interp_spline_cubic (one-shot) | 16 | 50,000 | 6,102 µs | 122 ns |
| interp_spline_precompute | 16 | 50,000 | 3,980 µs | 79 ns |
| interp_spline_eval (pre-built) | 16 | 50,000 | 185 µs | 3 ns |
| interp_chebyshev n=8 | 8 | 50,000 | 15,784 µs | 315 ns |
| interp_chebyshev n=16 | 16 | 50,000 | 36,194 µs | 723 ns |

### Precision vs numpy reference (nodes: xs=[0..4], ys=[0,1,4,9,16])

| Function | ref | numx | Error | Note |
|----------|-----|------|-------|------|
| linear at x=1.5 | 2.5 | 2.50000000 | 0.00e+00 | ✅ |
| spline at x=1.5 | 2.25 (exact x²) | 2.23214293 | 1.79e-02 | ⚠️ natural BC |
| spline at x=2.5 | 6.25 (exact x²) | 6.23214293 | 1.79e-02 | ⚠️ natural BC |
| chebyshev n=8 at x=1.5 | 2.25 | 2.25000000 | 0.00e+00 | ✅ |

⚠️ **spline error 1.79e-02:** natural BC forces m[0]=m[n-1]=0; x² has curvature=2 at endpoints. Expected deviation — scipy gives identical result. Not a bug.

---

## ARM64 — macOS 26.2 / Apple M4 Pro / Apple clang 21.0.0 / float32
**Validator:** Erfan Jazeb Nikoo | **Date:** 2026-05-29 | **Commit:** 37e581f

### Test cases (nodes: xs=[0,1,2,3,4], ys=[0,1,4,9,16])

| Function | Input | Expected | Computed | Error | Pass |
|----------|-------|----------|----------|-------|------|
| linear | x=1.5 | 2.5 | 2.50000000 | 0.00e+00 | ✅ |
| linear | x=2.5 | 6.5 | 6.50000000 | 0.00e+00 | ✅ |
| spline_cubic | x=1.5 | 2.23214293 | 2.23214293 | 0.00e+00 | ✅ |
| spline_cubic | x=2.5 | 6.23214293 | 6.23214293 | 0.00e+00 | ✅ |
| chebyshev n=8 | x=1.5 | 2.25 | 2.25000024 | 2.38e-07 | ✅ |
| chebyshev n=16 | x=1.5 | 2.25 | 2.25000000 | 0.00e+00 | ✅ |

*300 / 300 Unity tests PASS*

### Performance

| Function | n | N | Total | Per call |
|----------|---|---|-------|----------|
| interp_linear | 5 | 50,000 | 114 µs | 2 ns |
| interp_spline_cubic (one-shot) | 5 | 50,000 | 685 µs | 13 ns |
| interp_spline_eval (pre-built) | 5 | 50,000 | 147 µs | 2 ns |
| interp_chebyshev n=8 | 8 | 50,000 | 6,872 µs | 137 ns |

### Precision vs numpy reference

| Function | ref | numx | Error |
|----------|-----|------|-------|
| linear x=1.5 | 2.5 | 2.50000000 | 0.00e+00 |
| spline x=1.5 | 2.23214293 | 2.23214293 | 0.00e+00 |
| spline x=2.5 | 6.23214293 | 6.23214293 | 0.00e+00 |
| chebyshev n=8 x=1.5 | 2.25 | 2.25000024 | 2.38e-07 |

---

## ESP32-S3
**Status:** ⚠️ Pending
