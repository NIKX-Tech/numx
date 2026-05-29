# Validation: numx_lu_decompose + numx_lu_solve

**Validator:** Amir Ab Khoshk
**Date:** 2026-05-25
**numx version:** d81b386

## Hardware: ESP32
- Board: ESP32-S3-DevKitC-1
- CPU: Xtensa LX7 @ 160 MHz, HW FPU
- IDF: ESP-IDF v5.5.2
- numx commit: d81b386
- Compiler: xtensa-esp32s3-elf-gcc -O2 -std=gnu99
- Precision: float32
- Validator: Amir Ab Khoshk

## Hardware: Host
- OS: Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 -O2 / float32

## Test results

### ESP32-S3 hardware run — 15/15 PASS

| Test case                 | Input              | Expected             | numx output   | Pass/Fail | Notes |
|---------------------------|--------------------|----------------------|---------------|-----------|-------|
| lu_decompose 3×3 rc       | textbook 3×3       | NUMX_OK (0)          | rc=0          | ✅        |       |
| lu_solve 3×3 rc           | same               | NUMX_OK (0)          | rc=0          | ✅        |       |
| lu_solve x[0]=2           | A*x=b, b=[8,-11,-3]| 2.0                  | 1.9999996     | ✅        | err=3.58e-07, within float32 ε |
| lu_solve x[1]=3           | same               | 3.0                  | 3.0000002     | ✅        | err=2.38e-07, within float32 ε |
| lu_solve x[2]=-1          | same               | -1.0                 | -1.0000004    | ✅        | err=3.58e-07, within float32 ε |
| lu_solve I*x x[0]=5       | I*x=b, b=[5,7]     | 5.0                  | 5.0000000     | ✅        |       |
| lu_solve I*x x[1]=7       | same               | 7.0                  | 7.0000000     | ✅        |       |
| lu_solve residual[0]      | Ax vs b[0]=8       | 8.0                  | 8.0000000     | ✅        |       |
| lu_solve residual[1]      | Ax vs b[1]=-11     | -11.0                | -11.0000000   | ✅        |       |
| lu_solve residual[2]      | Ax vs b[2]=-3      | -3.0                 | -2.9999998    | ✅        | err=2.38e-07, within float32 ε |
| lu_decompose singular     | row2=2×row1        | ERR_SINGULAR (-3)    | rc=-3         | ✅        |       |
| lu_decompose null-A       | A=NULL             | ERR_NULL_PTR (-1)    | rc=-1         | ✅        |       |
| lu_decompose null-LU      | LU=NULL            | ERR_NULL_PTR (-1)    | rc=-1         | ✅        |       |
| lu_solve null-LU          | LU=NULL            | ERR_NULL_PTR (-1)    | rc=-1         | ✅        |       |
| lu_solve null-x           | x=NULL             | ERR_NULL_PTR (-1)    | rc=-1         | ✅        |       |

**ESP32-S3 result: 15 PASS / 0 FAIL**

*All 8 Unity tests (host): PASS (test_runner.c:567–574)*

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
