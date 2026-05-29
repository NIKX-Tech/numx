# Validation: numx_mat_det

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

### ESP32-S3 hardware run — 8/8 PASS

| Test case           | n | Input              | Expected             | numx output  | Pass/Fail |
|---------------------|---|--------------------|----------------------|--------------|-----------|
| mat_det 1×1 rc      | 1 | [[7]]              | NUMX_OK (0)          | rc=0         | ✅        |
| mat_det 1×1 = 7     | 1 | [[7]]              | 7.0                  | 7.0000000    | ✅        |
| mat_det 2×2 = -2    | 2 | [[1,2],[3,4]]      | -2.0                 | -2.0000000   | ✅        |
| mat_det 3×3 = -3    | 3 | [[1,2,3]...]       | -3.0                 | -3.0000000   | ✅        |
| mat_det I = 1       | 3 | identity           | 1.0                  | 1.0000000    | ✅        |
| mat_det singular=0  | 2 | row2=2×row1        | 0.0                  | 0.0000000    | ✅        |
| mat_det null-A      | — | A=NULL             | ERR_NULL_PTR (-1)    | rc=-1        | ✅        |
| mat_det null-out    | — | out=NULL           | ERR_NULL_PTR (-1)    | rc=-1        | ✅        |

**ESP32-S3 result: 8 PASS / 0 FAIL**

*All 7 Unity tests (host): PASS (test_runner.c:558–564)*

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
