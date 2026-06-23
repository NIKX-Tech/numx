#include <stdio.h>
#include "test_helpers.h"

void suite(const char *name)
{
    printf("\n── %s ─────────────────────────────────────\n", name);
}

void chk_f(numxt_ctx_t *ctx, const char *label,
           numx_real_t got, numx_real_t ref, numx_real_t tol)
{
    numx_real_t err = numxt_absf(got - ref);
    if (err <= tol) {
        printf("  PASS  %-48s  out=%.7f  err=%.2e\n", label, (double)got, (double)err);
        ctx->pass++;
    } else {
        printf("  FAIL  %-48s  out=%.7f  ref=%.7f  err=%.2e  tol=%.2e\n",
               label, (double)got, (double)ref, (double)err, (double)tol);
        ctx->fail++;
    }
}

void chk_rc(numxt_ctx_t *ctx, const char *label,
            numx_status_t got, numx_status_t exp)
{
    if (got == exp) {
        printf("  PASS  %-52s rc=%d\n", label, (int)got);
        ctx->pass++;
    } else {
        printf("  FAIL  %-52s got rc=%d expected=%d\n",
               label, (int)got, (int)exp);
        ctx->fail++;
    }
}

void chk_true(numxt_ctx_t *ctx, const char *label, int cond)
{
    if (cond) {
        printf("  PASS  %-52s\n", label);
        ctx->pass++;
    } else {
        printf("  FAIL  %-52s (condition false)\n", label);
        ctx->fail++;
    }
}
