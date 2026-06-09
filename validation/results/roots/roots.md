# Validation: numx_roots (bisect / newton / brent)

---

## x86-64 — Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-25 | **Commit:** d81b386

### Test cases

| Function | Input | Expected | Computed | Pass |
|----------|-------|----------|----------|------|
| bisect x²-2 on [1,2] tol=1e-6 | sqrt(2) | 1.41421356 | 1.41421413 | ✅ |
| newton x²-2 from x0=1.5 | sqrt(2) | 1.41421356 | 1.41421354 | ✅ |
| brent x²-2 on [1,2] | sqrt(2) | 1.41421356 | 1.41421342 | ✅ |
| brent x³-x-2 on [1,2] | 1.52137970 | 1.52137970 | 1.52137971 | ✅ |
| newton: zero derivative → ERR_SINGULAR | -3 | -3 | ✅ |
| bisect: no bracket → ERR_INVALID_ARG | -2 | -2 | ✅ |

*All 22 Unity tests: PASS (test_roots.c:276–302)*

### Performance

| Function | tol | N | Total | Per call |
|----------|-----|---|-------|----------|
| root_bisect x²-2 | 1e-6 | 10,000 | 1,006 µs | 100 ns |
| root_newton x²-2 | 1e-6 | 10,000 | 100 µs | 10 ns |
| root_brent x²-2 | 1e-6 | 10,000 | 1,031 µs | 103 ns |

### Precision vs reference (double)

| Function | double ref | numx (float32) | Error |
|----------|-----------|----------------|-------|
| bisect x²-2 | 1.414213562 | 1.41421413 | 5.74e-07 |
| newton x²-2 | 1.414213562 | 1.41421354 | 2.18e-08 |
| brent x²-2 | 1.414213562 | 1.41421342 | 1.41e-07 |
| brent x³-x-2 | 1.521379707 | 1.52137971 | 9.24e-09 |

*All errors within float32 machine epsilon (~1.2e-07).*

---

## ARM64 — macOS 26.2 / Apple M4 Pro / Apple clang 21.0.0 / float32
**Validator:** Erfan Jazeb Nikoo | **Date:** 2026-05-29 | **Commit:** 37e581f

### Test cases

| Function | Expected | Computed | Error | Pass |
|----------|----------|----------|-------|------|
| bisect x²-2 on [1,2] | sqrt(2)=1.41421354 | 1.41421413 | 5.96e-07 | ✅ |
| newton x²-2 x0=1.5 | sqrt(2)=1.41421354 | 1.41421354 | 0.00e+00 | ✅ |
| brent x²-2 on [1,2] | sqrt(2)=1.41421354 | 1.41421354 | 0.00e+00 | ✅ |
| brent x³-x-2 on [1,2] | 1.52137971 | 1.52137971 | 0.00e+00 | ✅ |

*300 / 300 Unity tests PASS*

### Performance

| Function | tol | N | Total | Per call |
|----------|-----|---|-------|----------|
| root_bisect x²-2 | 1e-6 | 10,000 | 656 µs | 65 ns |
| root_newton x²-2 | 1e-6 | 10,000 | 104 µs | 10 ns |
| root_brent x²-2 | 1e-6 | 10,000 | 618 µs | 61 ns |

### Precision vs reference (double)

| Function | double ref | numx (float32) | Error |
|----------|-----------|----------------|-------|
| bisect x²-2 | 1.41421356 | 1.41421413 | 5.96e-07 |
| newton x²-2 | 1.41421356 | 1.41421354 | 0.00e+00 |
| brent x²-2 | 1.41421356 | 1.41421354 | 0.00e+00 |
| brent x³-x-2 | 1.52137971 | 1.52137971 | 0.00e+00 |

---

## ESP32-S3 — ESP-IDF v5.5.2 / Xtensa LX7 / xtensa-esp32s3-elf-gcc / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-29 | **Commit:** d81b386

### Test cases — bisect

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| bisect linear rc | rc=0 | rc=0 | — | ✅ |
| bisect linear root=2 | 2.0 | 1.9999981 | 1.91e-06 | ✅ |
| bisect quad+ root=2 | 2.0 | 1.9999981 | 1.91e-06 | ✅ |
| bisect quad− root=−2 | −2.0 | −1.9999981 | 1.91e-06 | ✅ |
| bisect cubic residual | ~0 | confirmed | — | ✅ |
| bisect root@left=2 | 2.0 | 2.0000000 | 0.00e+00 | ✅ |
| bisect root@right=2 | 2.0 | 2.0000000 | 0.00e+00 | ✅ |
| bisect null-f | rc=-1 | rc=-1 | — | ✅ |
| bisect null-out | rc=-1 | rc=-1 | — | ✅ |
| bisect no-bracket | rc=-2 | rc=-2 | — | ✅ |
| bisect tol=0 | rc=-2 | rc=-2 | — | ✅ |

### Test cases — newton

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| newton linear rc | rc=0 | rc=0 | — | ✅ |
| newton linear=2 | 2.0 | 2.0000000 | 0.00e+00 | ✅ |
| newton quad+=2 | 2.0 | 2.0000000 | 0.00e+00 | ✅ |
| newton at-root=2 | 2.0 | 2.0000000 | 0.00e+00 | ✅ |
| newton cubic residual | ~0 | confirmed | — | ✅ |
| newton double-root ok/sing | confirmed | confirmed | — | ✅ |
| newton null-f | rc=-1 | rc=-1 | — | ✅ |
| newton null-df | rc=-1 | rc=-1 | — | ✅ |
| newton tol<0 | rc=-2 | rc=-2 | — | ✅ |

### Test cases — brent

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| brent linear rc | rc=0 | rc=0 | — | ✅ |
| brent linear=2 | 2.0 | 2.0000000 | 0.00e+00 | ✅ |
| brent quad+=2 | 2.0 | 2.0000000 | 0.00e+00 | ✅ |
| brent cubic root=1 | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| brent cubic− residual | ~0 | confirmed | — | ✅ |
| brent null-f | rc=-1 | rc=-1 | — | ✅ |
| brent null-out | rc=-1 | rc=-1 | — | ✅ |
| brent no-bracket | rc=-2 | rc=-2 | — | ✅ |
| brent tol=0 | rc=-2 | rc=-2 | — | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| root_bisect x²-4 tol=1e-5 | 1,000 | 11,819 µs | 11,819 ns |
| root_newton x²-4 tol=1e-5 | 1,000 | 5,039 µs | 5,039 ns |
| root_brent x²-4 tol=1e-5 | 1,000 | 19,858 µs | 19,858 ns |
| root_brent x³-x tol=1e-5 | 1,000 | 18,708 µs | 18,708 ns |

### Precision vs reference

| Function | ref | Computed | Error |
|----------|-----|----------|-------|
| bisect linear root=2 | 2.0 | 1.9999981 | 1.91e-06 |
| bisect quad+ root=2 | 2.0 | 1.9999981 | 1.91e-06 |
| bisect quad− root=−2 | −2.0 | −1.9999981 | 1.91e-06 |
| newton / brent integer roots | exact | exact | 0.00e+00 |

*bisect errors of 1.91e-06 reflect convergence to tolerance; newton and brent converge to exact float32 representation for integer roots.*

**RESULTS: 29 PASS / 0 FAIL / 29 TOTAL**

---

## Windows x64 — Windows 11 / MSVC 14.51 (VS 2026 Build Tools) / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-06-05 | **Commit:** 4c4c0f0

### Test cases

| Test | Result |
|------|--------|
| test_bisect_linear | ✅ |
| test_bisect_quadratic_positive | ✅ |
| test_bisect_quadratic_negative | ✅ |
| test_bisect_residual_near_zero | ✅ |
| test_bisect_root_at_left_endpoint | ✅ |
| test_bisect_root_at_right_endpoint | ✅ |
| test_bisect_null_f | ✅ |
| test_bisect_null_root | ✅ |
| test_bisect_no_bracket | ✅ |
| test_bisect_tol_zero | ✅ |
| test_newton_linear | ✅ |
| test_newton_quadratic | ✅ |
| test_newton_residual_near_zero | ✅ |
| test_newton_already_at_root | ✅ |
| test_newton_null_f | ✅ |
| test_newton_null_df | ✅ |
| test_newton_zero_derivative | ✅ |
| test_newton_tol_negative | ✅ |
| test_brent_linear | ✅ |
| test_brent_quadratic | ✅ |
| test_brent_cubic_root_at_one | ✅ |
| test_brent_residual_near_zero | ✅ |
| test_brent_null | ✅ |
| test_brent_no_bracket | ✅ |
| test_brent_tol_zero | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| root_bisect x^2-4 tol=1e-5 | 10,000 | 1,212 µs | 121 ns |
| root_newton x^2-4 tol=1e-5 | 10,000 | 493 µs | 49 ns |
| root_brent x^2-4 tol=1e-5 | 10,000 | 1,255 µs | 125 ns |
| root_brent x^3-x tol=1e-5 | 10,000 | 1,237 µs | 123 ns |

**RESULTS: 25 PASS / 0 FAIL / 25 TOTAL**

---

## Windows x64 — Windows 11 / MSVC 14.51 (VS 2026 Build Tools) / float64
**Validator:** Amir Ab Khoshk | **Date:** 2026-06-06 | **Commit:** 1bba399

### Test cases

| Test | Result |
|------|--------|
| test_bisect_linear | ✅ |
| test_bisect_quadratic_positive | ✅ |
| test_bisect_quadratic_negative | ✅ |
| test_bisect_residual_near_zero | ✅ |
| test_bisect_root_at_left_endpoint | ✅ |
| test_bisect_root_at_right_endpoint | ✅ |
| test_bisect_null_f | ✅ |
| test_bisect_null_root | ✅ |
| test_bisect_no_bracket | ✅ |
| test_bisect_tol_zero | ✅ |
| test_newton_linear | ✅ |
| test_newton_quadratic | ✅ |
| test_newton_residual_near_zero | ✅ |
| test_newton_already_at_root | ✅ |
| test_newton_null_f | ✅ |
| test_newton_null_df | ✅ |
| test_newton_zero_derivative | ✅ |
| test_newton_tol_negative | ✅ |
| test_brent_linear | ✅ |
| test_brent_quadratic | ✅ |
| test_brent_cubic_root_at_one | ✅ |
| test_brent_residual_near_zero | ✅ |
| test_brent_null | ✅ |
| test_brent_no_bracket | ✅ |
| test_brent_tol_zero | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| root_bisect x^2-4 tol=1e-5 | 10,000 | 750 µs | 75 ns |
| root_newton x^2-4 tol=1e-5 | 10,000 | 334 µs | 33 ns |
| root_brent x^2-4 tol=1e-5 | 10,000 | 1,021 µs | 102 ns |
| root_brent x^3-x tol=1e-5 | 10,000 | 1,159 µs | 115 ns |

**RESULTS: 25 PASS / 0 FAIL / 25 TOTAL**

---

## ARM64 — macOS 26.2 / Apple M1 Pro / Apple clang 17.0.0 / float32
**Validator:** Erfan Jazeb Nikoo | **Date:** 2026-06-08 | **Commit:** 1380ab1

### Test cases

| Function | Expected | Computed | Error | Pass |
|----------|----------|----------|-------|------|
| bisect x²-2 on [1,2] | sqrt(2)=1.41421354 | 1.41421413 | 5.96e-07 | ✅ |
| newton x²-2 x0=1.5 | sqrt(2)=1.41421354 | 1.41421354 | 0.00e+00 | ✅ |
| brent x²-2 on [1,2] | sqrt(2)=1.41421354 | 1.41421354 | 0.00e+00 | ✅ |
| brent x³-x-2 on [1,2] | 1.52137971 | 1.52137971 | 0.00e+00 | ✅ |

*300 / 300 Unity tests PASS*

### Performance

| Function | tol | N | Total | Per call |
|----------|-----|---|-------|----------|
| root_bisect x²-2 | 1e-6 | 10,000 | 1,083 µs | 108 ns |
| root_newton x²-2 | 1e-6 | 10,000 | 93 µs | 9 ns |
| root_brent x²-2 | 1e-6 | 10,000 | 706 µs | 70 ns |

### Precision vs reference (double)

| Function | double ref | numx (float32) | Error |
|----------|-----------|----------------|-------|
| bisect x²-2 | 1.41421356 | 1.41421413 | 5.96e-07 |
| newton x²-2 | 1.41421356 | 1.41421354 | 0.00e+00 |
| brent x²-2 | 1.41421356 | 1.41421354 | 0.00e+00 |
| brent x³-x-2 | 1.52137971 | 1.52137971 | 0.00e+00 |

---

## ARM64 — macOS 26.2 / Apple M4 Pro / Apple clang 21.0.0 / float32 (Updated)
**Validator:** Erfan Jazeb Nikoo | **Date:** 2026-06-09 | **Commit:** 2fc85d0

### Test cases

*300 / 300 Unity tests PASS*

### Performance

*roots was not included in M4 Pro benchmark suite for this run.*
