#include "test_helpers.h"
#include "math_helpers.h"
#include "numx/differentiate.h"

void test_differentiate(numxt_ctx_t *ctx)
{
    numx_real_t r;

    suite("diff / forward");
    {
        chk_rc(ctx, "fwd rc", numx_diff_forward(f_x3,(numx_real_t)3.0,(numx_real_t)1e-3,&r), NUMX_OK);
        chk_f (ctx,"fwd x^3 at 3 = 27",   r, (numx_real_t)27.0, (numx_real_t)1e-2);

        numx_diff_forward(f_x3,(numx_real_t)2.0,(numx_real_t)1e-3,&r);
        chk_f (ctx,"fwd x^3 at 2 = 12",   r, (numx_real_t)12.0, (numx_real_t)1e-2);

        numx_diff_forward(f_one,(numx_real_t)5.0,(numx_real_t)1e-3,&r);
        chk_f (ctx,"fwd const = 0",       r, (numx_real_t)0.0,  (numx_real_t)1e-5);

        numx_diff_forward(f_xlin,(numx_real_t)5.0,(numx_real_t)1e-3,&r);
        chk_f (ctx,"fwd linear = 1",      r, (numx_real_t)1.0,  (numx_real_t)1e-2);

        numx_diff_forward(f_xsq,(numx_real_t)3.0,(numx_real_t)1e-3,&r);
        chk_f (ctx,"fwd x^2 at 3 = 6",   r, (numx_real_t)6.0,  (numx_real_t)1e-2);

        chk_rc(ctx, "fwd null-f",   numx_diff_forward(NULL,(numx_real_t)1.0,(numx_real_t)1e-3,&r),    NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "fwd null-out", numx_diff_forward(f_x3,(numx_real_t)1.0,(numx_real_t)1e-3,NULL),  NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "fwd h=0",      numx_diff_forward(f_x3,(numx_real_t)1.0,(numx_real_t)0.0,&r),     NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "fwd h<0",      numx_diff_forward(f_x3,(numx_real_t)1.0,-(numx_real_t)1e-3,&r),   NUMX_ERR_INVALID_ARG);
    }

    suite("diff / central");
    {
        numx_diff_central(f_x3,(numx_real_t)3.0,(numx_real_t)1e-3,&r);
        chk_f (ctx,"central x^3 at 3 = 27", r, (numx_real_t)27.0, (numx_real_t)1e-2);

        numx_diff_central(f_xsq,-(numx_real_t)2.0,(numx_real_t)1e-3,&r);
        chk_f (ctx,"central x^2 at -2 = -4",r,-(numx_real_t)4.0,  (numx_real_t)1e-3);

        numx_diff_central(f_one,(numx_real_t)5.0,(numx_real_t)1e-3,&r);
        chk_f (ctx,"central const = 0",      r, (numx_real_t)0.0,  (numx_real_t)1e-5);

        numx_diff_central(f_xlin,(numx_real_t)1.0,(numx_real_t)1e-3,&r);
        chk_f (ctx,"central linear = 1",     r, (numx_real_t)1.0,  (numx_real_t)1e-3);

        numx_diff_central(f_xsq,(numx_real_t)3.0,(numx_real_t)1e-3,&r);
        chk_f (ctx,"central x^2 at 3 = 6",  r, (numx_real_t)6.0,  (numx_real_t)1e-3);

        numx_diff_central(f_xcb,(numx_real_t)2.0,(numx_real_t)1e-3,&r);
        chk_f (ctx,"central x^3 at 2 = 12", r, (numx_real_t)12.0, (numx_real_t)1e-2);

        chk_rc(ctx, "central null-f",   numx_diff_central(NULL,(numx_real_t)1.0,(numx_real_t)1e-3,&r),   NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "central null-out", numx_diff_central(f_x3,(numx_real_t)1.0,(numx_real_t)1e-3,NULL), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "central h=0",      numx_diff_central(f_x3,(numx_real_t)1.0,(numx_real_t)0.0,&r),    NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "central h<0",      numx_diff_central(f_x3,(numx_real_t)1.0,-(numx_real_t)1e-3,&r),  NUMX_ERR_INVALID_ARG);
    }

    suite("diff / richardson");
    {
        numx_diff_richardson(f_x3,(numx_real_t)3.0,(numx_real_t)1e-3,&r);
        chk_f (ctx,"rich x^3 at 3 = 27", r, (numx_real_t)27.0, (numx_real_t)1e-2);

        numx_diff_richardson(f_x3,(numx_real_t)2.0,(numx_real_t)1e-3,&r);
        chk_f (ctx,"rich x^3 at 2 = 12", r, (numx_real_t)12.0, (numx_real_t)1e-2);

        numx_real_t rc2, rr2;
        numx_diff_central(f_xcb,(numx_real_t)2.0,(numx_real_t)0.1,&rc2);
        numx_diff_richardson(f_xcb,(numx_real_t)2.0,(numx_real_t)0.1,&rr2);
        chk_true(ctx, "rich more accurate than central (large h)",
                  numxt_absf(rr2-(numx_real_t)12.0) < numxt_absf(rc2-(numx_real_t)12.0));

        numx_diff_richardson(f_xsq,(numx_real_t)3.0,(numx_real_t)1e-3,&r);
        chk_f (ctx,"rich x^2 at 3 = 6",  r, (numx_real_t)6.0,  (numx_real_t)1e-3);

        numx_diff_richardson(f_one,(numx_real_t)5.0,(numx_real_t)1e-3,&r);
        chk_f (ctx,"rich const = 0",      r, (numx_real_t)0.0,  (numx_real_t)1e-5);

        chk_rc(ctx, "rich null-f",   numx_diff_richardson(NULL,(numx_real_t)1.0,(numx_real_t)1e-3,&r),   NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "rich null-out", numx_diff_richardson(f_x3,(numx_real_t)1.0,(numx_real_t)1e-3,NULL), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "rich h=0",      numx_diff_richardson(f_x3,(numx_real_t)1.0,(numx_real_t)0.0,&r),    NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "rich h<0",      numx_diff_richardson(f_x3,(numx_real_t)1.0,-(numx_real_t)1e-3,&r),  NUMX_ERR_INVALID_ARG);
    }
}
