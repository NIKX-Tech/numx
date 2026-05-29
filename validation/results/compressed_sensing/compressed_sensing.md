# Validation: numx_compressed_sensing

Covers: `numx_cs_spectral_norm` · `numx_cs_omp` (Orthogonal Matching Pursuit) · `numx_cs_ista` (ISTA / LASSO)

---

## x86-64 — Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 / float32
**Validator:** — | **Date:** — | **Commit:** —

### spectral_norm test cases

| Matrix | σ_max expected | σ_max computed | Error | Pass |
|--------|---------------|----------------|-------|------|
| I₃ (3×3 identity) | 1.0 | — | — | — |
| diag(3,1,1) 3×3 | 3.0 | — | — | — |
| null-ptr guards | rc=-1 | — | — | — |
| m=0 / n=0 | rc=-2 | — | — | — |

### OMP test cases

| A | y | k | x expected | x computed | Error | Pass |
|---|---|---|-----------|-----------|-------|------|
| I₄ | [3,0,0,0] | 1 | [3,0,0,0] | — | — | — |
| I₄ | [2,0,5,0] | 2 | [2,0,5,0] | — | — | — |
| null-ptr guards | — | — | rc=-1 | — | — | — |
| m=0 / n=0 / k=0 | — | — | rc=-2 | — | — | — |

### ISTA test cases

| A | y | λ | step | iters | x[0] expected | x[0] computed | Error | Pass |
|---|---|---|------|-------|--------------|--------------|-------|------|
| I₄ | [5,0,0,0] | 0.5 | 0.9 | 1000 | 4.5 | — | — | — |
| null-ptr guards | — | — | — | — | rc=-1 | — | — | — |
| step≤0 / λ<0 | — | — | — | — | rc=-2 | — | — | — |

### Performance

| Function | A | Total | Per call |
|----------|---|-------|----------|
| spectral_norm 16×16, iter=32 | — | — | — |
| omp A=16×32, k=4 | — | — | — |
| ista A=16×32, λ=0.1, iter=500 | — | — | — |

---

## ARM64 — macOS / Apple M4 Pro / Apple clang / float32
**Validator:** — | **Date:** — | **Commit:** —

### Test cases

| Function | Scenario | Expected | Computed | Pass |
|----------|---------|----------|----------|------|
| *(awaiting results)* | | | | |

---

## ESP32-S3 — ESP-IDF v5.5.x / Xtensa LX7 / xtensa-esp32s3-elf-gcc / float32
**Validator:** — | **Date:** — | **Commit:** —

> **Stack note:** `numx_cs_omp` uses ~9 KB of internal local variables (bounded by
> `NUMX_MAX_CS_MEASUREMENTS` / `NUMX_MAX_CS_SPARSITY`). Requires
> `CONFIG_ESP_MAIN_TASK_STACK_SIZE >= 16384` in `sdkconfig`.

### Test cases

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| spectral_norm I3 rc | rc=0 | — | — | — |
| spectral_norm I3=1.0 | 1.0 | — | — | — |
| spectral_norm D3=3.0 | 3.0 | — | — | — |
| omp identity rc | rc=0 | — | — | — |
| omp identity x[0]=3 | 3.0 | — | — | — |
| omp identity x[1]=0 | 0.0 | — | — | — |
| omp 2-sparse x[0]=2 | 2.0 | — | — | — |
| omp 2-sparse x[2]=5 | 5.0 | — | — | — |
| ista identity rc | rc=0 | — | — | — |
| ista identity x[0]≈4.5 | 4.5 | — | — | — |
| ista identity x[1]=0 | 0.0 | — | — | — |
| null-ptr guards | rc=-1 | — | — | — |
| invalid-arg guards | rc=-2 | — | — | — |

**RESULTS: — PASS / — FAIL / — TOTAL**
