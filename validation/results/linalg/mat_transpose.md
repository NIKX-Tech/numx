# Validation: numx_mat_transpose + numx_mat_transpose_sq

---

## ESP32-S3 — ESP-IDF v5.5.2 / Xtensa LX7 / xtensa-esp32s3-elf-gcc / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-29 | **Commit:** d81b386

### Test cases — mat_transpose (out-of-place, 2×3 → 3×2)

Input: A = [[1,2,3],[4,5,6]] (row-major: [1,2,3,4,5,6])
Expected Aᵀ (3×2, row-major): [1,4,2,5,3,6]

| Element | Expected | Computed | Error | Pass |
|---------|----------|----------|-------|------|
| AT[0] | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| AT[1] | 4.0 | 4.0000000 | 0.00e+00 | ✅ |
| AT[2] | 2.0 | 2.0000000 | 0.00e+00 | ✅ |
| AT[3] | 5.0 | 5.0000000 | 0.00e+00 | ✅ |
| AT[4] | 3.0 | 3.0000000 | 0.00e+00 | ✅ |
| AT[5] | 6.0 | 6.0000000 | 0.00e+00 | ✅ |
| T(T(A)) = A | identity | confirmed | — | ✅ |
| null ptr | rc=-1 | rc=-1 | — | ✅ |

### Test cases — mat_transpose_sq (in-place, 2×2)

Input: S = [[1,2],[3,4]] (row-major: [1,2,3,4])
Expected Sᵀ (in-place): [1,3,2,4]

| Element | Expected | Computed | Error | Pass |
|---------|----------|----------|-------|------|
| S[0] | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| S[1] | 3.0 | 3.0000000 | 0.00e+00 | ✅ |
| S[2] | 2.0 | 2.0000000 | 0.00e+00 | ✅ |
| S[3] | 4.0 | 4.0000000 | 0.00e+00 | ✅ |
| T(T(S)) = S | identity | confirmed | — | ✅ |
| null ptr | rc=-1 | rc=-1 | — | ✅ |

**RESULTS: 16 PASS / 0 FAIL / 16 TOTAL**
