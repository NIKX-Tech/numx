# Validation: numx_integrate (trap / simpson / gauss)

---

## x86-64 — Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-25 | **Commit:** d81b386

### Test cases

| Function | n | Expected | Computed | Pass |
|----------|---|----------|----------|------|
| trap constant f=1 on [0,1] | — | 1.0 | 1.00000000 | ✅ |
| trap linear | — | exact | exact | ✅ |
| simpson linear | — | exact | exact | ✅ |
| simpson cubic | — | exact | exact | ✅ |
| gauss-2 linear | — | exact | exact | ✅ |
| gauss-4 quadratic | — | exact | exact | ✅ |
| gauss-8 degree-15 | — | exact | exact | ✅ |
| odd n for simpson → ERR | — | -2 | -2 | ✅ |

*All 24 Unity tests: PASS (test_integrate.c:232–256)*

### Performance

| Function | n | N | Total | Per call |
|----------|---|---|-------|----------|
| integrate_trap | 100 | 50,000 | 6,999 µs | 139 ns |
| integrate_trap | 1000 | 5,000 | 8,619 µs | 1,723 ns |
| integrate_simpson | 100 | 50,000 | 9,167 µs | 183 ns |
| integrate_gauss npts=2 | — | 50,000 | 240 µs | 4 ns |
| integrate_gauss npts=4 | — | 50,000 | 310 µs | 6 ns |
| integrate_gauss npts=8 | — | 50,000 | 570 µs | 11 ns |

### Precision vs numpy reference (f(x)=x³+1 on [0,1], exact=1.25)

| Function | numpy ref | numx | Error | Note |
|----------|-----------|------|-------|------|
| trap n=100 | 1.2500250000 | 1.25002503 | 2.50e-05 | ⚠️ O(h²) method error — expected |
| trap n=1000 | 1.2500002500 | 1.25000095 | 9.54e-07 | ✅ |
| simpson n=100 | 1.2500000000 | 1.25000012 | 1.19e-07 | ✅ |
| gauss npts=2 | 1.2500000000 | 1.25000000 | 0.00e+00 | ✅ |
| gauss npts=4 | 1.2500000000 | 1.25000000 | 0.00e+00 | ✅ |
| gauss npts=8 | 1.2500000000 | 1.25000000 | 0.00e+00 | ✅ |

⚠️ **trap n=100 error (2.50e-05)** is the inherent O(h²) truncation of the trapezoidal rule, not a numx bug. numx matches theoretical prediction exactly.

---

## ARM64 — macOS 26.2 / Apple M4 Pro / Apple clang 21.0.0 / float32
**Validator:** Erfan Jazeb Nikoo | **Date:** 2026-05-29 | **Commit:** 37e581f

### Test cases

| Function | Expected | Computed | Error | Pass |
|----------|----------|----------|-------|------|
| trap n=100 | 1.2500250 | 1.2500250340 | 0.00e+00 | ✅ NOTE: O(h²) |
| trap n=1000 | 1.2500003 | 1.25000095 | 5.96e-07 | ✅ |
| simpson n=100 | 1.25 | 1.25000012 | 1.19e-07 | ✅ |
| gauss npts=2 | 1.25 | 1.25000000 | 0.00e+00 | ✅ |
| gauss npts=4 | 1.25 | 1.25000000 | 0.00e+00 | ✅ |
| gauss npts=8 | 1.25 | 1.25000000 | 0.00e+00 | ✅ |

*300 / 300 Unity tests PASS*

### Performance (f(x)=x³+1 on [0,1])

| Function | n | N | Total | Per call |
|----------|---|---|-------|----------|
| integrate_trap | 100 | 50,000 | 5,871 µs | 117 ns |
| integrate_simpson | 100 | 50,000 | 5,231 µs | 104 ns |
| integrate_gauss npts=2 | — | 50,000 | 145 µs | 2 ns |

### Precision vs numpy reference

| Function | numpy | numx | Error |
|----------|-------|------|-------|
| trap n=100 | 1.2500250 | 1.2500250 | 0.00e+00 |
| trap n=1000 | 1.2500003 | 1.25000095 | 5.96e-07 |
| simpson n=100 | 1.2500000 | 1.25000012 | 1.19e-07 |
| gauss npts=2,4,8 | 1.2500000 | 1.25000000 | 0.00e+00 |

---

## ESP32-S3
**Status:** ⚠️ Pending
