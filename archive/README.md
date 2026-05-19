# archive/legacy_integral

Pre-restructure prototype code kept for historical reference and as an
algorithmic reference when implementing Phase 1 integration functions.

## What is here

| File | Maps to Phase 1 task |
|------|----------------------|
| `integral.h` | P1.16, P1.17 |
| `rectangle.c` | — (rectangle rule, informational only) |
| `trapezium.c` | P1.16 `numx_integrate_trap` |
| `simpsons.c` | P1.17 `numx_integrate_simpson` |
| `test.c` | — (manual test driver) |

## Why it does not conform to numx standards

- Uses raw `double` instead of `numx_real_t`
- Includes `<math.h>` and `<stdlib.h>`
- No error codes, no NULL checks, no Doxygen
- Windows-style path separator in `test.c`

## How to use it

Read-only reference. Not compiled by CMake. Do not import from `src/`.
