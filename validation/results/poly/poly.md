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

## ESP32-S3 — ESP-IDF v5.5.2 / Xtensa LX7 / xtensa-esp32s3-elf-gcc / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-29 | **Commit:** d81b386

### Test cases — poly_eval

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| poly_eval rc | rc=0 | rc=0 | — | ✅ |
| x²+1 at 3 = 10 | 10.0 | 10.0000000 | 0.00e+00 | ✅ |
| x³−2x+1 at 2 = 5 | 5.0 | 5.0000000 | 0.00e+00 | ✅ |
| at 0 = const = −2 | −2.0 | −2.0000000 | 0.00e+00 | ✅ |
| degree-0 = 5 | 5.0 | 5.0000000 | 0.00e+00 | ✅ |
| Horner 2x²+3x+4 at 5 = 69 | 69.0 | 69.0000000 | 0.00e+00 | ✅ |
| (x−1)(x−2)(x−3) at 1 = 0 | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| (x−1)(x−2)(x−3) at 2 = 0 | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| (x−1)(x−2)(x−3) at 3 = 0 | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| x²−3x+2 at 1 = 0 | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| poly_eval null-c | rc=-1 | rc=-1 | — | ✅ |
| poly_eval null-out | rc=-1 | rc=-1 | — | ✅ |

### Test cases — poly_roots

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| poly_roots linear rc | rc=0 | rc=0 | — | ✅ |
| poly_roots linear nroots=1 | 1 | 1 | — | ✅ |
| poly_roots linear root=−1 | −1.0 | −1.0000000 | 0.00e+00 | ✅ |
| poly_roots quad nroots=2 | 2 | 2 | — | ✅ |
| poly_roots quad f(r0)≈0 | 0.0 | 0.0000019 | 1.91e-06 | ✅ |
| poly_roots quad f(r1)≈0 | 0.0 | 0.0000024 | 2.38e-06 | ✅ |
| poly_roots quad root=2 found | 2.0 | confirmed | — | ✅ |
| poly_roots quad root=3 found | 3.0 | confirmed | — | ✅ |
| poly_roots cubic residuals all ≈ 0 | ~0 | confirmed | — | ✅ |
| poly_roots x³−8 has root | confirmed | confirmed | — | ✅ |
| poly_roots x³−8 real root≈2 | 2.0 | ≈2.0 | — | ✅ |
| poly_roots null-c | rc=-1 | rc=-1 | — | ✅ |
| poly_roots null-out | rc=-1 | rc=-1 | — | ✅ |
| poly_roots null-nr | rc=-1 | rc=-1 | — | ✅ |
| poly_roots degree=0 | rc=-2 | rc=-2 | — | ✅ |
| poly_roots tol=0 | rc=-2 | rc=-2 | — | ✅ |

### Precision vs reference

| Function | Input | Expected | Computed | Error |
|----------|-------|----------|----------|-------|
| poly_eval x²+1 | x=3 | 10.0 | 10.0000000 | 0.00e+00 |
| poly_eval Horner | x=5 | 69.0 | 69.0000000 | 0.00e+00 |
| poly_roots quad residual | f(r0) | 0.0 | 1.91e-06 | 1.91e-06 |
| poly_roots quad residual | f(r1) | 0.0 | 2.38e-06 | 2.38e-06 |

*Quadratic root residuals of ~2e-06 are within float32 precision for iterative root finding. Not a bug.*

**RESULTS: 28 PASS / 0 FAIL / 28 TOTAL**
