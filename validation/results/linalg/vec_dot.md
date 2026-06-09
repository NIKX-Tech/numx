# Validation: numx_vec_dot

---

## x86-64 — Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-25 | **Commit:** d81b386

### Test cases

| Input | Expected | Computed | Error | Pass |
|-------|----------|----------|-------|------|
| [1,2,3,4]·[5,6,7,8] | 70.0 | 70.00000000 | 0.00e+00 | ✅ |
| [1,0,0]·[0,1,0] (orthogonal) | 0.0 | 0.00000000 | 0.00e+00 | ✅ |
| a=NULL → ERR_NULL_PTR | -1 | -1 | — | ✅ |
| b=NULL → ERR_NULL_PTR | -1 | -1 | — | ✅ |
| n=0 → ERR_INVALID_ARG | -2 | -2 | — | ✅ |

*All 10 Unity tests: PASS (test_linalg.c:514–523)*

### Performance

| N | Total | Per call |
|---|-------|----------|
| 100,000 | 1,542 µs | 15 ns |

### Precision vs numpy reference

| Input | numpy (float32) | numx | Error |
|-------|----------------|------|-------|
| [1,2,3,4]·[5,6,7,8] | 70.0 | 70.0 | 0.00e+00 |
| [1,0,0]·[0,1,0] | 0.0 | 0.0 | 0.00e+00 |

---

## ARM64 — macOS 26.2 / Apple M4 Pro / Apple clang 21.0.0 / float32
**Validator:** Erfan Jazeb Nikoo | **Date:** 2026-05-29 | **Commit:** 37e581f

### Test cases

| Input | Expected | Computed | Error | Pass |
|-------|----------|----------|-------|------|
| [1,2,3,4]·[5,6,7,8] | 70.0 | 70.00000000 | 0.00e+00 | ✅ |
| [1,0,0]·[0,1,0] (orthogonal) | 0.0 | 0.00000000 | 0.00e+00 | ✅ |

*300 / 300 Unity tests PASS*

### Performance

| N | Total | Per call |
|---|-------|----------|
| 100,000 | 350 µs | 3 ns |

### Precision vs numpy reference

| Input | numpy (float32) | numx | Error |
|-------|----------------|------|-------|
| [1,2,3,4]·[5,6,7,8] | 70.0 | 70.00000000 | 0.00e+00 |
| [1,0,0]·[0,1,0] | 0.0 | 0.00000000 | 0.00e+00 |

---

## ESP32-S3 — ESP-IDF v5.5.2 / Xtensa LX7 / xtensa-esp32s3-elf-gcc / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-29 | **Commit:** d81b386

### Test cases

| Test case | Expected | Computed | Error | Pass |
|-----------|----------|----------|-------|------|
| [1,2,3]·[4,5,6] = 32 | 32.0 | 32.0000000 | 0.00e+00 | ✅ |
| orthogonal = 0 | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| commutative a·b = b·a | equal | confirmed | — | ✅ |
| self [3,4]·[3,4] = 25 | 25.0 | 25.0000000 | 0.00e+00 | ✅ |
| n=1: 7×3 = 21 | 21.0 | 21.0000000 | 0.00e+00 | ✅ |
| zero vector = 0 | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| null-a | rc=-1 | rc=-1 | — | ✅ |
| null-b | rc=-1 | rc=-1 | — | ✅ |
| null-out | rc=-1 | rc=-1 | — | ✅ |
| n=0 | rc=-2 | rc=-2 | — | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| vec_dot n=64 | 10,000 | 54,560 µs | 5,456 ns |

**RESULTS: 11 PASS / 0 FAIL / 11 TOTAL**

---

## Windows x64 — Windows 11 / MSVC 14.51 (VS 2026 Build Tools) / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-06-05 | **Commit:** 4c4c0f0

### Test cases

| Test | Result |
|------|--------|
| test_vec_dot_known | ✅ |
| test_vec_dot_orthogonal | ✅ |
| test_vec_dot_commutative | ✅ |
| test_vec_dot_self_equals_sq_norm | ✅ |
| test_vec_dot_single_element | ✅ |
| test_vec_dot_zero_vector | ✅ |
| test_vec_dot_null_a | ✅ |
| test_vec_dot_null_b | ✅ |
| test_vec_dot_null_result | ✅ |
| test_vec_dot_zero_n | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| vec_dot n=64 | 100,000 | 9,127 µs | 91 ns |

**RESULTS: 10 PASS / 0 FAIL / 10 TOTAL**

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
| test_vec_dot_known | ✅ |
| test_vec_dot_orthogonal | ✅ |
| test_vec_dot_commutative | ✅ |
| test_vec_dot_self_equals_sq_norm | ✅ |
| test_vec_dot_single_element | ✅ |
| test_vec_dot_zero_vector | ✅ |
| test_vec_dot_null_a | ✅ |
| test_vec_dot_null_b | ✅ |
| test_vec_dot_null_result | ✅ |
| test_vec_dot_zero_n | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| vec_dot n=64 | 100,000 | 9,441 µs | 94 ns |

**RESULTS: 10 PASS / 0 FAIL / 10 TOTAL**

---

## ARM64 — macOS 26.2 / Apple M1 Pro / Apple clang 17.0.0 / float32
**Validator:** Erfan Jazeb Nikoo | **Date:** 2026-06-08 | **Commit:** 1380ab1

### Test cases

| Input | Expected | Computed | Error | Pass |
|-------|----------|----------|-------|------|
| [1,2,3,4]·[5,6,7,8] | 70.0 | 70.00000000 | 0.00e+00 | ✅ |
| [1,0,0]·[0,1,0] (orthogonal) | 0.0 | 0.00000000 | 0.00e+00 | ✅ |

*300 / 300 Unity tests PASS*

### Performance

| N | Total | Per call |
|---|-------|----------|
| 100,000 | 299 µs | 2 ns |

### Precision vs numpy reference

| Input | numpy (float32) | numx | Error |
|-------|----------------|------|-------|
| [1,2,3,4]·[5,6,7,8] | 70.0 | 70.00000000 | 0.00e+00 |
| [1,0,0]·[0,1,0] | 0.0 | 0.00000000 | 0.00e+00 |

---

## ARM64 — macOS 26.2 / Apple M4 Pro / Apple clang 21.0.0 / float32 (Updated)
**Validator:** Erfan Jazeb Nikoo | **Date:** 2026-06-09 | **Commit:** 2fc85d0

### Test cases

| Input | Expected | Computed | Error | Pass |
|-------|----------|----------|-------|------|
| [1,2,3,4]·[5,6,7,8] | 70.0 | 70.00000000 | 0.00e+00 | ✅ |
| [1,0,0]·[0,1,0] (orthogonal) | 0.0 | 0.00000000 | 0.00e+00 | ✅ |

*300 / 300 Unity tests PASS*

### Performance

| N | Total | Per call |
|---|-------|----------|
| 100,000 | 350 µs | 3 ns |

### Precision vs numpy reference

| Input | numpy (float32) | numx | Error |
|-------|----------------|------|-------|
| [1,2,3,4]·[5,6,7,8] | 70.0 | 70.00000000 | 0.00e+00 |
| [1,0,0]·[0,1,0] | 0.0 | 0.00000000 | 0.00e+00 |
