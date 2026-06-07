# Validation: numx_differentiate (forward / central / richardson)

---

## x86-64 — Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-25 | **Commit:** d81b386

### Test cases

| Function | Input | Expected | Computed | Pass |
|----------|-------|----------|----------|------|
| forward quadratic at 3 | — | 6.0 | ≈6.0 | ✅ |
| central quadratic at 3 | — | 6.0 | ≈6.0 | ✅ |
| richardson more accurate | — | better | better | ✅ |
| constant function → 0 | — | 0.0 | 0.0 | ✅ |
| linear → 1 (central) | — | 1.0 | exact | ✅ |
| h≤0 → ERR_INVALID_ARG | — | -2 | -2 | ✅ |

*All 18 Unity tests: PASS (test_differentiate.c:217–237)*

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| diff_forward | 100,000 | 300 µs | 3 ns |
| diff_central | 100,000 | 261 µs | 2 ns |
| diff_richardson | 100,000 | 430 µs | 4 ns |

### Precision vs reference (f(x)=x³ at x=2, exact f′=12.0, h=1e-4)

| Function | double ref | numx (float32) | Error | Note |
|----------|-----------|----------------|-------|------|
| forward | 12.0006000 | 11.98768616 | 1.23e-02 | ⚠️ float32 cancellation |
| central | 12.0000000 | 11.99483871 | 5.16e-03 | ⚠️ float32 cancellation |
| richardson | 12.0000000 | 12.00437546 | 4.38e-03 | ⚠️ float32 cancellation |

⚠️ **FLAG F-01:** All errors are due to float32 cancellation at h=1e-4, not a code bug. Best achievable accuracy with float32 is ~1e-4 (optimal h≈√ε≈3.46e-4 for forward). Use `NUMX_USE_DOUBLE` if 1e-5 precision is required.

---

## ARM64 — macOS 26.2 / Apple M4 Pro / Apple clang 21.0.0 / float32
**Validator:** Erfan Jazeb Nikoo | **Date:** 2026-05-29 | **Commit:** 37e581f

### Test cases (f(x)=x³ at x=2, exact f′=12.0, h=1e-4)

| Function | Expected | Computed | Error | Pass |
|----------|----------|----------|-------|------|
| forward | 12.0 | 11.98768616 | 1.23e-02 | ✅ NOTE: float32 limit |
| central | 12.0 | 11.99483871 | 5.16e-03 | ✅ NOTE: float32 limit |
| richardson | 12.0 | 12.00437546 | 4.38e-03 | ✅ NOTE: float32 limit |

Identical values to x86-64 — confirms float32 cancellation is deterministic across architectures (IEEE 754).

*300 / 300 Unity tests PASS*

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| diff_forward | 100,000 | 258 µs | 2 ns |
| diff_central | 100,000 | 262 µs | 2 ns |
| diff_richardson | 100,000 | 420 µs | 4 ns |

### Precision — optimal h for float32 (√ε ≈ 3.46e-4)

| Function | h | Computed | Error |
|----------|---|----------|-------|
| forward | 3.46e-4 | 12.00086212 | 8.62e-04 |
| central | 3.46e-4 | 11.99810505 | 1.89e-03 |

---

## ESP32-S3 — ESP-IDF v5.5.2 / Xtensa LX7 / xtensa-esp32s3-elf-gcc / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-29 | **Commit:** d81b386

### Test cases — forward

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| fwd rc | rc=0 | rc=0 | — | ✅ |
| fwd x³ at 3 = 27 | 27.0 | 27.0061474 | 6.15e-03 | ✅ |
| fwd x³ at 2 = 12 | 12.0 | 12.0048513 | 4.85e-03 | ✅ |
| fwd const = 0 | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| fwd linear = 1 | 1.0 | 0.9999275 | 7.25e-05 | ✅ |
| fwd x² at 3 = 6 | 6.0 | 6.0005183 | 5.18e-04 | ✅ |
| fwd null-f | rc=-1 | rc=-1 | — | ✅ |
| fwd null-out | rc=-1 | rc=-1 | — | ✅ |
| fwd h=0 | rc=-2 | rc=-2 | — | ✅ |
| fwd h<0 | rc=-2 | rc=-2 | — | ✅ |

### Test cases — central

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| central x³ at 3 = 27 | 27.0 | 26.9975643 | 2.44e-03 | ✅ |
| central x² at −2 = −4 | −4.0 | −3.9999483 | 5.17e-05 | ✅ |
| central const = 0 | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| central linear = 1 | 1.0 | 1.0000169 | 1.69e-05 | ✅ |
| central x² at 3 = 6 | 6.0 | 5.9995646 | 4.35e-04 | ✅ |
| central x³ at 2 = 12 | 12.0 | 11.9998446 | 1.55e-04 | ✅ |
| central null-f | rc=-1 | rc=-1 | — | ✅ |
| central null-out | rc=-1 | rc=-1 | — | ✅ |
| central h=0 | rc=-2 | rc=-2 | — | ✅ |
| central h<0 | rc=-2 | rc=-2 | — | ✅ |

### Test cases — richardson

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| rich x³ at 3 = 27 | 27.0 | 26.9988346 | 1.17e-03 | ✅ |
| rich x³ at 2 = 12 | 12.0 | 11.9995270 | 4.73e-04 | ✅ |
| rich more accurate than central (large h) | better | confirmed | — | ✅ |
| rich x² at 3 = 6 | 6.0 | 5.9995646 | 4.35e-04 | ✅ |
| rich const = 0 | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| rich null-f | rc=-1 | rc=-1 | — | ✅ |
| rich null-out | rc=-1 | rc=-1 | — | ✅ |
| rich h=0 | rc=-2 | rc=-2 | — | ✅ |
| rich h<0 | rc=-2 | rc=-2 | — | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| diff_forward h=1e-3 | 10,000 | 8,644 µs | 864 ns |
| diff_central h=1e-3 | 10,000 | 9,393 µs | 939 ns |
| diff_richardson h=1e-3 | 10,000 | 21,410 µs | 2,141 ns |

### Precision vs reference

| Function | f | x | Expected f′ | Computed | Error | Note |
|----------|---|---|------------|----------|-------|------|
| forward | x³ | 3 | 27.0 | 27.0061474 | 6.15e-03 | float32 cancellation |
| forward | x³ | 2 | 12.0 | 12.0048513 | 4.85e-03 | float32 cancellation |
| central | x³ | 3 | 27.0 | 26.9975643 | 2.44e-03 | float32 cancellation |
| central | x³ | 2 | 12.0 | 11.9998446 | 1.55e-04 | float32 cancellation |
| richardson | x³ | 3 | 27.0 | 26.9988346 | 1.17e-03 | float32 cancellation |
| richardson | x³ | 2 | 12.0 | 11.9995270 | 4.73e-04 | float32 cancellation |

*Errors are consistent with FLAG F-01 (float32 cancellation at small h). Matches x86-64/ARM64 behaviour — not a bug.*

**RESULTS: 29 PASS / 0 FAIL / 29 TOTAL**

---

## Windows x64 — Windows 11 / MSVC 14.51 (VS 2026 Build Tools) / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-06-05 | **Commit:** 4c4c0f0

### Test cases

| Test | Result |
|------|--------|
| test_fwd_quadratic_at_3 | ✅ |
| test_fwd_cubic_at_2 | ✅ |
| test_fwd_constant_is_zero | ✅ |
| test_fwd_linear_is_one | ✅ |
| test_fwd_null | ✅ |
| test_fwd_nonpositive_h | ✅ |
| test_central_quadratic_at_3 | ✅ |
| test_central_quadratic_at_neg2 | ✅ |
| test_central_cubic_at_2 | ✅ |
| test_central_constant_is_zero | ✅ |
| test_central_linear_exact | ✅ |
| test_central_null | ✅ |
| test_central_nonpositive_h | ✅ |
| test_richardson_quadratic_at_3 | ✅ |
| test_richardson_cubic_at_2 | ✅ |
| test_richardson_more_accurate_than_central | ✅ |
| test_richardson_constant_is_zero | ✅ |
| test_richardson_null | ✅ |
| test_richardson_nonpositive_h | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| diff_forward h=1e-3 | 100,000 | 813 µs | 8 ns |
| diff_central h=1e-3 | 100,000 | 770 µs | 7 ns |
| diff_richardson h=1e-3 | 100,000 | 1,195 µs | 11 ns |

**RESULTS: 19 PASS / 0 FAIL / 19 TOTAL**

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
| test_fwd_quadratic_at_3 | ✅ |
| test_fwd_cubic_at_2 | ✅ |
| test_fwd_constant_is_zero | ✅ |
| test_fwd_linear_is_one | ✅ |
| test_fwd_null | ✅ |
| test_fwd_nonpositive_h | ✅ |
| test_central_quadratic_at_3 | ✅ |
| test_central_quadratic_at_neg2 | ✅ |
| test_central_cubic_at_2 | ✅ |
| test_central_constant_is_zero | ✅ |
| test_central_linear_exact | ✅ |
| test_central_null | ✅ |
| test_central_nonpositive_h | ✅ |
| test_richardson_quadratic_at_3 | ✅ |
| test_richardson_cubic_at_2 | ✅ |
| test_richardson_more_accurate_than_central | ✅ |
| test_richardson_constant_is_zero | ✅ |
| test_richardson_null | ✅ |
| test_richardson_nonpositive_h | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| diff_forward h=1e-3 | 100,000 | 368 µs | 3 ns |
| diff_central h=1e-3 | 100,000 | 439 µs | 4 ns |
| diff_richardson h=1e-3 | 100,000 | 558 µs | 5 ns |

**RESULTS: 19 PASS / 0 FAIL / 19 TOTAL**
