# Validation: numx_mat_mul

**Validator:** Amir Ab Khoshk
**Date:** 2026-05-25
**numx version:** d81b386

## Hardware: ESP32
*⚠️ Pending.*

## Hardware: Host
- OS: Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 -O2 / float32

## Test results

| Test case          | Dims        | Expected             | numx output          | Pass/Fail |
|--------------------|------------|----------------------|----------------------|-----------|
| 2×2 standard       | 2×2 × 2×2  | [[19,22],[43,50]]    | [[19,22],[43,50]]    | ✅        |
| 2×3 × 3×2          | 2×3 × 3×2  | [[58,64],[139,154]]  | [[58,64],[139,154]]  | ✅        |
| identity           | A × I      | A                    | A                    | ✅        |
| dim mismatch       | ca≠rb      | ERR_INVALID_ARG      | -2                   | ✅        |

*All 5 Unity tests: PASS (test_linalg.c:543–547)*

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
