# Validation: numx_compressed_sensing

Covers: `numx_cs_spectral_norm` · `numx_cs_omp` (Orthogonal Matching Pursuit) · `numx_cs_ista` (ISTA / LASSO)

---

## x86-64 — Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-06-07 | **Commit:** d81b386

### spectral_norm test cases

| Matrix | σ_max expected | σ_max computed | Error | Pass |
|--------|---------------|----------------|-------|------|
| I₃ (3×3 identity) | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| diag(3,1,1) 3×3 | 3.0 | 3.0000000 | 0.00e+00 | ✅ |
| null-ptr guards | rc=-1 | rc=-1 | — | ✅ |
| m=0 / n=0 | rc=-2 | rc=-2 | — | ✅ |

### OMP test cases

| A | y | k | x expected | x computed | Error | Pass |
|---|---|---|-----------|-----------|-------|------|
| I₄ | [3,0,0,0] | 1 | [3,0,0,0] | [3.0000000, 0, 0, 0] | 0.00e+00 | ✅ |
| I₄ | [2,0,5,0] | 2 | [2,0,5,0] | [2.0000000, 0, 5.0000000, 0] | 0.00e+00 | ✅ |
| null-ptr guards | — | — | rc=-1 | rc=-1 | — | ✅ |
| m=0 / n=0 / k=0 | — | — | rc=-2 | rc=-2 | — | ✅ |

### ISTA test cases

| A | y | λ | step | iters | x[0] expected | x[0] computed | Error | Pass |
|---|---|---|------|-------|--------------|--------------|-------|------|
| I₄ | [5,0,0,0] | 0.5 | 0.9 | 1000 | 4.5 | 4.5000000 | 0.00e+00 | ✅ |
| null-ptr guards | — | — | — | — | rc=-1 | rc=-1 | — | ✅ |
| step≤0 / λ<0 | — | — | — | — | rc=-2 | rc=-2 | — | ✅ |

### Performance

| Function | A | Total | Per call |
|----------|---|-------|----------|
| spectral_norm 16×32, iter=32 | 100 | 2,032 µs | 20,327 ns |
| omp A=16×32, k=4 | 100 | 136 µs | 1,363 ns |
| ista A=16×32, λ=0.1, iter=500 | 100 | 18,262 µs | 182,629 ns |

---

## ARM64 — macOS 26.2 / Apple M4 Pro / Apple clang 21.0.0 / float32
**Validator:** Erfan Jazeb Nikoo | **Date:** 2026-06-09 | **Commit:** 2fc85d0

### Test cases

| Test | Result |
|------|--------|
| test_spectral_norm_identity | ✅ |
| test_spectral_norm_scaled_identity | ✅ |
| test_spectral_norm_tall_matrix | ✅ |
| test_spectral_norm_null_returns_error | ✅ |
| test_spectral_norm_invalid_arg | ✅ |
| test_omp_identity_1sparse | ✅ |
| test_omp_identity_2sparse | ✅ |
| test_omp_overdetermined_1sparse | ✅ |
| test_omp_null_returns_error | ✅ |
| test_omp_invalid_arg_returns_error | ✅ |
| test_ista_identity_shrinkage | ✅ |
| test_ista_zero_lambda_recovery | ✅ |
| test_ista_large_lambda_zeros | ✅ |
| test_ista_null_returns_error | ✅ |
| test_ista_invalid_step_returns_error | ✅ |

*300 / 300 Unity tests PASS*

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| cs_spectral_norm 16×32 iter=32 | 100 | 1,403 µs | 14,030 ns |
| cs_omp 16×32 k=4 | 100 | 86 µs | 860 ns |
| cs_ista 16×32 lam=0.1 iter=500 | 100 | 11,144 µs | 111,440 ns |

**RESULTS: 15 PASS / 0 FAIL / 15 TOTAL**

---

## ARM64 — macOS 26.2 / Apple M1 Pro / Apple clang 17.0.0 / float32
**Validator:** Erfan Jazeb Nikoo | **Date:** 2026-06-08 | **Commit:** 1380ab1

### Test cases

| Test | Result |
|------|--------|
| test_spectral_norm_identity | ✅ |
| test_spectral_norm_scaled_identity | ✅ |
| test_spectral_norm_tall_matrix | ✅ |
| test_spectral_norm_null_returns_error | ✅ |
| test_spectral_norm_invalid_arg | ✅ |
| test_omp_identity_1sparse | ✅ |
| test_omp_identity_2sparse | ✅ |
| test_omp_overdetermined_1sparse | ✅ |
| test_omp_null_returns_error | ✅ |
| test_omp_invalid_arg_returns_error | ✅ |
| test_ista_identity_shrinkage | ✅ |
| test_ista_zero_lambda_recovery | ✅ |
| test_ista_large_lambda_zeros | ✅ |
| test_ista_null_returns_error | ✅ |
| test_ista_invalid_step_returns_error | ✅ |

*300 / 300 Unity tests PASS*

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| cs_spectral_norm 16×32 iter=32 | 100 | 2,191 µs | 21,910 ns |
| cs_omp 16×32 k=4 | 100 | 117 µs | 1,170 ns |
| cs_ista 16×32 lam=0.1 iter=500 | 100 | 16,765 µs | 167,650 ns |

**RESULTS: 15 PASS / 0 FAIL / 15 TOTAL**

---

## ESP32-S3 — ESP-IDF v5.5.2 / Xtensa LX7 / xtensa-esp32s3-elf-gcc / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-29 | **Commit:** d81b386

> **Stack note:** `numx_cs_omp` uses ~9 KB of internal local variables (bounded by
> `NUMX_MAX_CS_MEASUREMENTS` / `NUMX_MAX_CS_SPARSITY`). Requires
> `CONFIG_ESP_MAIN_TASK_STACK_SIZE >= 16384` in `sdkconfig`.

### Test cases — spectral_norm

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| spectral_norm I3 rc | rc=0 | rc=0 | — | ✅ |
| spectral_norm I3=1.0 | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| spectral_norm D3=3.0 | 3.0 | 3.0000000 | 0.00e+00 | ✅ |
| spectral_norm null-A | rc=-1 | rc=-1 | — | ✅ |
| spectral_norm null-out | rc=-1 | rc=-1 | — | ✅ |
| spectral_norm m=0 | rc=-2 | rc=-2 | — | ✅ |
| spectral_norm n=0 | rc=-2 | rc=-2 | — | ✅ |

### Test cases — omp

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| omp identity rc | rc=0 | rc=0 | — | ✅ |
| omp identity x[0]=3 | 3.0 | 3.0000000 | 0.00e+00 | ✅ |
| omp identity x[1]=0 | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| omp identity x[2]=0 | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| omp identity x[3]=0 | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| omp 2-sparse x[0]=2 | 2.0 | 2.0000000 | 0.00e+00 | ✅ |
| omp 2-sparse x[2]=5 | 5.0 | 5.0000000 | 0.00e+00 | ✅ |
| omp null-A | rc=-1 | rc=-1 | — | ✅ |
| omp null-y | rc=-1 | rc=-1 | — | ✅ |
| omp null-x | rc=-1 | rc=-1 | — | ✅ |
| omp m=0 | rc=-2 | rc=-2 | — | ✅ |
| omp n=0 | rc=-2 | rc=-2 | — | ✅ |
| omp k=0 | rc=-2 | rc=-2 | — | ✅ |

### Test cases — ista

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| ista identity rc | rc=0 | rc=0 | — | ✅ |
| ista identity x[0]≈4.5 | 4.5 | 4.5000000 | 0.00e+00 | ✅ |
| ista identity x[1]=0 | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| ista identity x[2]=0 | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| ista null-A | rc=-1 | rc=-1 | — | ✅ |
| ista null-y | rc=-1 | rc=-1 | — | ✅ |
| ista null-x | rc=-1 | rc=-1 | — | ✅ |
| ista step=0 | rc=-2 | rc=-2 | — | ✅ |
| ista step<0 | rc=-2 | rc=-2 | — | ✅ |
| ista lambda<0 | rc=-2 | rc=-2 | — | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| cs_spectral_norm 16x32 iter=32 | 100 | — | — |
| cs_omp 16x32 k=4 | 100 | — | — |
| cs_ista 16x32 lam=0.1 iter=100 | 100 | — | — |

### Precision vs reference

| Function | Scenario | Exact | Computed | Error |
|----------|----------|-------|----------|-------|
| spectral_norm | I₃ σ_max | 1.0 | 1.0000000 | 0.00e+00 |
| spectral_norm | diag(3,1,1) σ_max | 3.0 | 3.0000000 | 0.00e+00 |
| omp | 1-sparse recovery x[0] | 3.0 | 3.0000000 | 0.00e+00 |
| omp | 2-sparse recovery x[0],x[2] | 2.0, 5.0 | exact | 0.00e+00 |
| ista | soft-threshold x[0] (λ=0.5, step=0.9) | 4.5 | 4.5000000 | 0.00e+00 |

*All results exact in float32. OMP recovers sparse signals from identity measurements without error. ISTA converges to the correct soft-threshold solution (5.0 − 0.5 = 4.5) within 1000 iterations.*

**RESULTS: 30 PASS / 0 FAIL / 30 TOTAL**

---

## Windows x64 — Windows 11 / MSVC 14.51 (VS 2026 Build Tools) / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-06-05 | **Commit:** 4c4c0f0

### Test cases

| Test | Result |
|------|--------|
| test_omp_identity_1sparse | ✅ |
| test_omp_identity_2sparse | ✅ |
| test_omp_overdetermined_1sparse | ✅ |
| test_omp_null_returns_error | ✅ |
| test_omp_invalid_arg_returns_error | ✅ |
| test_ista_identity_shrinkage | ✅ |
| test_ista_zero_lambda_recovery | ✅ |
| test_ista_large_lambda_zeros | ✅ |
| test_ista_null_returns_error | ✅ |
| test_ista_invalid_step_returns_error | ✅ |
| test_spectral_norm_identity | ✅ |
| test_spectral_norm_scaled_identity | ✅ |
| test_spectral_norm_tall_matrix | ✅ |
| test_spectral_norm_null_returns_error | ✅ |
| test_spectral_norm_invalid_arg | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| cs_spectral_norm 16x32 iter=32 | 100 | 6,299 µs | 62,990 ns |
| cs_omp 16x32 k=4 | 100 | 370 µs | 3,700 ns |
| cs_ista 16x32 lam=0.1 iter=100 | 100 | 16,238 µs | 162,380 ns |

**RESULTS: 15 PASS / 0 FAIL / 15 TOTAL**

---

## Windows x64 — Windows 11 / MSVC 14.51 (VS 2026 Build Tools) / float64
**Validator:** Amir Ab Khoshk | **Date:** 2026-06-06 | **Commit:** 1bba399

> Build note: Some Unity assertions use float32 tolerances even when built with
> NUMX_USE_DOUBLE. Affected tests still pass because the errors are well within
> float32 tolerance, but the assertion threshold does not tighten to double precision.
> This is a test harness configuration issue, not a library bug.

### Test cases

| Test | Result |
|------|--------|
| test_omp_identity_1sparse | ✅ |
| test_omp_identity_2sparse | ✅ |
| test_omp_overdetermined_1sparse | ✅ |
| test_omp_null_returns_error | ✅ |
| test_omp_invalid_arg_returns_error | ✅ |
| test_ista_identity_shrinkage | ✅ |
| test_ista_zero_lambda_recovery | ✅ |
| test_ista_large_lambda_zeros | ✅ |
| test_ista_null_returns_error | ✅ |
| test_ista_invalid_step_returns_error | ✅ |
| test_spectral_norm_identity | ✅ |
| test_spectral_norm_scaled_identity | ✅ |
| test_spectral_norm_tall_matrix | ✅ |
| test_spectral_norm_null_returns_error | ✅ |
| test_spectral_norm_invalid_arg | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| cs_spectral_norm 16x32 iter=32 | 100 | 6,257 µs | 62,570 ns |
| cs_omp 16x32 k=4 | 100 | 367 µs | 3,670 ns |
| cs_ista 16x32 lam=0.1 iter=100 | 100 | 16,109 µs | 161,090 ns |

**RESULTS: 15 PASS / 0 FAIL / 15 TOTAL**
