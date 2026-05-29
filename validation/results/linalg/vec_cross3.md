# Validation: numx_vec_cross3

**Validator:** Amir Ab Khoshk
**Date:** 2026-05-25
**numx version:** d81b386

## Hardware: ESP32
*⚠️ Pending.*

## Hardware: Host
- OS: Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 -O2 / float32

## Test results

| Test case             | Input            | Expected    | numx output        | Pass/Fail |
|-----------------------|-----------------|-------------|---------------------|-----------|
| x × y = z            | [1,0,0]×[0,1,0] | [0,0,1]     | [0,0,1]             | ✅        |
| anti-commutative      | y × x           | [0,0,-1]    | [0,0,-1]            | ✅        |
| parallel → zero       | [1,2,3]×[2,4,6] | [0,0,0]     | [0,0,0]             | ✅        |
| alias-safe            | result=a        | correct     | correct             | ✅        |
| [1,2,3]×[4,5,6]      | see above       | [-3,6,-3]   | [-3, 6, -3]         | ✅        |

*All 5 Unity tests: PASS (test_linalg.c:536–540)*

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
