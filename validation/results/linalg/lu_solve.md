# Validation: numx_lu_decompose + numx_lu_solve

**Validator:** Amir Ab Khoshk
**Date:** 2026-05-25
**numx version:** d81b386

## Hardware: ESP32
*⚠️ Pending.*

## Hardware: Host
- OS: Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 -O2 / float32

## Test results

| Test case              | Input             | Expected x    | numx x         | Pass/Fail | Notes        |
|------------------------|------------------|---------------|----------------|-----------|--------------|
| 4×4 standard A*x=b     | b=[1,2,4,5]       | [1,0,-1,1]    | [1.00000012, 0, -1.00000012, 1] | ✅ | err=1.2e-7 within float32 limit |
| 3×3 textbook           | textbook system   | known         | known          | ✅        |              |
| identity system        | I*x=b             | b             | b              | ✅        |              |
| residual \|\|Ax-b\|\|  | 4×4               | ~0            | < 1e-5         | ✅        |              |
| singular → ERR_SINGULAR|                   | -3            | -3             | ✅        |              |

*All 8 Unity tests: PASS (test_linalg.c:567–574)*

## Performance (x86-64)

| Function             | N       | Total    | Per call |
|----------------------|---------|----------|----------|
| lu_decompose 4×4     | 100,000 | 2,989 µs | 29 ns    |
| lu_solve 4×4 (pre-factored) | 100,000 | 1,156 µs | 11 ns |
| ESP32                |         | *pending*| *pending*|

## Python comparison

| Component | numpy ref  | numx       | Error    | OK |
|-----------|-----------|------------|----------|----|
| x[0]      | 1.0       | 1.00000012 | 1.19e-07 | ✅ |
| x[1]      | 0.0       | 0.00000000 | 0.00e+00 | ✅ |
| x[2]      | -1.0      | -1.00000012| 1.19e-07 | ✅ |
| x[3]      | 1.0       | 1.00000000 | 0.00e+00 | ✅ |

## Notes
Errors of 1.19e-07 on x[0] and x[2] are within single-precision machine epsilon (~1.2e-7). This is expected for float32 LU with partial pivoting on a near-unity condition-number system. **Not a bug.**
