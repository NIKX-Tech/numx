# Validation: numx_mat_mul

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

### ESP32-S3 hardware run — 12/12 PASS (mat_mul) + 12/12 PASS (mat_transpose)

| Test case                    | Input          | Expected             | numx output  | Pass/Fail |
|------------------------------|----------------|----------------------|--------------|-----------|
| mat_mul 2×2 rc               | 2×2 × 2×2      | NUMX_OK (0)          | rc=0         | ✅        |
| mat_mul 2×2 C[0,0]=19        | same           | 19.0                 | 19.0000000   | ✅        |
| mat_mul 2×2 C[0,1]=22        | same           | 22.0                 | 22.0000000   | ✅        |
| mat_mul 2×2 C[1,0]=43        | same           | 43.0                 | 43.0000000   | ✅        |
| mat_mul 2×2 C[1,1]=50        | same           | 50.0                 | 50.0000000   | ✅        |
| mat_mul 2x3*3x2 C[0,0]=58    | 2×3 × 3×2      | 58.0                 | 58.0000000   | ✅        |
| mat_mul 2x3*3x2 C[1,1]=154   | same           | 154.0                | 154.0000000  | ✅        |
| mat_mul A*I=A [0]            | A × I          | A[0]=1.0             | 1.0000000    | ✅        |
| mat_mul A*I=A [3]            | same           | A[3]=4.0             | 4.0000000    | ✅        |
| mat_mul dim-mismatch         | cols≠rows      | ERR_INVALID_ARG (-2) | rc=-2        | ✅        |
| mat_mul null-A               | A=NULL         | ERR_NULL_PTR (-1)    | rc=-1        | ✅        |
| mat_mul null-C               | C=NULL         | ERR_NULL_PTR (-1)    | rc=-1        | ✅        |
| mat_transpose 2x3 rc         | 2×3 matrix     | NUMX_OK (0)          | rc=0         | ✅        |
| mat_transpose AT[0]=1        | A=[1..6]       | 1.0                  | 1.0000000    | ✅        |
| mat_transpose AT[1]=4        | same           | 4.0                  | 4.0000000    | ✅        |
| mat_transpose AT[2]=2        | same           | 2.0                  | 2.0000000    | ✅        |
| mat_transpose AT[5]=6        | same           | 6.0                  | 6.0000000    | ✅        |
| mat_transpose T(T(A))=A      | double T       | identity             | true         | ✅        |
| mat_transpose_sq 2x2 rc      | 2×2 square     | NUMX_OK (0)          | rc=0         | ✅        |
| mat_transpose_sq S[1]=3      | [[1,2],[3,4]]  | 3.0                  | 3.0000000    | ✅        |
| mat_transpose_sq S[2]=2      | same           | 2.0                  | 2.0000000    | ✅        |
| mat_transpose_sq twice=ident | double T_sq    | identity             | true         | ✅        |
| mat_transpose null           | A=NULL         | ERR_NULL_PTR (-1)    | rc=-1        | ✅        |
| mat_transpose_sq null        | A=NULL         | ERR_NULL_PTR (-1)    | rc=-1        | ✅        |

**ESP32-S3 result: 24 PASS / 0 FAIL**

*All 5 Unity tests (host): PASS (test_runner.c:543–547)*

## Performance (x86-64)

| Size | N       | Total    | Per call  |
|------|---------|----------|-----------|
| 4×4  | 100,000 | 2,615 µs | 26 ns     |
| 8×8  | 10,000  | 2,038 µs | 203 ns    |
| ESP32|         | *pending*| *pending* |

## Python comparison

| Case     | [i,j] | numpy | numx | Error    | OK |
|----------|-------|-------|------|----------|----|
| 2×2 [0,0]| —     | 19.0  | 19.0 | 0.00e+00 | ✅ |
| 2×2 [0,1]| —     | 22.0  | 22.0 | 0.00e+00 | ✅ |
| 2×2 [1,0]| —     | 43.0  | 43.0 | 0.00e+00 | ✅ |
| 2×2 [1,1]| —     | 50.0  | 50.0 | 0.00e+00 | ✅ |

## Notes
8×8 is ~7.8× slower than 4×4 (expected: 8× for O(n³) naive). Performance consistent.
