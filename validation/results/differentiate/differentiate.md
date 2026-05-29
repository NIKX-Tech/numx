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

## ESP32-S3
**Status:** ⚠️ Pending
