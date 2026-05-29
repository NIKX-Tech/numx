# Validation: numx_mat_det

**Validator:** Amir Ab Khoshk
**Date:** 2026-05-25
**numx version:** d81b386

## Hardware: ESP32
*⚠️ Pending.*

## Hardware: Host
- OS: Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 -O2 / float32

## Test results

| Test case        | n  | Expected  | numx output  | Pass/Fail |
|------------------|----|-----------|--------------|-----------|
| 1×1 [[5]]        | 1  | 5.0       | 5.00000000   | ✅        |
| 2×2 [[1,2],[3,4]]| 2  | -2.0      | -2.00000000  | ✅        |
| 3×3 (textbook)   | 3  | 4.0       | 4.00000000   | ✅        |
| 4×4 symmetric    | 4  | 20.0      | 20.00000000  | ✅        |
| singular → 0     | 2  | 0.0       | 0.00000000   | ✅        |
| identity → 1     | 3  | 1.0       | 1.00000000   | ✅        |

*All 7 Unity tests: PASS (test_linalg.c:558–564)*

## Performance (x86-64)

| Size  | N       | Total   | Per call |
|-------|---------|---------|----------|
| 4×4   | 100,000 | 376 µs  | 3 ns     |
| ESP32 |         | *pending*|*pending*|

## Python comparison

| n  | numpy det   | numx det    | Error    | OK |
|----|-------------|-------------|----------|----|
| 2  | -2.0        | -2.0        | 0.00e+00 | ✅ |
| 3  | 4.0         | 4.0         | 0.00e+00 | ✅ |
| 4  | 20.0        | 20.0        | 0.00e+00 | ✅ |

## Notes
Uses exact cofactor formulas for n≤4, LU for n>4. All tested cases use exact path (integer inputs → zero rounding error).
