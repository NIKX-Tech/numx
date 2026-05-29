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

## ESP32-S3 — ESP32-S3-DevKitC-1 / Xtensa LX7 @ 160 MHz / ESP-IDF v5.5.2 / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-25 | **Commit:** d81b386

### Test cases

| Case | Expected | Computed | Pass |
|------|----------|----------|------|
| 2×2: [[1,2],[3,4]] @ [[5,6],[7,8]] | [[19,22],[43,50]] | [[19,22],[43,50]] | ✅ |
| 2×3 @ 3×2: C[0,0]=58, C[1,1]=154 | [58, 154] | [58, 154] | ✅ |
| A × I (identity) | A | A | ✅ |
| dim mismatch (ca≠rb) | ERR_INVALID_ARG | -2 | ✅ |
| mat_transpose 2×3 + square in-place | correct | correct | ✅ |

*24 / 24 Unity tests PASS (12 mat_mul + 12 mat_transpose)*

### Performance

*Run `run_benchmarks()` on device to collect.*

### Precision vs numpy reference

| Case | numpy | numx | Error |
|------|-------|------|-------|
| 2×2 all elements | exact integers | exact match | 0.00e+00 |
