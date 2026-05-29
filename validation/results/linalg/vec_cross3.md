# Validation: numx_vec_cross3

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

### ESP32-S3 hardware run — 12/12 PASS

| Test case                  | Input            | Expected             | numx output | Pass/Fail |
|----------------------------|------------------|----------------------|-------------|-----------|
| vec_cross3 rc              | [1,0,0]×[0,1,0]  | NUMX_OK (0)          | rc=0        | ✅        |
| vec_cross3 x×y [0]         | same             | 0.0                  | 0.0000000   | ✅        |
| vec_cross3 x×y [1]         | same             | 0.0                  | 0.0000000   | ✅        |
| vec_cross3 x×y [2]         | same             | 1.0                  | 1.0000000   | ✅        |
| vec_cross3 anticomm [0]    | [1,2,3]×[4,5,6]  | axb[0]+bxa[0]=0      | 0.0000000   | ✅        |
| vec_cross3 anticomm [1]    | same             | axb[1]+bxa[1]=0      | 0.0000000   | ✅        |
| vec_cross3 anticomm [2]    | same             | axb[2]+bxa[2]=0      | 0.0000000   | ✅        |
| vec_cross3 parallel [0]    | [2,0,0]×[5,0,0]  | 0.0                  | 0.0000000   | ✅        |
| vec_cross3 parallel [2]    | same             | 0.0                  | 0.0000000   | ✅        |
| vec_cross3 null-a          | a=NULL           | ERR_NULL_PTR (-1)    | rc=-1       | ✅        |
| vec_cross3 null-b          | b=NULL           | ERR_NULL_PTR (-1)    | rc=-1       | ✅        |
| vec_cross3 null-c          | c=NULL           | ERR_NULL_PTR (-1)    | rc=-1       | ✅        |

**ESP32-S3 result: 12 PASS / 0 FAIL**

*All 5 Unity tests (host): PASS (test_runner.c:536–540)*

## Performance (x86-64)

| N        | Total  | Per call |
|----------|--------|----------|
| 100,000  | 125 µs | 1 ns     |

## Python comparison

| Input           | component | numpy | numx | Error    | OK |
|-----------------|-----------|-------|------|----------|----|
| [1,2,3]×[4,5,6] | [0]       | -3.0  | -3.0 | 0.00e+00 | ✅ |
| [1,2,3]×[4,5,6] | [1]       |  6.0  |  6.0 | 0.00e+00 | ✅ |
| [1,2,3]×[4,5,6] | [2]       | -3.0  | -3.0 | 0.00e+00 | ✅ |

## Notes
Intermediate values computed before write — alias safety confirmed.
