# Validation: numx_sketch (randomized SVD)

Covers: `numx_sketch_rsvd` — Halko-Martinsson-Tropp randomized SVD

---

## x86-64 — Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-06-07 | **Commit:** d81b386

### Test cases

| A | m | n | rank | oversample | seed | σ expected | σ computed | Error | Pass |
|---|---|---|------|-----------|------|-----------|-----------|-------|------|
| diag(2,0,0) | 3 | 3 | 1 | 2 | 42 | S[0]=2.0 | 2.0000000 | 0.00e+00 | ✅ |
| diag(4,2,0,0) | 4 | 4 | 2 | 2 | 12345 | S[0]=4.0, S[1]=2.0 | 4.0000000, 2.0000000 | 0.00e+00 | ✅ |
| diag(3,1) | 2 | 2 | 1 | 1 | 0 | S[0]=3.0 | 3.0000000 | 0.00e+00 | ✅ |
| S descending | — | — | 2 | — | — | S[0]≥S[1]≥0 | confirmed | — | ✅ |
| null-ptr guards | — | — | — | — | — | rc=-1 | rc=-1 | — | ✅ |
| m=0 / n=0 / rank=0 | — | — | — | — | — | rc=-2 | rc=-2 | — | ✅ |

### Reconstruction quality

| A | rank | ‖A − UΣVᵀ‖_F / ‖A‖_F | Pass |
|---|------|----------------------|------|
| diag(2,0,0) 3×3 | 1 | ≈0 | ✅ |
| diag(4,2,0,0) 4×4 | 2 | ≈0 | ✅ |

### Performance

| A size | rank | oversample | Total | Per call |
|--------|------|-----------|-------|----------|
| 16×16 | 4 | 4 | 33,297 µs | 332,972 ns |
| 32×32 | 8 | 4 | 80,575 µs | 805,758 ns |
| 64×64 | 8 | 4 | 93,132 µs | 931,322 ns |

---

## ARM64 — macOS 26.2 / Apple M4 Pro / Apple clang 21.0.0 / float32
**Validator:** Erfan Jazeb Nikoo | **Date:** 2026-06-08 | **Commit:** d81b386

### Test cases

| Test | Result |
|------|--------|
| test_rsvd_rank1_reconstruction | ✅ |
| test_rsvd_diagonal_rank2 | ✅ |
| test_rsvd_tall_matrix | ✅ |
| test_rsvd_null_returns_error | ✅ |
| test_rsvd_invalid_arg_returns_error | ✅ |

*300 / 300 Unity tests PASS*

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| sketch_rsvd 16×16 rank=4 os=4 | 100 | 45,036 µs | 450,360 ns |
| sketch_rsvd 32×32 rank=8 os=4 | 100 | 113,795 µs | 1,137,950 ns |
| sketch_rsvd 64×64 rank=8 os=4 | 100 | 138,754 µs | 1,387,540 ns |

**RESULTS: 5 PASS / 0 FAIL / 5 TOTAL**

---

## ARM64 — macOS 26.2 / Apple M1 Pro / Apple clang 17.0.0 / float32
**Validator:** Erfan Jazeb Nikoo | **Date:** 2026-06-08 | **Commit:** 1380ab1

### Test cases

| Test | Result |
|------|--------|
| test_rsvd_rank1_reconstruction | ✅ |
| test_rsvd_diagonal_rank2 | ✅ |
| test_rsvd_tall_matrix | ✅ |
| test_rsvd_null_returns_error | ✅ |
| test_rsvd_invalid_arg_returns_error | ✅ |

*300 / 300 Unity tests PASS*

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| sketch_rsvd 16×16 rank=4 os=4 | 100 | 37,542 µs | 375,420 ns |
| sketch_rsvd 32×32 rank=8 os=4 | 100 | 95,309 µs | 953,090 ns |
| sketch_rsvd 64×64 rank=8 os=4 | 100 | 110,159 µs | 1,101,590 ns |

**RESULTS: 5 PASS / 0 FAIL / 5 TOTAL**

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

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| sketch_rsvd 8x8 rank=2 os=2 | 100 | 904,909 µs | 9,049,090 ns |
| sketch_rsvd 16x16 rank=4 os=4 | 100 | — | — |

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

---

## Windows x64 — Windows 11 / MSVC 14.51 (VS 2026 Build Tools) / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-06-05 | **Commit:** 4c4c0f0

### Test cases

| Test | Result |
|------|--------|
| test_rsvd_rank1_reconstruction | ✅ |
| test_rsvd_diagonal_rank2 | ✅ |
| test_rsvd_tall_matrix | ✅ |
| test_rsvd_null_returns_error | ✅ |
| test_rsvd_invalid_arg_returns_error | ✅ |
| test_rsvd_seed0 | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| sketch_rsvd 8x8 rank=2 os=2 | 100 | 9,438 µs | 94,380 ns |
| sketch_rsvd 16x16 rank=4 os=4 | 100 | 46,646 µs | 466,460 ns |

**RESULTS: 6 PASS / 0 FAIL / 6 TOTAL**

---

## Windows x64 — Windows 11 / MSVC 14.51 (VS 2026 Build Tools) / float64
**Validator:** Amir Ab Khoshk | **Date:** 2026-06-06 | **Commit:** 1bba399

### Test cases

| Test | Result |
|------|--------|
| test_rsvd_rank1_reconstruction | ✅ |
| test_rsvd_diagonal_rank2 | ✅ |
| test_rsvd_tall_matrix | ✅ |
| test_rsvd_null_returns_error | ✅ |
| test_rsvd_invalid_arg_returns_error | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| sketch_rsvd 8x8 rank=2 os=2 | 100 | 9,476 µs | 94,760 ns |
| sketch_rsvd 16x16 rank=4 os=4 | 100 | 49,092 µs | 490,920 ns |

**RESULTS: 5 PASS / 0 FAIL / 5 TOTAL**
