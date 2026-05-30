# Validation: numx_sketch (randomized SVD)

Covers: `numx_sketch_rsvd` — Halko-Martinsson-Tropp randomized SVD

---

## x86-64 — Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 / float32
**Validator:** — | **Date:** — | **Commit:** —

### Test cases

| A | m | n | rank | oversample | seed | σ expected | σ computed | Error | Pass |
|---|---|---|------|-----------|------|-----------|-----------|-------|------|
| diag(2,0,0) | 3 | 3 | 1 | 2 | 42 | S[0]=2.0 | — | — | — |
| diag(4,2,0,0) | 4 | 4 | 2 | 2 | 12345 | S[0]=4.0, S[1]=2.0 | — | — | — |
| diag(3,1) | 2 | 2 | 1 | 1 | 0 | S[0]=3.0 | — | — | — |
| S descending | — | — | 2 | — | — | S[0]≥S[1]≥0 | — | — | — |
| null-ptr guards | — | — | — | — | — | rc=-1 | — | — | — |
| m=0 / n=0 / rank=0 | — | — | — | — | — | rc=-2 | — | — | — |

### Reconstruction quality

| A | rank | ‖A − UΣVᵀ‖_F / ‖A‖_F | Pass |
|---|------|----------------------|------|
| diag(2,0,0) 3×3 | 1 | ≈0 | — |
| diag(4,2,0,0) 4×4 | 2 | ≈0 | — |

### Performance

| A size | rank | oversample | Total | Per call |
|--------|------|-----------|-------|----------|
| 16×16 | 4 | 4 | — | — |
| 32×32 | 8 | 4 | — | — |
| 64×64 | 8 | 4 | — | — |

---

## ARM64 — macOS / Apple M4 Pro / Apple clang / float32
**Validator:** — | **Date:** — | **Commit:** —

### Test cases

| A | rank | σ expected | σ computed | Error | Pass |
|---|------|-----------|-----------|-------|------|
| *(awaiting results)* | | | | | |

---

## ESP32-S3 — ESP-IDF v5.5.x / Xtensa LX7 / xtensa-esp32s3-elf-gcc / float32
**Validator:** — | **Date:** — | **Commit:** —

> **Stack note:** `numx_sketch_rsvd` uses ~14 KB of internal local variables
> (bounded by `NUMX_MAX_SKETCH_M` / `NUMX_MAX_SKETCH_N` / `NUMX_MAX_SKETCH_RANK`).

### Test cases

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| rsvd rank1 rc | rc=0 | — | — | — |
| rsvd rank1 S[0]>0 | >0 | — | — | — |
| rsvd rank1 S[0]≈2 | 2.0 ±0.1 | — | — | — |
| rsvd rank2 rc | rc=0 | — | — | — |
| rsvd rank2 S[0]≥S[1] | S[0]≥S[1] | — | — | — |
| rsvd rank2 S[0]≈4 | 4.0 ±0.2 | — | — | — |
| rsvd rank2 S[1]≈2 | 2.0 ±0.2 | — | — | — |
| rsvd seed=0 rc | rc=0 | — | — | — |
| rsvd seed=0 S[0]≈3 | 3.0 ±0.15 | — | — | — |
| null-ptr guards | rc=-1 | — | — | — |
| m=0 / n=0 / rank=0 | rc=-2 | — | — | — |

**RESULTS: — PASS / — FAIL / — TOTAL**
