# Validation: numx_cholesky_decompose

Covers: `numx_cholesky_decompose`

---

## ARM64 — macOS 26.5.1 / Apple M4 Pro / Apple clang 21.0.0 / float32
**Validator:** Erfan Jazeb Nikoo | **Date:** 2026-07-09 | **Commit:** 73a7610

> Implementation contributed by Erfan Esmaeili ([#51](https://github.com/NIKX-Tech/numx/pull/51)).
> Verified with AddressSanitizer and UndefinedBehaviorSanitizer (335/335 tests, zero
> issues) and compiled clean under `-std=c99 -pedantic-errors -Wall -Wextra -Werror`
> in both float32 and float64. Only one platform validated so far; full multi-platform
> sweep (Windows, Raspberry Pi, ESP32-S3) to follow, matching every other function in
> this table.

### Test cases

| Function | Input / scenario | Expected | Computed | Error | Pass |
|----------|-----------------|----------|----------|-------|------|
| cholesky_decompose | 3×3 textbook SPD, L[0][0] | 2.0 | 2.0000000 | 0.00e+00 | ✅ |
| cholesky_decompose | 3×3 textbook SPD, L[1][0] | 6.0 | 6.0000000 | 0.00e+00 | ✅ |
| cholesky_decompose | 3×3 textbook SPD, L[1][1] | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| cholesky_decompose | 3×3 textbook SPD, L[2][0] | -8.0 | -8.0000000 | 0.00e+00 | ✅ |
| cholesky_decompose | 3×3 textbook SPD, L[2][1] | 5.0 | 5.0000000 | 0.00e+00 | ✅ |
| cholesky_decompose | 3×3 textbook SPD, L[2][2] | 3.0 | 3.0000000 | 0.00e+00 | ✅ |
| cholesky_decompose | L·L^T reconstruction vs original A | A | A | 0.00e+00 | ✅ |
| cholesky_decompose | non-SPD input (zero diagonal pivot) | rc=NUMX_ERR_SINGULAR | rc=NUMX_ERR_SINGULAR | — | ✅ |
| null-ptr guards | A, L | rc=NUMX_ERR_NULL_PTR | rc=NUMX_ERR_NULL_PTR | — | ✅ |
| invalid-arg guard | n=0 | rc=NUMX_ERR_INVALID_ARG | rc=NUMX_ERR_INVALID_ARG | — | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| cholesky_decompose 3×3 | 100,000 | 2,195 µs | 21 ns |

**RESULTS: 6 PASS / 0 FAIL / 6 TOTAL** (Unity suite, `tests/test_linalg.c`); validation-runner checks above match.

---

## Remaining validation

Not yet run: Windows x64 (MSVC), Raspberry Pi 4 (aarch64/gcc), ESP32-S3 (Xtensa/ESP-IDF),
Linux x86-64. Needed before this function carries the same validation weight as the
rest of `linalg`.
