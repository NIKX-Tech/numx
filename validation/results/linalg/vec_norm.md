# Validation: numx_vec_norm

---

## x86-64 — Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-25 | **Commit:** d81b386

### Test cases

| Input | Norm | Expected | Computed | Error | Pass |
|-------|------|----------|----------|-------|------|
| [3,4] | L2 | 5.0 | 5.00000000 | 0.00e+00 | ✅ |
| [3,4] | L1 | 7.0 | 7.00000000 | 0.00e+00 | ✅ |
| [3,4] | Linf | 4.0 | 4.00000000 | 0.00e+00 | ✅ |
| NULL / n=0 | — | errors | errors | — | ✅ |

*All 8 Unity tests: PASS (test_linalg.c:526–533)*

### Performance

| Variant | N | Total | Per call |
|---------|---|-------|----------|
| L2 n=64 | 100,000 | 4,373 µs | 43 ns |
| L1 n=64 | 100,000 | 3,743 µs | 37 ns |

### Precision vs numpy reference

| Input | Norm | numpy | numx | Error |
|-------|------|-------|------|-------|
| [3,4] | L2 | 5.0 | 5.0 | 0.00e+00 |
| [3,4] | L1 | 7.0 | 7.0 | 0.00e+00 |
| [3,4] | Linf | 4.0 | 4.0 | 0.00e+00 |

*L2 uses internal Newton-Raphson sqrt — no libm dependency.*

---

## ARM64 — macOS 26.2 / Apple M4 Pro / Apple clang 21.0.0 / float32
**Validator:** Erfan Jazeb Nikoo | **Date:** 2026-05-29 | **Commit:** 37e581f

### Test cases

| Input | Norm | Expected | Computed | Error | Pass |
|-------|------|----------|----------|-------|------|
| [3,4] | L2 | 5.0 | 5.00000000 | 0.00e+00 | ✅ |
| [3,4] | L1 | 7.0 | 7.00000000 | 0.00e+00 | ✅ |
| [3,4] | Linf | 4.0 | 4.00000000 | 0.00e+00 | ✅ |
| [1,2,3,4,5] | L2 | sqrt(55)=7.41619849 | 7.41619873 | 4.77e-07 | ✅ |

*300 / 300 Unity tests PASS*

### Performance

| Variant | N | Total | Per call |
|---------|---|-------|----------|
| L2 n=2 | 100,000 | 1,283 µs | 12 ns |

### Precision vs numpy reference

| Input | Norm | numpy | numx | Error |
|-------|------|-------|------|-------|
| [3,4] | L2 | 5.0 | 5.00000000 | 0.00e+00 |
| [3,4] | L1 | 7.0 | 7.00000000 | 0.00e+00 |
| [3,4] | Linf | 4.0 | 4.00000000 | 0.00e+00 |

---

## ESP32-S3 — ESP-IDF v5.5.2 / Xtensa LX7 / xtensa-esp32s3-elf-gcc / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-29 | **Commit:** d81b386

### Test cases

| Test case | Norm | Expected | Computed | Error | Pass |
|-----------|------|----------|----------|-------|------|
| [3,4] = 5 | L2 | 5.0 | 5.0000000 | 0.00e+00 | ✅ |
| [3,−4,0] = 7 | L1 | 7.0 | 7.0000000 | 0.00e+00 | ✅ |
| [3,−5,4] = 5 | Linf | 5.0 | 5.0000000 | 0.00e+00 | ✅ |
| unit vector = 1 | L2 | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| zero vector = 0 | L2 | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| null-a | — | rc=-1 | rc=-1 | — | ✅ |
| null-out | — | rc=-1 | rc=-1 | — | ✅ |
| bad norm type | — | rc=-2 | rc=-2 | — | ✅ |

**RESULTS: 9 PASS / 0 FAIL / 9 TOTAL**
