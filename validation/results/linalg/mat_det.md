# Validation: numx_mat_det

---

## x86-64 — Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-25 | **Commit:** d81b386

### Test cases

| n | Input | Expected | Computed | Error | Pass |
|---|-------|----------|----------|-------|------|
| 1 | [[5]] | 5.0 | 5.00000000 | 0.00e+00 | ✅ |
| 2 | [[1,2],[3,4]] | -2.0 | -2.00000000 | 0.00e+00 | ✅ |
| 3 | textbook | 4.0 | 4.00000000 | 0.00e+00 | ✅ |
| 4 | symmetric | 20.0 | 20.00000000 | 0.00e+00 | ✅ |
| 2 | singular | 0.0 | 0.00000000 | 0.00e+00 | ✅ |
| 3 | identity | 1.0 | 1.00000000 | 0.00e+00 | ✅ |

*All 7 Unity tests: PASS (test_linalg.c:558–564)*

### Performance

| Size | N | Total | Per call |
|------|---|-------|----------|
| 4×4 | 100,000 | 376 µs | 3 ns |

### Precision vs numpy reference

| n | numpy | numx | Error |
|---|-------|------|-------|
| 2 | -2.0 | -2.0 | 0.00e+00 |
| 3 | 4.0 | 4.0 | 0.00e+00 |
| 4 | 20.0 | 20.0 | 0.00e+00 |

---

## ARM64 — macOS 26.2 / Apple M4 Pro / Apple clang 21.0.0 / float32
**Validator:** Erfan Jazeb Nikoo | **Date:** 2026-05-29 | **Commit:** 37e581f

### Test cases

| n | Expected | Computed | Error | Pass |
|---|----------|----------|-------|------|
| 1×1 [[5]] | 5.0 | 5.00000000 | 0.00e+00 | ✅ |
| 2×2 [[1,2],[3,4]] | -2.0 | -2.00000000 | 0.00e+00 | ✅ |
| 3×3 textbook | 4.0 | 4.00000000 | 0.00e+00 | ✅ |
| 4×4 symmetric | 20.0 | 20.00000000 | 0.00e+00 | ✅ |

*300 / 300 Unity tests PASS*

### Performance

| Size | N | Total | Per call |
|------|---|-------|----------|
| 4×4 | 100,000 | 796 µs | 7 ns |

### Precision vs numpy reference

| n | numpy | numx | Error |
|---|-------|------|-------|
| 2 | -2.0 | -2.00000000 | 0.00e+00 |
| 3 | 4.0 | 4.00000000 | 0.00e+00 |
| 4 | 20.0 | 20.00000000 | 0.00e+00 |

---

## ESP32-S3 — ESP-IDF v5.5.2 / Xtensa LX7 / xtensa-esp32s3-elf-gcc / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-29 | **Commit:** d81b386

### Test cases

| n | Test case | Expected | Computed | Error | Pass |
|---|-----------|----------|----------|-------|------|
| 1×1 | det = 7 | 7.0 | 7.0000000 | 0.00e+00 | ✅ |
| 2×2 | det = −2 | −2.0 | −2.0000000 | 0.00e+00 | ✅ |
| 3×3 | det = −3 | −3.0 | −3.0000000 | 0.00e+00 | ✅ |
| n×n | identity = 1 | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| 2×2 | singular = 0 | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| — | null-A | rc=-1 | rc=-1 | — | ✅ |
| — | null-out | rc=-1 | rc=-1 | — | ✅ |

**RESULTS: 8 PASS / 0 FAIL / 8 TOTAL**
