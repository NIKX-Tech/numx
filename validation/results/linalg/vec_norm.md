# Validation: numx_vec_norm

**Validator:** Amir Ab Khoshk
**Date:** 2026-05-25
**numx version:** d81b386

## Hardware: ESP32
*⚠️ Pending.*

## Hardware: Host
- OS: Ubuntu 22.04 / Intel i7-13700H
- Compiler: gcc 11.4.0 -O2 / float32

## Test results

| Test case               | Input      | Expected   | numx output  | Pass/Fail | Notes |
|-------------------------|-----------|------------|--------------|-----------|-------|
| L2 Pythagorean 3-4-5    | [3,4], L2  | 5.0        | 5.00000000   | ✅        | Uses Newton-Raphson sqrt (no libm) |
| L1 known                | [3,4], L1  | 7.0        | 7.00000000   | ✅        |       |
| Linf known              | [3,4], Linf| 4.0        | 4.00000000   | ✅        |       |
| null / invalid          | NULL, n=0  | errors     | errors       | ✅        |       |

*All 8 Unity tests: PASS (test_linalg.c:526–533)*

## Performance (x86-64)

| Variant | N        | Total   | Per call |
|---------|----------|---------|----------|
| L2 n=64 | 100,000  | 4,373 µs | 43 ns   |
| L1 n=64 | 100,000  | 3,743 µs | 37 ns   |
| ESP32   | —        | *pending* | *pending* |

## Python comparison

| Input  | Norm | numpy     | numx      | Error    | OK     |
|--------|------|-----------|-----------|----------|--------|
| [3,4]  | L2   | 5.0       | 5.0       | 0.00e+00 | ✅     |
| [3,4]  | L1   | 7.0       | 7.0       | 0.00e+00 | ✅     |
| [3,4]  | Linf | 4.0       | 4.0       | 0.00e+00 | ✅     |

## Notes
L2 uses internal Newton-Raphson sqrt; exact for integer inputs that are perfect 3-4-5 triples.
