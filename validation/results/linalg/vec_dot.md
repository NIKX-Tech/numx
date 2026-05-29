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

## ESP32-S3 — ESP32-S3-DevKitC-1 / Xtensa LX7 @ 160 MHz / ESP-IDF v5.5.2 / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-25 | **Commit:** d81b386

### Test cases

| Input | Expected | Computed | Error | Pass |
|-------|----------|----------|-------|------|
| [1,2,3]·[4,5,6] | 32.0 | 32.0000000 | 0.00e+00 | ✅ |
| [1,0,0]·[0,1,0] (orthogonal) | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| a·b == b·a (commutative) | true | true | — | ✅ |
| [3,4]·[3,4] (self) | 25.0 | 25.0000000 | 0.00e+00 | ✅ |
| [0,0,0]·[4,5,6] | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| NULL / n=0 checks | errors | errors | — | ✅ |

*11 / 11 Unity tests PASS*

### Performance

*Run `run_benchmarks()` on device to collect.*

### Precision vs numpy reference

| Input | numpy (float32) | numx | Error |
|-------|----------------|------|-------|
| [1,2,3,4]·[5,6,7,8] | 70.0 | 70.0 | 0.00e+00 |
| [1,0,0]·[0,1,0] | 0.0 | 0.0 | 0.00e+00 |
