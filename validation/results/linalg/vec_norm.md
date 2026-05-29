# Validation: numx_vec_norm

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
- OS: Ubuntu 22.04 / Intel i7-13700H
- Compiler: gcc 11.4.0 -O2 / float32

## Test results

### ESP32-S3 hardware run — 9/9 PASS

| Test case                  | Input           | Expected             | numx output | Pass/Fail |
|----------------------------|-----------------|----------------------|-------------|-----------|
| vec_norm L2 rc             | [3,4], L2       | NUMX_OK (0)          | rc=0        | ✅        |
| vec_norm L2 [3,4]=5        | [3,4], L2       | 5.0                  | 5.0000000   | ✅        |
| vec_norm L1 [3,-4,0]=7     | [3,-4,0], L1    | 7.0                  | 7.0000000   | ✅        |
| vec_norm Linf [3,-5,4]=5   | [3,-5,4], Linf  | 5.0                  | 5.0000000   | ✅        |
| vec_norm unit = 1          | [1,0,0], L2     | 1.0                  | 1.0000000   | ✅        |
| vec_norm zero = 0          | [0,0,0], L2     | 0.0                  | 0.0000000   | ✅        |
| vec_norm null-a            | a=NULL          | ERR_NULL_PTR (-1)    | rc=-1       | ✅        |
| vec_norm null-out          | out=NULL        | ERR_NULL_PTR (-1)    | rc=-1       | ✅        |
| vec_norm bad-type          | type=99         | ERR_INVALID_ARG (-2) | rc=-2       | ✅        |

**ESP32-S3 result: 9 PASS / 0 FAIL**

*All 8 Unity tests (host): PASS (test_runner.c:526–533)*

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
