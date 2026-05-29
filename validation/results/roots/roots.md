# Validation: numx_roots (bisect / newton / brent)

---

## x86-64 — Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-25 | **Commit:** d81b386

### Test cases

| Function | Input | Expected | Computed | Pass |
|----------|-------|----------|----------|------|
| bisect x²-2 on [1,2] tol=1e-6 | sqrt(2) | 1.41421356 | 1.41421413 | ✅ |
| newton x²-2 from x0=1.5 | sqrt(2) | 1.41421356 | 1.41421354 | ✅ |
| brent x²-2 on [1,2] | sqrt(2) | 1.41421356 | 1.41421342 | ✅ |
| brent x³-x-2 on [1,2] | 1.52137970 | 1.52137970 | 1.52137971 | ✅ |
| newton: zero derivative → ERR_SINGULAR | -3 | -3 | ✅ |
| bisect: no bracket → ERR_INVALID_ARG | -2 | -2 | ✅ |

*All 22 Unity tests: PASS (test_roots.c:276–302)*

### Performance

| Function | tol | N | Total | Per call |
|----------|-----|---|-------|----------|
| root_bisect x²-2 | 1e-6 | 10,000 | 1,006 µs | 100 ns |
| root_newton x²-2 | 1e-6 | 10,000 | 100 µs | 10 ns |
| root_brent x²-2 | 1e-6 | 10,000 | 1,031 µs | 103 ns |

### Precision vs reference (double)

| Function | double ref | numx (float32) | Error |
|----------|-----------|----------------|-------|
| bisect x²-2 | 1.414213562 | 1.41421413 | 5.74e-07 |
| newton x²-2 | 1.414213562 | 1.41421354 | 2.18e-08 |
| brent x²-2 | 1.414213562 | 1.41421342 | 1.41e-07 |
| brent x³-x-2 | 1.521379707 | 1.52137971 | 9.24e-09 |

*All errors within float32 machine epsilon (~1.2e-07).*

---

## ARM64 — macOS 26.2 / Apple M4 Pro / Apple clang 21.0.0 / float32
**Validator:** Erfan Jazeb Nikoo | **Date:** 2026-05-29 | **Commit:** 37e581f

### Test cases

| Function | Expected | Computed | Error | Pass |
|----------|----------|----------|-------|------|
| bisect x²-2 on [1,2] | sqrt(2)=1.41421354 | 1.41421413 | 5.96e-07 | ✅ |
| newton x²-2 x0=1.5 | sqrt(2)=1.41421354 | 1.41421354 | 0.00e+00 | ✅ |
| brent x²-2 on [1,2] | sqrt(2)=1.41421354 | 1.41421354 | 0.00e+00 | ✅ |
| brent x³-x-2 on [1,2] | 1.52137971 | 1.52137971 | 0.00e+00 | ✅ |

*300 / 300 Unity tests PASS*

### Performance

| Function | tol | N | Total | Per call |
|----------|-----|---|-------|----------|
| root_bisect x²-2 | 1e-6 | 10,000 | 656 µs | 65 ns |
| root_newton x²-2 | 1e-6 | 10,000 | 104 µs | 10 ns |
| root_brent x²-2 | 1e-6 | 10,000 | 618 µs | 61 ns |

### Precision vs reference (double)

| Function | double ref | numx (float32) | Error |
|----------|-----------|----------------|-------|
| bisect x²-2 | 1.41421356 | 1.41421413 | 5.96e-07 |
| newton x²-2 | 1.41421356 | 1.41421354 | 0.00e+00 |
| brent x²-2 | 1.41421356 | 1.41421354 | 0.00e+00 |
| brent x³-x-2 | 1.52137971 | 1.52137971 | 0.00e+00 |

---

## ESP32-S3
**Status:** ⚠️ Pending
