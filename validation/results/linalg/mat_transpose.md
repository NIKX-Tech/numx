# Validation: numx_mat_transpose + numx_mat_transpose_sq

---

## ESP32-S3 — ESP-IDF v5.5.2 / Xtensa LX7 / xtensa-esp32s3-elf-gcc / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-29 | **Commit:** d81b386

### Test cases — mat_transpose (out-of-place, 2×3 → 3×2)

Input: A = [[1,2,3],[4,5,6]] (row-major: [1,2,3,4,5,6])
Expected Aᵀ (3×2, row-major): [1,4,2,5,3,6]

| Element | Expected | Computed | Error | Pass |
|---------|----------|----------|-------|------|
| AT[0] | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| AT[1] | 4.0 | 4.0000000 | 0.00e+00 | ✅ |
| AT[2] | 2.0 | 2.0000000 | 0.00e+00 | ✅ |
| AT[3] | 5.0 | 5.0000000 | 0.00e+00 | ✅ |
| AT[4] | 3.0 | 3.0000000 | 0.00e+00 | ✅ |
| AT[5] | 6.0 | 6.0000000 | 0.00e+00 | ✅ |
| T(T(A)) = A | identity | confirmed | — | ✅ |
| null ptr | rc=-1 | rc=-1 | — | ✅ |

### Test cases — mat_transpose_sq (in-place, 2×2)

Input: S = [[1,2],[3,4]] (row-major: [1,2,3,4])
Expected Sᵀ (in-place): [1,3,2,4]

| Element | Expected | Computed | Error | Pass |
|---------|----------|----------|-------|------|
| S[0] | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| S[1] | 3.0 | 3.0000000 | 0.00e+00 | ✅ |
| S[2] | 2.0 | 2.0000000 | 0.00e+00 | ✅ |
| S[3] | 4.0 | 4.0000000 | 0.00e+00 | ✅ |
| T(T(S)) = S | identity | confirmed | — | ✅ |
| null ptr | rc=-1 | rc=-1 | — | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| mat_transpose 8x8 | 10,000 | 73,511 µs | 7,351 ns |
| mat_transpose_sq 8x8 | 10,000 | 35,418 µs | 3,541 ns |

**RESULTS: 16 PASS / 0 FAIL / 16 TOTAL**

---

## Windows x64 — Windows 11 / MSVC 14.51 (VS 2026 Build Tools) / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-06-05 | **Commit:** 4c4c0f0

### Test cases

| Test | Result |
|------|--------|
| test_mat_transpose_2x3 | ✅ |
| test_mat_transpose_double_is_identity | ✅ |
| test_mat_transpose_sq_inplace | ✅ |
| test_mat_transpose_sq_twice_is_identity | ✅ |
| test_mat_transpose_null | ✅ |
| test_mat_transpose_sq_null | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| mat_transpose 8x8 | 100,000 | 7,933 µs | 79 ns |
| mat_transpose_sq 8x8 | 100,000 | 6,083 µs | 60 ns |

**RESULTS: 6 PASS / 0 FAIL / 6 TOTAL**

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
| test_mat_transpose_2x3 | ✅ |
| test_mat_transpose_double_is_identity | ✅ |
| test_mat_transpose_sq_inplace | ✅ |
| test_mat_transpose_sq_twice_is_identity | ✅ |
| test_mat_transpose_null | ✅ |
| test_mat_transpose_sq_null | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| mat_transpose 8x8 | 100,000 | 8,409 µs | 84 ns |
| mat_transpose_sq 8x8 | 100,000 | 7,026 µs | 70 ns |

**RESULTS: 6 PASS / 0 FAIL / 6 TOTAL**

---

## ARM64 — macOS 26.2 / Apple M1 Pro / Apple clang 17.0.0 / float32
**Validator:** Erfan Jazeb Nikoo | **Date:** 2026-06-08 | **Commit:** 1380ab1

### Test cases

| Test | Result |
|------|--------|
| test_mat_transpose_2x3 | ✅ |
| test_mat_transpose_double_is_identity | ✅ |
| test_mat_transpose_sq_inplace | ✅ |
| test_mat_transpose_sq_twice_is_identity | ✅ |
| test_mat_transpose_null | ✅ |
| test_mat_transpose_sq_null | ✅ |

*300 / 300 Unity tests PASS*

### Performance

*mat_transpose was not included in Phase 1 val_runner benchmark suite for this run.*

**RESULTS: 6 PASS / 0 FAIL / 6 TOTAL**

---

## ARM64 — macOS 26.2 / Apple M4 Pro / Apple clang 21.0.0 / float32 (Updated)
**Validator:** Erfan Jazeb Nikoo | **Date:** 2026-06-09 | **Commit:** 2fc85d0

### Test cases

| Test | Result |
|------|--------|
| test_mat_transpose_2x3 | ✅ |
| test_mat_transpose_double_is_identity | ✅ |
| test_mat_transpose_sq_inplace | ✅ |
| test_mat_transpose_sq_twice_is_identity | ✅ |
| test_mat_transpose_null | ✅ |
| test_mat_transpose_sq_null | ✅ |

*300 / 300 Unity tests PASS*

### Performance

*mat_transpose was not included in M4 Pro benchmark suite for this run.*

**RESULTS: 6 PASS / 0 FAIL / 6 TOTAL**
