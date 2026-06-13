/*
 * Example 04 — Automatic differentiation.
 *
 * Demonstrates both modes on f(x, y) = x^2 * y + sin(x):
 *
 *  Forward-mode: compute df/dx at (x=1.5, y=2.0) using dual numbers.
 *    df/dx = 2*x*y + cos(x)  →  at (1.5, 2.0): 2*1.5*2 + cos(1.5) ≈ 6.0705
 *
 *  Reverse-mode: compute both df/dx and df/dy at (1.5, 2.0) in one backward pass.
 *    df/dy = x^2  →  at x=1.5: 1.5^2 = 2.25
 */

#include "numx/autodiff.h"
#include <stdio.h>

int main(void)
{
    /* ── Forward-mode: df/dx ─────────────────────────────────────── */
    {
        numx_dual_t x = numx_dual_var(1.5f);   /* differentiate w.r.t. x */
        numx_dual_t y = numx_dual_const(2.0f);

        /* f = x^2 * y + sin(x) */
        numx_dual_t x2   = numx_dual_mul(x, x);
        numx_dual_t x2y  = numx_dual_mul(x2, y);
        numx_dual_t sinx = numx_dual_sin(x);
        numx_dual_t f    = numx_dual_add(x2y, sinx);

        printf("Forward-mode:\n");
        printf("  f(1.5, 2.0) = %.6f  (expect ~5.4975)\n", (double)f.re);
        printf("  df/dx       = %.6f  (expect ~6.0705)\n", (double)f.du);
    }

    /* ── Reverse-mode: df/dx and df/dy in one pass ───────────────── */
    {
        numx_ad_tape_t tape;
        numx_size_t ix, iy, ix2, ix2y, isinx, iout;
        numx_status_t s;

        s = numx_ad_init(&tape);
        if (s != NUMX_OK) { printf("ad_init failed\n"); return 1; }

        s  = numx_ad_var(&tape, 1.5f, &ix);
        s |= numx_ad_var(&tape, 2.0f, &iy);
        s |= numx_ad_mul(&tape, ix, ix,   &ix2);
        s |= numx_ad_mul(&tape, ix2, iy,  &ix2y);
        s |= numx_ad_sin(&tape, ix,        &isinx);
        s |= numx_ad_add(&tape, ix2y, isinx, &iout);
        if (s != NUMX_OK) { printf("tape build failed\n"); return 1; }

        s = numx_ad_backward(&tape, iout);
        if (s != NUMX_OK) { printf("backward failed\n"); return 1; }

        printf("\nReverse-mode:\n");
        printf("  f(1.5, 2.0) = %.6f  (expect ~5.4975)\n",
               (double)numx_ad_val(&tape, iout));
        printf("  df/dx       = %.6f  (expect ~6.0705)\n",
               (double)numx_ad_grad(&tape, ix));
        printf("  df/dy       = %.6f  (expect  2.2500)\n",
               (double)numx_ad_grad(&tape, iy));
    }

    return 0;
}
