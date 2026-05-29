# Validation: numx_poly (poly_eval / poly_roots)

---

## x86-64 — Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-25 | **Commit:** d81b386

### Test cases (p(x) = x³-6x²+11x-6, roots: [1, 2, 3])

| Function | Input | Expected | Computed | Pass |
|----------|-------|----------|----------|------|
| poly_eval | x=1 | 0.0 | 0.0 | ✅ |
| poly_eval | x=2 | 0.0 | 0.0 | ✅ |
| poly_eval | x=3 | 0.0 | 0.0 | ✅ |
| poly_eval | x=2.5 | -0.375 | -0.37500000 | ✅ |
| poly_eval | x=0 | -6.0 | -6.00000000 | ✅ |
| poly_roots | — | [1,2,3] | [1,2,3]±ε | ✅ |
| degree-0 → ERR | — | -2 | -2 | ✅ |

*All 17 Unity tests: PASS (test_poly.c:229–245)*

### Performance

| Function | degree | N | Total | Per call |
|----------|--------|---|-------|----------|
| poly_eval | 3 | 100,000 | 132 µs | 1 ns |
| poly_eval | 8 | 100,000 | 211 µs | 2 ns |
| poly_roots | 3 | 1,000 | 104 µs | 104 ns |

### Precision vs numpy reference (p(x)=x³-6x²+11x-6)

| x | numpy | numx | Error |
|---|-------|------|-------|
| 1.0 | 0.0 | 0.00000000 | 0.00e+00 |
| 2.0 | 0.0 | 0.00000000 | 0.00e+00 |
| 3.0 | 0.0 | 0.00000000 | 0.00e+00 |
| 2.5 | -0.375 | -0.37500000 | 0.00e+00 |
| 0.0 | -6.0 | -6.00000000 | 0.00e+00 |

| Root | numpy | numx | Error |
|------|-------|------|-------|
| r[0] | 1.0 | 1.000000 | 0.00e+00 |
| r[1] | 2.0 | 2.000001 | 1.00e-06 |
| r[2] | 3.0 | 2.999999 | 1.00e-06 |

---

## ARM64 — macOS 26.2 / Apple M4 Pro / Apple clang 21.0.0 / float32
**Validator:** Erfan Jazeb Nikoo | **Date:** 2026-05-29 | **Commit:** 37e581f

### Test cases

| Function | Input | Expected | Computed | Error | Pass |
|----------|-------|----------|----------|-------|------|
| poly_eval | x=1 | 0.0 | 0.00000000 | 0.00e+00 | ✅ |
| poly_eval | x=2 | 0.0 | 0.00000000 | 0.00e+00 | ✅ |
| poly_eval | x=3 | 0.0 | 0.00000000 | 0.00e+00 | ✅ |
| poly_eval | x=2.5 | -0.375 | -0.37500000 | 0.00e+00 | ✅ |
| poly_eval | x=0 | -6.0 | -6.00000000 | 0.00e+00 | ✅ |
| poly_eval degree-8 | x=1.5 | 12.44140625 | 12.44140625 | 0.00e+00 | ✅ |
| poly_roots | [1,2,3] | r[0]=1.0 | 0.99999976 | 2.38e-07 | ✅ |
| poly_roots | [1,2,3] | r[1]=2.0 | 2.00000119 | 1.19e-06 | ✅ |
| poly_roots | [1,2,3] | r[2]=3.0 | 2.99999881 | 1.19e-06 | ✅ |

*300 / 300 Unity tests PASS*

### Performance

| Function | degree | N | Total | Per call |
|----------|--------|---|-------|----------|
| poly_eval | 3 | 100,000 | 182 µs | 1 ns |
| poly_roots | 3 | 1,000 | 41 µs | 41 ns |

### Precision vs numpy reference

| x | numpy | numx | Error |
|---|-------|------|-------|
| 1.0 | 0.0 | 0.00000000 | 0.00e+00 |
| 2.5 | -0.375 | -0.37500000 | 0.00e+00 |
| p8(1.5) | 12.44140625 | 12.44140625 | 0.00e+00 |

---

## ESP32-S3
**Status:** ⚠️ Pending
