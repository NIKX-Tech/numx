# Validation: numx_integrate (trap / simpson / gauss)

---

## x86-64 — Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-25 | **Commit:** d81b386

### Test cases

| Function | n | Expected | Computed | Pass |
|----------|---|----------|----------|------|
| trap constant f=1 on [0,1] | — | 1.0 | 1.00000000 | ✅ |
| trap linear | — | exact | exact | ✅ |
| simpson linear | — | exact | exact | ✅ |
| simpson cubic | — | exact | exact | ✅ |
| gauss-2 linear | — | exact | exact | ✅ |
| gauss-4 quadratic | — | exact | exact | ✅ |
| gauss-8 degree-15 | — | exact | exact | ✅ |
| odd n for simpson → ERR | — | -2 | -2 | ✅ |

*All 24 Unity tests: PASS (test_integrate.c:232–256)*

### Performance

| Function | n | N | Total | Per call |
|----------|---|---|-------|----------|
| integrate_trap | 100 | 50,000 | 6,999 µs | 139 ns |
| integrate_trap | 1000 | 5,000 | 8,619 µs | 1,723 ns |
| integrate_simpson | 100 | 50,000 | 9,167 µs | 183 ns |
| integrate_gauss npts=2 | — | 50,000 | 240 µs | 4 ns |
| integrate_gauss npts=4 | — | 50,000 | 310 µs | 6 ns |
| integrate_gauss npts=8 | — | 50,000 | 570 µs | 11 ns |

### Precision vs numpy reference (f(x)=x³+1 on [0,1], exact=1.25)

| Function | numpy ref | numx | Error | Note |
|----------|-----------|------|-------|------|
| trap n=100 | 1.2500250000 | 1.25002503 | 2.50e-05 | ⚠️ O(h²) method error — expected |
| trap n=1000 | 1.2500002500 | 1.25000095 | 9.54e-07 | ✅ |
| simpson n=100 | 1.2500000000 | 1.25000012 | 1.19e-07 | ✅ |
| gauss npts=2 | 1.2500000000 | 1.25000000 | 0.00e+00 | ✅ |
| gauss npts=4 | 1.2500000000 | 1.25000000 | 0.00e+00 | ✅ |
| gauss npts=8 | 1.2500000000 | 1.25000000 | 0.00e+00 | ✅ |

⚠️ **trap n=100 error (2.50e-05)** is the inherent O(h²) truncation of the trapezoidal rule, not a numx bug. numx matches theoretical prediction exactly.

---

## ARM64 — macOS 26.2 / Apple M4 Pro / Apple clang 21.0.0 / float32
**Validator:** Erfan Jazeb Nikoo | **Date:** 2026-05-29 | **Commit:** 37e581f

### Test cases

| Function | Expected | Computed | Error | Pass |
|----------|----------|----------|-------|------|
| trap n=100 | 1.2500250 | 1.2500250340 | 0.00e+00 | ✅ NOTE: O(h²) |
| trap n=1000 | 1.2500003 | 1.25000095 | 5.96e-07 | ✅ |
| simpson n=100 | 1.25 | 1.25000012 | 1.19e-07 | ✅ |
| gauss npts=2 | 1.25 | 1.25000000 | 0.00e+00 | ✅ |
| gauss npts=4 | 1.25 | 1.25000000 | 0.00e+00 | ✅ |
| gauss npts=8 | 1.25 | 1.25000000 | 0.00e+00 | ✅ |

*300 / 300 Unity tests PASS*

### Performance (f(x)=x³+1 on [0,1])

| Function | n | N | Total | Per call |
|----------|---|---|-------|----------|
| integrate_trap | 100 | 50,000 | 5,871 µs | 117 ns |
| integrate_simpson | 100 | 50,000 | 5,231 µs | 104 ns |
| integrate_gauss npts=2 | — | 50,000 | 145 µs | 2 ns |

### Precision vs numpy reference

| Function | numpy | numx | Error |
|----------|-------|------|-------|
| trap n=100 | 1.2500250 | 1.2500250 | 0.00e+00 |
| trap n=1000 | 1.2500003 | 1.25000095 | 5.96e-07 |
| simpson n=100 | 1.2500000 | 1.25000012 | 1.19e-07 |
| gauss npts=2,4,8 | 1.2500000 | 1.25000000 | 0.00e+00 |

---

## ESP32-S3 — ESP-IDF v5.5.2 / Xtensa LX7 / xtensa-esp32s3-elf-gcc / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-29 | **Commit:** d81b386

### Test cases — trap

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| trap const rc | rc=0 | rc=0 | — | ✅ |
| trap const = 1 | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| trap linear = 0.5 | 0.5 | 0.5000000 | 0.00e+00 | ✅ |
| trap x² = 1/3 | 0.3333333 | 0.3333336 | 2.09e-07 | ✅ |
| trap linearity | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| trap n=1 | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| trap null-f | rc=-1 | rc=-1 | — | ✅ |
| trap null-out | rc=-1 | rc=-1 | — | ✅ |
| trap a>b | rc=-2 | rc=-2 | — | ✅ |
| trap a=b | rc=-2 | rc=-2 | — | ✅ |
| trap n=0 | rc=-2 | rc=-2 | — | ✅ |

### Test cases — simpson

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| simp const rc | rc=0 | rc=0 | — | ✅ |
| simp const = 1 | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| simp linear = 0.5 | 0.5 | 0.5000000 | 0.00e+00 | ✅ |
| simp x² = 1/3 | 0.3333333 | 0.3333333 | 0.00e+00 | ✅ |
| simp x³ = 0.25 | 0.25 | 0.2500000 | 0.00e+00 | ✅ |
| simp x² [0,3] = 9 | 9.0 | 9.0000000 | 0.00e+00 | ✅ |
| simp odd-n rejected | rc=-2 | rc=-2 | — | ✅ |
| simp n<2 rejected | rc=-2 | rc=-2 | — | ✅ |
| simp null-f | rc=-1 | rc=-1 | — | ✅ |
| simp null-out | rc=-1 | rc=-1 | — | ✅ |

### Test cases — gauss

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| gauss2 linear rc | rc=0 | rc=0 | — | ✅ |
| gauss2 linear = 0.5 | 0.5 | 0.5000000 | 0.00e+00 | ✅ |
| gauss4 x² = 1/3 | 0.3333333 | 0.3333333 | 2.98e-08 | ✅ |
| gauss8 x³ = 0.25 | 0.25 | 0.2500000 | 0.00e+00 | ✅ |
| gauss8 const [−3,3] = 6 | 6.0 | 6.0000000 | 0.00e+00 | ✅ |
| gauss null-f | rc=-1 | rc=-1 | — | ✅ |
| gauss null-out | rc=-1 | rc=-1 | — | ✅ |
| gauss bad-npts=3 | rc=-2 | rc=-2 | — | ✅ |
| gauss bad-npts=16 | rc=-2 | rc=-2 | — | ✅ |
| gauss a>b | rc=-2 | rc=-2 | — | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| integrate_trap n=100 | 5,000 | 122,438 µs | 24,487 ns |
| integrate_trap n=1000 | 1,000 | 238,439 µs | 238,439 ns |
| integrate_simpson n=100 | 5,000 | 144,343 µs | 28,868 ns |
| integrate_gauss npts=2 | 5,000 | 5,333 µs | 1,066 ns |
| integrate_gauss npts=4 | 5,000 | 8,014 µs | 1,602 ns |
| integrate_gauss npts=8 | 5,000 | 13,461 µs | 2,692 ns |

### Precision vs reference

| Function | f | Exact | Computed | Error |
|----------|---|-------|----------|-------|
| trap | x² on [0,1] | 0.3333333 | 0.3333336 | 2.09e-07 |
| simp | x² on [0,1] | 0.3333333 | 0.3333333 | 0.00e+00 |
| simp | x³ on [0,1] | 0.25 | 0.2500000 | 0.00e+00 |
| gauss4 | x² on [0,1] | 0.3333333 | 0.3333333 | 2.98e-08 |
| gauss8 | x³ on [0,1] | 0.25 | 0.2500000 | 0.00e+00 |

*trap x² error of 2.09e-07 is inherent O(h²) trapezoidal truncation, not a bug. Simpson and Gauss methods are exact for polynomials of their respective degrees.*

**RESULTS: 31 PASS / 0 FAIL / 31 TOTAL**

---

## Windows x64 — Windows 11 / MSVC 14.51 (VS 2026 Build Tools) / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-06-05 | **Commit:** 4c4c0f0

### Test cases

| Test | Result |
|------|--------|
| test_trap_constant_one | ✅ |
| test_trap_linear | ✅ |
| test_trap_quadratic | ✅ |
| test_trap_linearity | ✅ |
| test_trap_n1 | ✅ |
| test_trap_null | ✅ |
| test_trap_a_ge_b | ✅ |
| test_trap_n_zero | ✅ |
| test_simpson_constant_one | ✅ |
| test_simpson_linear_exact | ✅ |
| test_simpson_quadratic_exact | ✅ |
| test_simpson_cubic_exact | ✅ |
| test_simpson_wider_interval | ✅ |
| test_simpson_odd_n_rejected | ✅ |
| test_simpson_n_lt_2_rejected | ✅ |
| test_simpson_null | ✅ |
| test_gauss2_linear | ✅ |
| test_gauss4_quadratic_exact | ✅ |
| test_gauss8_cubic_exact | ✅ |
| test_gauss8_constant_exact | ✅ |
| test_gauss_null | ✅ |
| test_gauss_invalid_npts | ✅ |
| test_gauss_a_ge_b | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| integrate_trap n=100 | 50,000 | 58,811 µs | 1,176 ns |
| integrate_trap n=1000 | 10,000 | 122,892 µs | 12,289 ns |
| integrate_simpson n=100 | 50,000 | 65,975 µs | 1,319 ns |
| integrate_gauss npts=2 | 50,000 | 449 µs | 8 ns |
| integrate_gauss npts=4 | 50,000 | 672 µs | 13 ns |
| integrate_gauss npts=8 | 50,000 | 1,176 µs | 23 ns |

**RESULTS: 23 PASS / 0 FAIL / 23 TOTAL**

---

## Windows x64 — Windows 11 / MSVC 14.51 (VS 2026 Build Tools) / float64
**Validator:** Amir Ab Khoshk | **Date:** 2026-06-06 | **Commit:** 1bba399

> **Note:** Gauss-Legendre nodes/weights stored as float32 literals in `src/integrate.c`; promoted to double they carry ~1e-7 weight error. Gauss tests use `TOL_GAUSS = 1e-6` accordingly.

> Build note: Some Unity assertions use float32 tolerances even when built with
> NUMX_USE_DOUBLE. Affected tests still pass because the errors are well within
> float32 tolerance, but the assertion threshold does not tighten to double precision.
> This is a test harness configuration issue, not a library bug.

### Test cases

| Test | Result |
|------|--------|
| test_trap_constant_one | ✅ |
| test_trap_linear | ✅ |
| test_trap_quadratic | ✅ |
| test_trap_linearity | ✅ |
| test_trap_n1 | ✅ |
| test_trap_null | ✅ |
| test_trap_a_ge_b | ✅ |
| test_trap_n_zero | ✅ |
| test_simpson_constant_one | ✅ |
| test_simpson_linear_exact | ✅ |
| test_simpson_quadratic_exact | ✅ |
| test_simpson_cubic_exact | ✅ |
| test_simpson_wider_interval | ✅ |
| test_simpson_odd_n_rejected | ✅ |
| test_simpson_n_lt_2_rejected | ✅ |
| test_simpson_null | ✅ |
| test_gauss2_linear | ✅ |
| test_gauss4_quadratic_exact | ✅ |
| test_gauss8_cubic_exact | ✅ |
| test_gauss8_constant_exact | ✅ |
| test_gauss_null | ✅ |
| test_gauss_invalid_npts | ✅ |
| test_gauss_a_ge_b | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| integrate_trap n=100 | 50,000 | 32,848 µs | 656 ns |
| integrate_trap n=1000 | 10,000 | 67,912 µs | 6,791 ns |
| integrate_simpson n=100 | 50,000 | 35,513 µs | 710 ns |
| integrate_gauss npts=2 | 50,000 | 271 µs | 5 ns |
| integrate_gauss npts=4 | 50,000 | 440 µs | 8 ns |
| integrate_gauss npts=8 | 50,000 | 783 µs | 15 ns |

**RESULTS: 23 PASS / 0 FAIL / 23 TOTAL**

---

## ARM64 — macOS 26.2 / Apple M1 Pro / Apple clang 17.0.0 / float32
**Validator:** Erfan Jazeb Nikoo | **Date:** 2026-06-08 | **Commit:** 1380ab1

### Test cases

| Function | Expected | Computed | Error | Pass |
|----------|----------|----------|-------|------|
| trap n=100 | 1.2500250 | 1.2500250340 | 0.00e+00 | ✅ NOTE: O(h²) |
| trap n=1000 | 1.2500003 | 1.25000095 | 5.96e-07 | ✅ |
| simpson n=100 | 1.25 | 1.25000012 | 1.19e-07 | ✅ |
| gauss npts=2 | 1.25 | 1.25000000 | 0.00e+00 | ✅ |
| gauss npts=4 | 1.25 | 1.25000000 | 0.00e+00 | ✅ |
| gauss npts=8 | 1.25 | 1.25000000 | 0.00e+00 | ✅ |

*300 / 300 Unity tests PASS*

### Performance (f(x)=x³+1 on [0,1])

| Function | n | N | Total | Per call |
|----------|---|---|-------|----------|
| integrate_trap | 100 | 50,000 | 5,595 µs | 111 ns |
| integrate_simpson | 100 | 50,000 | 5,113 µs | 102 ns |
| integrate_gauss npts=2 | — | 50,000 | 217 µs | 4 ns |

### Precision vs numpy reference

| Function | numpy | numx | Error |
|----------|-------|------|-------|
| trap n=100 | 1.2500250 | 1.2500250 | 0.00e+00 |
| trap n=1000 | 1.2500003 | 1.25000095 | 5.96e-07 |
| simpson n=100 | 1.2500000 | 1.25000012 | 1.19e-07 |
| gauss npts=2,4,8 | 1.2500000 | 1.25000000 | 0.00e+00 |
