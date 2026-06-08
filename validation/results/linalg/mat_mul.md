# Validation: numx_mat_mul

---

## x86-64 — Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-25 | **Commit:** d81b386

### Test cases

| Case | Expected | Computed | Pass |
|------|----------|----------|------|
| 2×2: [[1,2],[3,4]] @ [[5,6],[7,8]] | [[19,22],[43,50]] | [[19,22],[43,50]] | ✅ |
| 2×3 @ 3×2 | [[58,64],[139,154]] | [[58,64],[139,154]] | ✅ |
| A × I (identity) | A | A | ✅ |
| dim mismatch (ca≠rb) | ERR_INVALID_ARG | -2 | ✅ |

*All 5 Unity tests: PASS (test_linalg.c:543–547)*

### Performance

| Size | N | Total | Per call |
|------|---|-------|----------|
| 4×4 | 100,000 | 2,615 µs | 26 ns |
| 8×8 | 10,000 | 2,038 µs | 203 ns |

### Precision vs numpy reference

| Case | [i,j] | numpy | numx | Error |
|------|-------|-------|------|-------|
| 2×2 | [0,0] | 19.0 | 19.0 | 0.00e+00 |
| 2×2 | [0,1] | 22.0 | 22.0 | 0.00e+00 |
| 2×2 | [1,0] | 43.0 | 43.0 | 0.00e+00 |
| 2×2 | [1,1] | 50.0 | 50.0 | 0.00e+00 |

---

## ARM64 — macOS 26.2 / Apple M4 Pro / Apple clang 21.0.0 / float32
**Validator:** Erfan Jazeb Nikoo | **Date:** 2026-05-29 | **Commit:** 37e581f

### Test cases

| Case | Expected | Computed | Pass |
|------|----------|----------|------|
| 2×2 A@B | [19,22,43,50] | [19.000000, 22.000000, 43.000000, 50.000000] | ✅ |
| 2×3 @ 3×2 | [58,64,139,154] | [58.000000, 64.000000, 139.000000, 154.000000] | ✅ |

*300 / 300 Unity tests PASS*

### Performance

| Size | N | Total | Per call |
|------|---|-------|----------|
| 2×2 | 100,000 | 937 µs | 9 ns |

### Precision vs numpy reference

| Case | numpy | numx | Error |
|------|-------|------|-------|
| 2×2 all elements | exact integers | exact match | 0.00e+00 |

---

## ESP32-S3 — ESP-IDF v5.5.2 / Xtensa LX7 / xtensa-esp32s3-elf-gcc / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-29 | **Commit:** d81b386

### Test cases

| Test case | Element | Expected | Computed | Error | Pass |
|-----------|---------|----------|----------|-------|------|
| 2×2 A@B | C[0,0] | 19.0 | 19.0000000 | 0.00e+00 | ✅ |
| 2×2 A@B | C[0,1] | 22.0 | 22.0000000 | 0.00e+00 | ✅ |
| 2×2 A@B | C[1,0] | 43.0 | 43.0000000 | 0.00e+00 | ✅ |
| 2×2 A@B | C[1,1] | 50.0 | 50.0000000 | 0.00e+00 | ✅ |
| 2×3 @ 3×2 | C[0,0] | 58.0 | 58.0000000 | 0.00e+00 | ✅ |
| 2×3 @ 3×2 | C[0,1] | 64.0 | 64.0000000 | 0.00e+00 | ✅ |
| 2×3 @ 3×2 | C[1,0] | 139.0 | 139.0000000 | 0.00e+00 | ✅ |
| 2×3 @ 3×2 | C[1,1] | 154.0 | 154.0000000 | 0.00e+00 | ✅ |
| A × I = A | [0] | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| A × I = A | [1] | 2.0 | 2.0000000 | 0.00e+00 | ✅ |
| A × I = A | [2] | 3.0 | 3.0000000 | 0.00e+00 | ✅ |
| A × I = A | [3] | 4.0 | 4.0000000 | 0.00e+00 | ✅ |
| dim mismatch (ca≠rb) | — | rc=-2 | rc=-2 | — | ✅ |
| null-A | — | rc=-1 | rc=-1 | — | ✅ |
| null-C | — | rc=-1 | rc=-1 | — | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| mat_mul 4x4 | 10,000 | 112,629 µs | 11,262 ns |
| mat_mul 8x8 | 100 | 7,881 µs | 78,810 ns |
| mat_transpose 8x8 | 10,000 | 73,511 µs | 7,351 ns |
| mat_transpose_sq 8x8 | 10,000 | 35,418 µs | 3,541 ns |

**RESULTS: 16 PASS / 0 FAIL / 16 TOTAL**

---

## Windows x64 — Windows 11 / MSVC 14.51 (VS 2026 Build Tools) / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-06-05 | **Commit:** 4c4c0f0

### Test cases

| Test | Result |
|------|--------|
| test_mat_mul_2x2 | ✅ |
| test_mat_mul_2x3_times_3x2 | ✅ |
| test_mat_mul_identity | ✅ |
| test_mat_mul_dim_mismatch | ✅ |
| test_mat_mul_null_A | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| mat_mul 4x4 | 100,000 | 9,872 µs | 98 ns |
| mat_mul 8x8 | 10,000 | 7,472 µs | 747 ns |

**RESULTS: 5 PASS / 0 FAIL / 5 TOTAL**

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
| test_mat_mul_2x2 | ✅ |
| test_mat_mul_2x3_times_3x2 | ✅ |
| test_mat_mul_identity | ✅ |
| test_mat_mul_dim_mismatch | ✅ |
| test_mat_mul_null_A | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| mat_mul 4x4 | 100,000 | 11,027 µs | 110 ns |
| mat_mul 8x8 | 10,000 | 7,878 µs | 787 ns |

**RESULTS: 5 PASS / 0 FAIL / 5 TOTAL**

---

## ARM64 — macOS 26.2 / Apple M1 Pro / Apple clang 17.0.0 / float32
**Validator:** Erfan Jazeb Nikoo | **Date:** 2026-06-08 | **Commit:** 1380ab1

### Test cases

| Case | Expected | Computed | Pass |
|------|----------|----------|------|
| 2×2 A@B | [19,22,43,50] | [19.000000, 22.000000, 43.000000, 50.000000] | ✅ |
| 2×3 @ 3×2 | [58,64,139,154] | [58.000000, 64.000000, 139.000000, 154.000000] | ✅ |

*300 / 300 Unity tests PASS*

### Performance

| Size | N | Total | Per call |
|------|---|-------|----------|
| 2×2 | 100,000 | 660 µs | 6 ns |

### Precision vs numpy reference

| Case | numpy | numx | Error |
|------|-------|------|-------|
| 2×2 all elements | exact integers | exact match | 0.00e+00 |
