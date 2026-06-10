#include "test_helpers.h"
#include "numx/compressed_sensing.h"

/*
 * Stack note: numx_cs_omp uses ~9 KB of local variables (configurable via
 * NUMX_MAX_CS_MEASUREMENTS / NUMX_MAX_CS_SPARSITY in numx_config.h).
 * Ensure CONFIG_ESP_MAIN_TASK_STACK_SIZE >= 16384 in sdkconfig.
 */

void test_compressed_sensing(numxt_ctx_t *ctx)
{
    /* ── spectral_norm ───────────────────────────────────────────── */
    suite("cs / spectral_norm");
    {
        /* 3×3 identity: σ_max = 1 */
        numx_real_t I3[9] = {1,0,0, 0,1,0, 0,0,1};
        numx_real_t sigma;
        chk_rc(ctx, "spectral_norm I3 rc",
               numx_cs_spectral_norm(I3, 3, 3, 32, &sigma), NUMX_OK);
        chk_f (ctx, "spectral_norm I3=1.0", sigma, 1.0f, 5e-3f);

        /* 3×3 diagonal [3,1,1]: σ_max = 3 */
        numx_real_t D3[9] = {3,0,0, 0,1,0, 0,0,1};
        numx_cs_spectral_norm(D3, 3, 3, 64, &sigma);
        chk_f (ctx, "spectral_norm D3=3.0", sigma, 3.0f, 5e-2f);

        chk_rc(ctx, "spectral_norm null-A",
               numx_cs_spectral_norm(NULL, 3, 3, 32, &sigma), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "spectral_norm null-out",
               numx_cs_spectral_norm(I3, 3, 3, 32, NULL),     NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "spectral_norm m=0",
               numx_cs_spectral_norm(I3, 0, 3, 32, &sigma),   NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "spectral_norm n=0",
               numx_cs_spectral_norm(I3, 3, 0, 32, &sigma),   NUMX_ERR_INVALID_ARG);
    }

    /* ── omp ─────────────────────────────────────────────────────── */
    suite("cs / omp");
    {
        /*
         * A = I_4, y = [3,0,0,0], k=1.
         * OMP should select column 0 and recover x = [3,0,0,0].
         */
        numx_real_t A[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
        numx_real_t y[4]  = {3,0,0,0};
        numx_real_t x[4]  = {0,0,0,0};

        chk_rc(ctx, "omp identity rc",
               numx_cs_omp(A, y, 4, 4, 1, x), NUMX_OK);
        chk_f (ctx, "omp identity x[0]=3", x[0], 3.0f, 5e-3f);
        chk_f (ctx, "omp identity x[1]=0", x[1], 0.0f, 5e-3f);
        chk_f (ctx, "omp identity x[2]=0", x[2], 0.0f, 5e-3f);
        chk_f (ctx, "omp identity x[3]=0", x[3], 0.0f, 5e-3f);

        /* 2-sparse recovery: y = [2,0,5,0], k=2 → x=[2,0,5,0] */
        numx_real_t y2[4] = {2,0,5,0}, x2[4] = {0,0,0,0};
        numx_cs_omp(A, y2, 4, 4, 2, x2);
        chk_f (ctx, "omp 2-sparse x[0]=2", x2[0], 2.0f, 5e-3f);
        chk_f (ctx, "omp 2-sparse x[2]=5", x2[2], 5.0f, 5e-3f);

        chk_rc(ctx, "omp null-A",
               numx_cs_omp(NULL, y, 4, 4, 1, x),  NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "omp null-y",
               numx_cs_omp(A, NULL, 4, 4, 1, x),  NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "omp null-x",
               numx_cs_omp(A, y, 4, 4, 1, NULL),   NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "omp m=0",
               numx_cs_omp(A, y, 0, 4, 1, x),      NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "omp n=0",
               numx_cs_omp(A, y, 4, 0, 1, x),      NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "omp k=0",
               numx_cs_omp(A, y, 4, 4, 0, x),      NUMX_ERR_INVALID_ARG);
    }

    /* ── ista ────────────────────────────────────────────────────── */
    suite("cs / ista");
    {
        /*
         * A = I_4, y = [5,0,0,0], lambda=0.5, step=0.9 (< 1/σ_max^2 = 1).
         * LASSO solution: x* = soft_threshold(y, lambda) = [4.5,0,0,0].
         */
        numx_real_t A[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
        numx_real_t y[4]  = {5,0,0,0};
        numx_real_t x[4]  = {0,0,0,0};

        chk_rc(ctx, "ista identity rc",
               numx_cs_ista(A, y, 4, 4, 0.5f, 0.9f, 1000, x), NUMX_OK);
        chk_f (ctx, "ista identity x[0]≈4.5", x[0], 4.5f, 0.05f);
        chk_f (ctx, "ista identity x[1]=0",   x[1], 0.0f, 1e-3f);
        chk_f (ctx, "ista identity x[2]=0",   x[2], 0.0f, 1e-3f);

        numx_real_t xb[4] = {0};
        chk_rc(ctx, "ista null-A",
               numx_cs_ista(NULL, y, 4, 4, 0.5f, 0.9f, 100, xb), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "ista null-y",
               numx_cs_ista(A, NULL, 4, 4, 0.5f, 0.9f, 100, xb), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "ista null-x",
               numx_cs_ista(A, y, 4, 4, 0.5f, 0.9f, 100, NULL),  NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "ista step=0",
               numx_cs_ista(A, y, 4, 4, 0.5f, 0.0f, 100, xb),    NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "ista step<0",
               numx_cs_ista(A, y, 4, 4, 0.5f, -0.1f, 100, xb),   NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "ista lambda<0",
               numx_cs_ista(A, y, 4, 4, -0.1f, 0.9f, 100, xb),   NUMX_ERR_INVALID_ARG);
    }
}
