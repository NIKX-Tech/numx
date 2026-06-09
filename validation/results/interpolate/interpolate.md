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

## ESP32-S3 — ESP-IDF v5.5.2 / Xtensa LX7 / xtensa-esp32s3-elf-gcc / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-29 | **Commit:** d81b386

### Test cases — linear

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| linear rc | rc=0 | rc=0 | — | ✅ |
| linear x=0.5 → 0.5 | 0.5 | 0.5000000 | 0.00e+00 | ✅ |
| linear x=1.5 → 2.5 | 2.5 | 2.5000000 | 0.00e+00 | ✅ |
| linear at knots | exact | exact | — | ✅ |
| linear clamp-below | 10.0 | 10.0000000 | 0.00e+00 | ✅ |
| linear clamp-above | 30.0 | 30.0000000 | 0.00e+00 | ✅ |
| linear n=2 midpoint=4 | 4.0 | 4.0000000 | 0.00e+00 | ✅ |
| linear null-xs | rc=-1 | rc=-1 | — | ✅ |
| linear null-ys | rc=-1 | rc=-1 | — | ✅ |
| linear null-out | rc=-1 | rc=-1 | — | ✅ |
| linear n<2 | rc=-2 | rc=-2 | — | ✅ |
| linear n=0 | rc=-2 | rc=-2 | — | ✅ |

### Test cases — spline_cubic

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| spline precompute rc | rc=0 | rc=0 | — | ✅ |
| spline linear data x=0.5 | 0.5 | 0.5000000 | 0.00e+00 | ✅ |
| spline linear data x=1.7 | 1.7 | 1.7000000 | 0.00e+00 | ✅ |
| spline cubic oneshot rc | rc=0 | rc=0 | — | ✅ |
| spline cubic at 0.5 | 0.35 | 0.3500000 | 2.98e-08 | ✅ |
| spline at knots | exact | exact | — | ✅ |
| spline clamp-below | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| spline clamp-above | 9.0 | 9.0000000 | 0.00e+00 | ✅ |
| spline n=2 linear midpoint=2 | 2.0 | 2.0000000 | 0.00e+00 | ✅ |
| spline precompute null-xs | rc=-1 | rc=-1 | — | ✅ |
| spline precompute null-ys | rc=-1 | rc=-1 | — | ✅ |
| spline precompute null-m | rc=-1 | rc=-1 | — | ✅ |
| spline precompute n<2 | rc=-2 | rc=-2 | — | ✅ |

### Test cases — chebyshev

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| cheb const rc | rc=0 | rc=0 | — | ✅ |
| cheb constant=3 | 3.0 | 3.0000002 | 2.38e-07 | ✅ |
| cheb linear=1.0 | 1.0 | 1.0000001 | 1.19e-07 | ✅ |
| cheb x²  at 0.5 | 0.25 | 0.2500000 | 0.00e+00 | ✅ |
| cheb x²  at 0 | 0.0 | 0.0000000 | 7.45e-09 | ✅ |
| cheb n=2 x²  at 0 ≈ 0.5 | 0.5 | 0.5000000 | 0.00e+00 | ✅ |
| cheb null-f | rc=-1 | rc=-1 | — | ✅ |
| cheb null-out | rc=-1 | rc=-1 | — | ✅ |
| cheb b<=a | rc=-2 | rc=-2 | — | ✅ |
| cheb a=b | rc=-2 | rc=-2 | — | ✅ |
| cheb n<2 | rc=-2 | rc=-2 | — | ✅ |
| cheb n=0 | rc=-2 | rc=-2 | — | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| interp_linear n=16 | 5,000 | 6,907 µs | 1,381 ns |
| interp_spline_cubic n=16 | 5,000 | 167,179 µs | 33,435 ns |
| interp_spline_eval (precomp) | 5,000 | 12,395 µs | 2,479 ns |
| interp_spline_precompute n=16 | 5,000 | 153,953 µs | 30,790 ns |
| interp_chebyshev n=8 | 5,000 | 631,558 µs | 126,311 ns |
| interp_chebyshev n=16 | 1,000 | 237,083 µs | 237,083 ns |

### Precision vs reference

| Function | Input | Exact | Computed | Error |
|----------|-------|-------|----------|-------|
| linear | x=0.5 | 0.5 | 0.5000000 | 0.00e+00 |
| linear | x=1.5 | 2.5 | 2.5000000 | 0.00e+00 |
| spline linear | x=0.5 | 0.5 | 0.5000000 | 0.00e+00 |
| spline cubic | x=0.5 | 0.35 | 0.3500000 | 2.98e-08 |
| chebyshev | constant=3 | 3.0 | 3.0000002 | 2.38e-07 |
| chebyshev | linear x=1 | 1.0 | 1.0000001 | 1.19e-07 |
| chebyshev | x² at 0.5 | 0.25 | 0.2500000 | 0.00e+00 |
| chebyshev | x² at 0 | 0.0 | 0.0000000 | 7.45e-09 |

*Chebyshev errors of 2.38e-07 and 1.19e-07 are at float32 machine epsilon (~1.2e-07) — expected for polynomial interpolation in floating-point arithmetic. Spline cubic error of 2.98e-08 is sub-epsilon.*

**RESULTS: 37 PASS / 0 FAIL / 37 TOTAL**

---

## Windows x64 — Windows 11 / MSVC 14.51 (VS 2026 Build Tools) / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-06-05 | **Commit:** 4c4c0f0

### Test cases

| Test | Result |
|------|--------|
| test_linear_midpoint_known | ✅ |
| test_linear_second_interval | ✅ |
| test_linear_at_knots | ✅ |
| test_linear_clamp_below | ✅ |
| test_linear_clamp_above | ✅ |
| test_linear_n2 | ✅ |
| test_linear_null | ✅ |
| test_linear_n_lt_2 | ✅ |
| test_spline_linear_data_exact | ✅ |
| test_spline_cubic_oneshot_midpoint | ✅ |
| test_spline_at_knots | ✅ |
| test_spline_clamp_below | ✅ |
| test_spline_clamp_above | ✅ |
| test_spline_n2 | ✅ |
| test_spline_precompute_null | ✅ |
| test_spline_precompute_n_lt_2 | ✅ |
| test_cheb_constant | ✅ |
| test_cheb_linear_exact | ✅ |
| test_cheb_quadratic_exact | ✅ |
| test_cheb_quadratic_at_zero | ✅ |
| test_cheb_n2_gives_linear_interp | ✅ |
| test_cheb_null | ✅ |
| test_cheb_b_le_a | ✅ |
| test_cheb_n_lt_2 | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| interp_linear n=16 | 50,000 | 504 µs | 10 ns |
| interp_spline_cubic n=16 | 50,000 | 11,000 µs | 220 ns |
| interp_spline_eval (precomp) | 50,000 | 563 µs | 11 ns |
| interp_spline_precompute n=16 | 50,000 | 9,608 µs | 192 ns |
| interp_chebyshev n=8 | 50,000 | 38,619 µs | 772 ns |
| interp_chebyshev n=16 | 10,000 | 15,942 µs | 1,594 ns |

**RESULTS: 24 PASS / 0 FAIL / 24 TOTAL**

---

## Windows x64 — Windows 11 / MSVC 14.51 (VS 2026 Build Tools) / float64
**Validator:** Amir Ab Khoshk | **Date:** 2026-06-06 | **Commit:** 1bba399

### Test cases

| Test | Result |
|------|--------|
| test_linear_midpoint_known | ✅ |
| test_linear_second_interval | ✅ |
| test_linear_at_knots | ✅ |
| test_linear_clamp_below | ✅ |
| test_linear_clamp_above | ✅ |
| test_linear_n2 | ✅ |
| test_linear_null | ✅ |
| test_linear_n_lt_2 | ✅ |
| test_spline_linear_data_exact | ✅ |
| test_spline_cubic_oneshot_midpoint | ✅ |
| test_spline_at_knots | ✅ |
| test_spline_clamp_below | ✅ |
| test_spline_clamp_above | ✅ |
| test_spline_n2 | ✅ |
| test_spline_precompute_null | ✅ |
| test_spline_precompute_n_lt_2 | ✅ |
| test_cheb_constant | ✅ |
| test_cheb_linear_exact | ✅ |
| test_cheb_quadratic_exact | ✅ |
| test_cheb_quadratic_at_zero | ✅ |
| test_cheb_n2_gives_linear_interp | ✅ |
| test_cheb_null | ✅ |
| test_cheb_b_le_a | ✅ |
| test_cheb_n_lt_2 | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| interp_linear n=16 | 50,000 | 386 µs | 7 ns |
| interp_spline_cubic n=16 | 50,000 | 11,040 µs | 220 ns |
| interp_spline_eval (precomp) | 50,000 | 555 µs | 11 ns |
| interp_spline_precompute n=16 | 50,000 | 9,935 µs | 198 ns |
| interp_chebyshev n=8 | 50,000 | 44,093 µs | 881 ns |
| interp_chebyshev n=16 | 10,000 | 17,814 µs | 1,781 ns |

**RESULTS: 24 PASS / 0 FAIL / 24 TOTAL**

---

## ARM64 — macOS 26.2 / Apple M1 Pro / Apple clang 17.0.0 / float32
**Validator:** Erfan Jazeb Nikoo | **Date:** 2026-06-08 | **Commit:** 1380ab1

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
| interp_linear | 5 | 50,000 | 124 µs | 2 ns |
| interp_spline_cubic (one-shot) | 5 | 50,000 | 836 µs | 16 ns |
| interp_spline_eval (pre-built) | 5 | 50,000 | 172 µs | 3 ns |
| interp_chebyshev n=8 | 8 | 50,000 | 5,942 µs | 118 ns |

### Precision vs numpy reference

| Function | ref | numx | Error |
|----------|-----|------|-------|
| linear x=1.5 | 2.5 | 2.50000000 | 0.00e+00 |
| spline x=1.5 | 2.23214293 | 2.23214293 | 0.00e+00 |
| spline x=2.5 | 6.23214293 | 6.23214293 | 0.00e+00 |
| chebyshev n=8 x=1.5 | 2.25 | 2.25000024 | 2.38e-07 |

---

## ARM64 — macOS 26.2 / Apple M4 Pro / Apple clang 21.0.0 / float32 (Updated)
**Validator:** Erfan Jazeb Nikoo | **Date:** 2026-06-09 | **Commit:** 2fc85d0

### Test cases

*300 / 300 Unity tests PASS*

### Performance

*interpolate was not included in M4 Pro benchmark suite for this run.*
