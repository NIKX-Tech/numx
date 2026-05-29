# Validation: numx_vec_dot

**Validator:** Amir Ab Khoshk
**Date:** 2026-05-25
**numx version:** d81b386

## Hardware: ESP32
*⚠️ Pending — hardware benchmarks not yet collected.*

## Hardware: Host
- OS: Ubuntu 22.04 LTS (kernel 6.8.0-106-generic)
- CPU: 13th Gen Intel Core i7-13700H
- Compiler: gcc 11.4.0
- Optimization: -O2
- Precision: float32 (default)

## Test results

| Test case                      | Input                          | Expected     | numx output  | Pass/Fail | Notes |
|--------------------------------|-------------------------------|--------------|--------------|-----------|-------|
| basic dot product              | [1,2,3,4]·[5,6,7,8]           | 70.0         | 70.00000000  | ✅        |       |
| orthogonal vectors             | [1,0,0]·[0,1,0]               | 0.0          | 0.00000000   | ✅        |       |
| null a → ERR_NULL_PTR          | a=NULL                        | NUMX_ERR_NULL_PTR | -1      | ✅        |       |
| null b → ERR_NULL_PTR          | b=NULL                        | NUMX_ERR_NULL_PTR | -1      | ✅        |       |
| n=0 → ERR_INVALID_ARG          | n=0                           | NUMX_ERR_INVALID_ARG | -2   | ✅        |       |

*All 10 Unity tests: PASS (see test_linalg.c:514–523)*

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
