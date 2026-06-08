# Validation: numx_lu_decompose + numx_lu_solve

---

## x86-64 — Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-25 | **Commit:** d81b386

### Test cases (A·x = b, 4×4 system)

| Test | Expected x | Computed x | Pass |
|------|-----------|-----------|------|
| 4×4 standard | [1, 0, -1, 1] | [1.00000012, 0, -1.00000012, 1] | ✅ |
| 3×3 textbook | known | known | ✅ |
| identity I·x=b | b | b | ✅ |
| residual ‖Ax−b‖ | ~0 | < 1e-5 | ✅ |
| singular → ERR_SINGULAR | -3 | -3 | ✅ |

*All 8 Unity tests: PASS (test_linalg.c:567–574)*

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| lu_decompose 4×4 | 100,000 | 2,989 µs | 29 ns |
| lu_solve 4×4 (pre-factored) | 100,000 | 1,156 µs | 11 ns |

### Precision vs numpy reference

| Component | numpy | numx | Error |
|-----------|-------|------|-------|
| x[0] | 1.0 | 1.00000012 | 1.19e-07 |
| x[1] | 0.0 | 0.00000000 | 0.00e+00 |
| x[2] | -1.0 | -1.00000012 | 1.19e-07 |
| x[3] | 1.0 | 1.00000000 | 0.00e+00 |

*Errors of 1.19e-07 are within float32 machine epsilon (~1.2e-7). Not a bug.*

---

## ARM64 — macOS 26.2 / Apple M4 Pro / Apple clang 21.0.0 / float32
**Validator:** Erfan Jazeb Nikoo | **Date:** 2026-05-29 | **Commit:** 37e581f

### Test cases (A·x = b, 4×4 system)

| Component | Expected | Computed | Error | Pass |
|-----------|----------|----------|-------|------|
| x[0] | 1.0 | 1.00000000 | 0.00e+00 | ✅ |
| x[1] | 0.0 | 0.00000000 | 0.00e+00 | ✅ |
| x[2] | -1.0 | -1.00000000 | 0.00e+00 | ✅ |
| x[3] | 1.0 | 1.00000000 | 0.00e+00 | ✅ |

*300 / 300 Unity tests PASS*

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| lu_decompose + lu_solve 4×4 (combined) | 100,000 | 14,297 µs | 142 ns |

### Precision vs numpy reference

| Component | numpy | numx | Error |
|-----------|-------|------|-------|
| x[0] | 1.0 | 1.00000000 | 0.00e+00 |
| x[1] | 0.0 | 0.00000000 | 0.00e+00 |
| x[2] | -1.0 | -1.00000000 | 0.00e+00 |
| x[3] | 1.0 | 1.00000000 | 0.00e+00 |

*ARM64 combined benchmark measures both decompose+solve together; individual breakdown pending.*

---

## ESP32-S3 — ESP-IDF v5.5.2 / Xtensa LX7 / xtensa-esp32s3-elf-gcc / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-29 | **Commit:** d81b386

### Test cases (3×3 system A·x = b)

| Test | Component | Expected | Computed | Error | Pass |
|------|-----------|----------|----------|-------|------|
| lu_decompose 3×3 | rc | 0 | 0 | — | ✅ |
| lu_solve 3×3 | rc | 0 | 0 | — | ✅ |
| solution x | x[0] | 2.0 | 2.0000000 | 0.00e+00 | ✅ |
| solution x | x[1] | 3.0 | 2.9999998 | 2.38e-07 | ✅ |
| solution x | x[2] | −1.0 | −0.9999999 | 5.96e-08 | ✅ |
| I·x = b | x[0] | 5.0 | 5.0000000 | 0.00e+00 | ✅ |
| I·x = b | x[1] | 7.0 | 7.0000000 | 0.00e+00 | ✅ |
| residual Ax | [0] | 8.0 | 8.0000000 | 0.00e+00 | ✅ |
| residual Ax | [1] | −11.0 | −11.0000000 | 0.00e+00 | ✅ |
| residual Ax | [2] | −3.0 | −3.0000000 | 0.00e+00 | ✅ |
| singular matrix | rc | rc=-3 | rc=-3 | — | ✅ |
| lu_decompose null-A | rc | rc=-1 | rc=-1 | — | ✅ |
| lu_decompose null-LU | rc | rc=-1 | rc=-1 | — | ✅ |
| lu_solve null-LU | rc | rc=-1 | rc=-1 | — | ✅ |
| lu_solve null-x | rc | rc=-1 | rc=-1 | — | ✅ |

*Errors of 2.38e-07 / 5.96e-08 on x[1] and x[2] are within float32 machine epsilon (~1.2e-7). Not a bug.*

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| lu_decompose 4x4 | 10,000 | 104,666 µs | 10,466 ns |
| lu_solve 4x4 (factored) | 10,000 | 44,181 µs | 4,418 ns |

**RESULTS: 15 PASS / 0 FAIL / 15 TOTAL**

---

## Windows x64 — Windows 11 / MSVC 14.51 (VS 2026 Build Tools) / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-06-05 | **Commit:** 4c4c0f0

### Test cases

| Test | Result |
|------|--------|
| test_lu_solve_3x3_textbook | ✅ |
| test_lu_solve_identity_system | ✅ |
| test_lu_solve_residual_is_zero | ✅ |
| test_lu_decompose_singular | ✅ |
| test_lu_decompose_null_A | ✅ |
| test_lu_decompose_null_LU | ✅ |
| test_lu_solve_null_LU | ✅ |
| test_lu_solve_null_x | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| lu_decompose 4x4 | 100,000 | 11,089 µs | 110 ns |
| lu_solve 4x4 (factored) | 100,000 | 5,508 µs | 55 ns |

**RESULTS: 8 PASS / 0 FAIL / 8 TOTAL**

---

## Windows x64 — Windows 11 / MSVC 14.51 (VS 2026 Build Tools) / float64
**Validator:** Amir Ab Khoshk | **Date:** 2026-06-06 | **Commit:** 1bba399

> Build note: Some Unity assertions use float32 tolerances even when built with
> NUMX_USE_DOUBLE. Affected tests still pass because the errors are well within
> float32 tolerance, but the assertion threshold does not tighten to double precision.
> This is a test harness configuration issue, not a library bug.

### Test cases

| Test | Result |
|------|--------|
| test_lu_solve_3x3_textbook | ✅ |
| test_lu_solve_identity_system | ✅ |
| test_lu_solve_residual_is_zero | ✅ |
| test_lu_decompose_singular | ✅ |
| test_lu_decompose_null_A | ✅ |
| test_lu_decompose_null_LU | ✅ |
| test_lu_solve_null_LU | ✅ |
| test_lu_solve_null_x | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| lu_decompose 4x4 | 100,000 | 11,171 µs | 111 ns |
| lu_solve 4x4 (factored) | 100,000 | 4,310 µs | 43 ns |

**RESULTS: 8 PASS / 0 FAIL / 8 TOTAL**

---

## ARM64 — macOS 26.2 / Apple M1 Pro / Apple clang 17.0.0 / float32
**Validator:** Erfan Jazeb Nikoo | **Date:** 2026-06-08 | **Commit:** 1380ab1

### Test cases (A·x = b, 4×4 system)

| Component | Expected | Computed | Error | Pass |
|-----------|----------|----------|-------|------|
| x[0] | 1.0 | 1.00000000 | 0.00e+00 | ✅ |
| x[1] | 0.0 | 0.00000000 | 0.00e+00 | ✅ |
| x[2] | -1.0 | -1.00000000 | 0.00e+00 | ✅ |
| x[3] | 1.0 | 1.00000000 | 0.00e+00 | ✅ |

*300 / 300 Unity tests PASS*

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| lu_decompose 4×4 (combined with lu_solve) | 100,000 | 8,359 µs | 83 ns |

### Precision vs numpy reference

| Component | numpy | numx | Error |
|-----------|-------|------|-------|
| x[0] | 1.0 | 1.00000000 | 0.00e+00 |
| x[1] | 0.0 | 0.00000000 | 0.00e+00 |
| x[2] | -1.0 | -1.00000000 | 0.00e+00 |
| x[3] | 1.0 | 1.00000000 | 0.00e+00 |
