# Validation: numx_vec_dot

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
- OS: Ubuntu 22.04 LTS (kernel 6.8.0-106-generic)
- CPU: 13th Gen Intel Core i7-13700H
- Compiler: gcc 11.4.0
- Optimization: -O2
- Precision: float32 (default)

## Test results

### ESP32-S3 hardware run — 11/11 PASS

| Test case                 | Input               | Expected              | numx output  | Pass/Fail |
|---------------------------|---------------------|-----------------------|--------------|-----------|
| vec_dot known-answer rc   | [1,2,3]·[4,5,6]     | NUMX_OK (0)           | rc=0         | ✅        |
| vec_dot [1,2,3]·[4,5,6]=32| [1,2,3]·[4,5,6]     | 32.0                  | 32.0000000   | ✅        |
| vec_dot orthogonal = 0    | [1,0,0]·[0,1,0]     | 0.0                   | 0.0000000    | ✅        |
| vec_dot commutative       | a·b == b·a          | true                  | true         | ✅        |
| vec_dot self=[3,4] = 25   | [3,4]·[3,4]         | 25.0                  | 25.0000000   | ✅        |
| vec_dot n=1: 7*3=21       | [7]·[3]             | 21.0                  | 21.0000000   | ✅        |
| vec_dot zero vec = 0      | [0,0,0]·[4,5,6]     | 0.0                   | 0.0000000    | ✅        |
| vec_dot null-a            | a=NULL              | ERR_NULL_PTR (-1)     | rc=-1        | ✅        |
| vec_dot null-b            | b=NULL              | ERR_NULL_PTR (-1)     | rc=-1        | ✅        |
| vec_dot null-out          | out=NULL            | ERR_NULL_PTR (-1)     | rc=-1        | ✅        |
| vec_dot n=0               | n=0                 | ERR_INVALID_ARG (-2)  | rc=-2        | ✅        |

**ESP32-S3 result: 11 PASS / 0 FAIL**

*All 10 Unity tests (host): PASS (test_runner.c:514–523)*

## Performance (x86-64)

| Platform  | N iterations | Total time | Per call  |
|-----------|-------------|------------|-----------|
| x86-64    | 100,000     | 1,542 µs   | 15 ns     |
| ESP32     | —           | *pending*  | *pending* |

## Python comparison (numpy reference)

| Input              | numpy result | numx result | Absolute error | Within tolerance |
|--------------------|-------------|-------------|----------------|-----------------|
| [1,2,3,4]·[5,6,7,8]| 70.0        | 70.0        | 0.00e+00       | ✅ (< 1e-5)     |
| [1,0,0]·[0,1,0]    | 0.0         | 0.0         | 0.00e+00       | ✅              |

## Notes
None. Exact integer inputs → zero floating-point error.
