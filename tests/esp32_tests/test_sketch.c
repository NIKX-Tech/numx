#include "test_helpers.h"
#include "numx/sketch.h"

void test_sketch(numxt_ctx_t *ctx)
{
    /* ── rsvd: rank-1 diagonal matrix ───────────────────────────── */
    suite("sketch / rsvd_rank1");
    {
        /*
         * A = diag(2,0,0) as 3×3: only one non-zero singular value σ_1=2.
         * rank=1, oversample=2 → (rank+oversample)=3 ≤ min(3,3).
         * S[0] should converge to 2.0 regardless of random seed.
         */
        numx_real_t A[9] = {2,0,0, 0,0,0, 0,0,0};
        numx_real_t U[3], S[1], Vt[3]; /* U: 3×1, Vt: 1×3 */

        chk_rc  (ctx, "rsvd rank1 rc",
                 numx_sketch_rsvd(A, 3, 3, 1, 2, 1u, U, S, Vt), NUMX_OK);
        chk_true(ctx, "rsvd rank1 S[0]>0",  S[0] > (numx_real_t)0.0);
        chk_f   (ctx, "rsvd rank1 S[0]≈2",  S[0], (numx_real_t)2.0, (numx_real_t)0.1);
    }

    /* ── rsvd: rank-2 diagonal — singular values descending ──────── */
    suite("sketch / rsvd_rank2");
    {
        /*
         * A = diag(4,2,0,0) as 4×4: σ_1=4, σ_2=2.
         * rank=2, oversample=2 → (rank+oversample)=4 ≤ min(4,4).
         */
        numx_real_t A[16] = {4,0,0,0, 0,2,0,0, 0,0,0,0, 0,0,0,0};
        numx_real_t U[8], S[2], Vt[8]; /* U: 4×2, Vt: 2×4 */

        chk_rc  (ctx, "rsvd rank2 rc",
                 numx_sketch_rsvd(A, 4, 4, 2, 2, 1u, U, S, Vt), NUMX_OK);
        chk_true(ctx, "rsvd rank2 S[0]>S[1]", S[0] >= S[1]);
        chk_true(ctx, "rsvd rank2 S[1]>=0",   S[1] >= (numx_real_t)0.0);
        chk_f   (ctx, "rsvd rank2 S[0]≈4",    S[0], (numx_real_t)4.0, (numx_real_t)0.2);
        chk_f   (ctx, "rsvd rank2 S[1]≈2",    S[1], (numx_real_t)2.0, (numx_real_t)0.2);
    }

    /* ── rsvd: seed=0 (internally becomes 1) ────────────────────── */
    suite("sketch / rsvd_seed0");
    {
        numx_real_t A[4] = {3,0, 0,1}; /* 2×2 diagonal */
        numx_real_t U[2], S[1], Vt[2];

        chk_rc  (ctx, "rsvd seed=0 rc",
                 numx_sketch_rsvd(A, 2, 2, 1, 1, 0u, U, S, Vt), NUMX_OK);
        chk_true(ctx, "rsvd seed=0 S[0]>0", S[0] > (numx_real_t)0.0);
        chk_f   (ctx, "rsvd seed=0 S[0]≈3", S[0], (numx_real_t)3.0, (numx_real_t)0.15);
    }

    /* ── rsvd: error cases ───────────────────────────────────────── */
    suite("sketch / rsvd_errors");
    {
        numx_real_t A[4] = {1,0, 0,1};
        numx_real_t U[2], S[1], Vt[2];

        chk_rc(ctx, "rsvd null-A",
               numx_sketch_rsvd(NULL, 2, 2, 1, 0, 1u, U, S, Vt),  NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "rsvd null-U",
               numx_sketch_rsvd(A, 2, 2, 1, 0, 1u, NULL, S, Vt),  NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "rsvd null-S",
               numx_sketch_rsvd(A, 2, 2, 1, 0, 1u, U, NULL, Vt),  NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "rsvd null-Vt",
               numx_sketch_rsvd(A, 2, 2, 1, 0, 1u, U, S, NULL),   NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "rsvd m=0",
               numx_sketch_rsvd(A, 0, 2, 1, 0, 1u, U, S, Vt),     NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "rsvd n=0",
               numx_sketch_rsvd(A, 2, 0, 1, 0, 1u, U, S, Vt),     NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "rsvd rank=0",
               numx_sketch_rsvd(A, 2, 2, 0, 0, 1u, U, S, Vt),     NUMX_ERR_INVALID_ARG);
    }
}
