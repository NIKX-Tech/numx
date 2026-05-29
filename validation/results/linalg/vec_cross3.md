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

## ESP32-S3 — ESP32-S3-DevKitC-1 / Xtensa LX7 @ 160 MHz / ESP-IDF v5.5.2 / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-25 | **Commit:** d81b386

### Test cases

| Input | Expected | Computed | Pass |
|-------|----------|----------|------|
| [1,0,0]×[0,1,0] | [0,0,1] | [0,0,1] | ✅ |
| [1,2,3]×[4,5,6] anti-commutative (axb+bxa=0) | [0,0,0] | [0,0,0] | ✅ |
| [2,0,0]×[5,0,0] (parallel) | [0,0,0] | [0,0,0] | ✅ |
| NULL checks (a, b, c) | -1 | -1 | ✅ |

*12 / 12 Unity tests PASS*

### Performance

*Run `run_benchmarks()` on device to collect.*

### Precision vs numpy reference

| Input | component | numpy | numx | Error |
|-------|-----------|-------|------|-------|
| [1,2,3]×[4,5,6] | [0] | -3.0 | -3.0 | 0.00e+00 |
| [1,2,3]×[4,5,6] | [1] | 6.0 | 6.0 | 0.00e+00 |
| [1,2,3]×[4,5,6] | [2] | -3.0 | -3.0 | 0.00e+00 |
