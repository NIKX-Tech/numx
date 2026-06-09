# Validation: numx_vec_cross3

---

## x86-64 — Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-25 | **Commit:** d81b386

### Test cases

| Input | Expected | Computed | Pass |
|-------|----------|----------|------|
| [1,0,0]×[0,1,0] | [0,0,1] | [0,0,1] | ✅ |
| [0,1,0]×[1,0,0] (anti-commutative) | [0,0,-1] | [0,0,-1] | ✅ |
| [1,2,3]×[2,4,6] (parallel) | [0,0,0] | [0,0,0] | ✅ |
| alias-safe (result=a) | correct | correct | ✅ |
| [1,2,3]×[4,5,6] | [-3,6,-3] | [-3,6,-3] | ✅ |

*All 5 Unity tests: PASS (test_linalg.c:536–540)*

### Performance

| N | Total | Per call |
|---|-------|----------|
| 100,000 | 125 µs | 1 ns |

### Precision vs numpy reference

| Input | component | numpy | numx | Error |
|-------|-----------|-------|------|-------|
| [1,2,3]×[4,5,6] | [0] | -3.0 | -3.0 | 0.00e+00 |
| [1,2,3]×[4,5,6] | [1] | 6.0 | 6.0 | 0.00e+00 |
| [1,2,3]×[4,5,6] | [2] | -3.0 | -3.0 | 0.00e+00 |

---

## ARM64 — macOS 26.2 / Apple M4 Pro / Apple clang 21.0.0 / float32
**Validator:** Erfan Jazeb Nikoo | **Date:** 2026-05-29 | **Commit:** 37e581f

### Test cases

| Input | Expected | Computed | Pass |
|-------|----------|----------|------|
| [1,0,0]×[0,1,0] | [0,0,1] | [0.000000, 0.000000, 1.000000] | ✅ |
| [1,2,3]×[4,5,6] | [-3,6,-3] | [-3.000000, 6.000000, -3.000000] | ✅ |

*300 / 300 Unity tests PASS*

### Performance

| N | Total | Per call |
|---|-------|----------|
| 100,000 | 265 µs | 2 ns |

### Precision vs numpy reference

| Input | component | numpy | numx | Error |
|-------|-----------|-------|------|-------|
| [1,2,3]×[4,5,6] | [0] | -3.0 | -3.0 | 0.00e+00 |
| [1,2,3]×[4,5,6] | [1] | 6.0 | 6.0 | 0.00e+00 |
| [1,2,3]×[4,5,6] | [2] | -3.0 | -3.0 | 0.00e+00 |

---

## ESP32-S3 — ESP-IDF v5.5.2 / Xtensa LX7 / xtensa-esp32s3-elf-gcc / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-29 | **Commit:** d81b386

### Test cases

| Test case | Component | Expected | Computed | Error | Pass |
|-----------|-----------|----------|----------|-------|------|
| x̂×ŷ = ẑ | [0] | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| x̂×ŷ = ẑ | [1] | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| x̂×ŷ = ẑ | [2] | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| anti-commutative b×a = −(a×b) | [0] | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| anti-commutative b×a = −(a×b) | [1] | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| anti-commutative b×a = −(a×b) | [2] | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| parallel → zero | [0] | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| parallel → zero | [2] | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| alias-safe (out=a) | [0] | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| alias-safe (out=a) | [1] | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| alias-safe (out=a) | [2] | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| null-a | — | rc=-1 | rc=-1 | — | ✅ |
| null-b | — | rc=-1 | rc=-1 | — | ✅ |
| null-c | — | rc=-1 | rc=-1 | — | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| vec_cross3 | 10,000 | 3,324 µs | 332 ns |

**RESULTS: 15 PASS / 0 FAIL / 15 TOTAL**

---

## Windows x64 — Windows 11 / MSVC 14.51 (VS 2026 Build Tools) / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-06-05 | **Commit:** 4c4c0f0

### Test cases

| Test | Result |
|------|--------|
| test_vec_cross3_x_cross_y | ✅ |
| test_vec_cross3_anticommutative | ✅ |
| test_vec_cross3_parallel_is_zero | ✅ |
| test_vec_cross3_alias_safe | ✅ |
| test_vec_cross3_null | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| vec_cross3 | 100,000 | 343 µs | 3 ns |

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
| test_vec_cross3_x_cross_y | ✅ |
| test_vec_cross3_anticommutative | ✅ |
| test_vec_cross3_parallel_is_zero | ✅ |
| test_vec_cross3_alias_safe | ✅ |
| test_vec_cross3_null | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| vec_cross3 | 100,000 | 507 µs | 5 ns |

**RESULTS: 5 PASS / 0 FAIL / 5 TOTAL**

---

## ARM64 — macOS 26.2 / Apple M1 Pro / Apple clang 17.0.0 / float32
**Validator:** Erfan Jazeb Nikoo | **Date:** 2026-06-08 | **Commit:** 1380ab1

### Test cases

| Input | Expected | Computed | Pass |
|-------|----------|----------|------|
| [1,0,0]×[0,1,0] | [0,0,1] | [0.000000, 0.000000, 1.000000] | ✅ |
| [1,2,3]×[4,5,6] | [-3,6,-3] | [-3.000000, 6.000000, -3.000000] | ✅ |

*300 / 300 Unity tests PASS*

### Performance

| N | Total | Per call |
|---|-------|----------|
| 100,000 | 131 µs | 1 ns |

### Precision vs numpy reference

| Input | component | numpy | numx | Error |
|-------|-----------|-------|------|-------|
| [1,2,3]×[4,5,6] | [0] | -3.0 | -3.0 | 0.00e+00 |
| [1,2,3]×[4,5,6] | [1] | 6.0 | 6.0 | 0.00e+00 |
| [1,2,3]×[4,5,6] | [2] | -3.0 | -3.0 | 0.00e+00 |

---

## ARM64 — macOS 26.2 / Apple M4 Pro / Apple clang 21.0.0 / float32 (Updated)
**Validator:** Erfan Jazeb Nikoo | **Date:** 2026-06-09 | **Commit:** 2fc85d0

### Test cases

| Input | Expected | Computed | Pass |
|-------|----------|----------|------|
| [1,0,0]×[0,1,0] | [0,0,1] | [0.000000, 0.000000, 1.000000] | ✅ |
| [1,2,3]×[4,5,6] | [-3,6,-3] | [-3.000000, 6.000000, -3.000000] | ✅ |

*300 / 300 Unity tests PASS*

### Performance

| N | Total | Per call |
|---|-------|----------|
| 100,000 | 265 µs | 2 ns |

### Precision vs numpy reference

| Input | component | numpy | numx | Error |
|-------|-----------|-------|------|-------|
| [1,2,3]×[4,5,6] | [0] | -3.0 | -3.0 | 0.00e+00 |
| [1,2,3]×[4,5,6] | [1] | 6.0 | 6.0 | 0.00e+00 |
| [1,2,3]×[4,5,6] | [2] | -3.0 | -3.0 | 0.00e+00 |
