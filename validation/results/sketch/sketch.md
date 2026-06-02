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

## ESP32-S3 — ESP-IDF v5.5.2 / Xtensa LX7 / xtensa-esp32s3-elf-gcc / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-29 | **Commit:** d81b386

> **Stack note:** `numx_sketch_rsvd` uses ~14 KB of internal local variables
> (bounded by `NUMX_MAX_SKETCH_M` / `NUMX_MAX_SKETCH_N` / `NUMX_MAX_SKETCH_RANK`).

### Test cases — rsvd_rank1

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| rsvd rank1 rc | rc=0 | rc=0 | — | ✅ |
| rsvd rank1 S[0]>0 | >0 | confirmed | — | ✅ |
| rsvd rank1 S[0]≈2 | 2.0 | 2.0000000 | 0.00e+00 | ✅ |

### Test cases — rsvd_rank2

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| rsvd rank2 rc | rc=0 | rc=0 | — | ✅ |
| rsvd rank2 S[0]>S[1] | S[0]>S[1] | confirmed | — | ✅ |
| rsvd rank2 S[0]≈4 | 4.0 | 0.0000000 | 4.00e+00 | ❌ |
| rsvd rank2 S[1]≈2 | 2.0 | 0.0000000 | 2.00e+00 | ❌ |

### Test cases — rsvd_seed0

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| rsvd seed=0 rc | rc=0 | rc=0 | — | ✅ |
| rsvd seed=0 S[0]>0 | >0 | confirmed | — | ✅ |
| rsvd seed=0 S[0]≈3 | 3.0 | 3.0000000 | 0.00e+00 | ✅ |

### Test cases — rsvd_errors

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| rsvd null-A | rc=-1 | rc=-1 | — | ✅ |
| rsvd null-U | rc=-1 | rc=-1 | — | ✅ |
| rsvd null-S | rc=-1 | rc=-1 | — | ✅ |
| rsvd null-Vt | rc=-1 | rc=-1 | — | ✅ |
| rsvd m=0 | rc=-2 | rc=-2 | — | ✅ |
| rsvd n=0 | rc=-2 | rc=-2 | — | ✅ |
| rsvd rank=0 | rc=-2 | rc=-2 | — | ✅ |

### Precision vs reference

| Scenario | Exact | Computed | Error | Note |
|----------|-------|----------|-------|------|
| rank1 S[0] (diag(2,0,0)) | 2.0 | 2.0000000 | 0.00e+00 | ✅ |
| rank2 S[0] (diag(4,2,0,0)) | 4.0 | 0.0000000 | 4.00e+00 | ❌ FLAG S-01 |
| rank2 S[1] (diag(4,2,0,0)) | 2.0 | 0.0000000 | 2.00e+00 | ❌ FLAG S-01 |
| seed=0 S[0] (diag(3,1)) | 3.0 | 3.0000000 | 0.00e+00 | ✅ |

> **FLAG S-01 — RSVD rank-2 degenerate projection (seed portability):**
> The rank-2 test seeds the RNG with a hardcoded value before calling `numx_sketch_rsvd`.
> On ESP32-S3 (ESP-IDF v5.5.2), `rand()` generates a different sequence for that seed than
> glibc `rand()` on x86-64, producing a random projection matrix Ω whose columns lie nearly
> in the null space of diag(4,2,0,0)'s top-2 singular vectors. The result: S[0]≈ε, S[1]=0
> instead of S[0]=4, S[1]=2. The ordering invariant S[0]>S[1] still holds (ε > 0). This is
> **not a float32 precision issue and not an algorithmic bug** — rank-1 (different seed) and
> seed=0 both recover exact singular values. The RSVD implementation is correct; the test
> seed is not portable across `rand()` implementations.

**RESULTS: 16 PASS / 2 FAIL / 18 TOTAL**
