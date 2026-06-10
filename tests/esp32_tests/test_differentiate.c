#include "test_helpers.h"
#include "math_helpers.h"
#include "numx/differentiate.h"

void test_differentiate(numxt_ctx_t *ctx)
{
    numx_real_t r;

    suite("diff / forward");
    {
        chk_rc(ctx, "fwd rc", numx_diff_forward(f_x3,3.0f,1e-3f,&r), NUMX_OK);
        chk_f (ctx,"fwd x^3 at 3 = 27",   r, 27.0f, 1e-2f);

        numx_diff_forward(f_x3,2.0f,1e-3f,&r);
        chk_f (ctx,"fwd x^3 at 2 = 12",   r, 12.0f, 1e-2f);

        numx_diff_forward(f_one,5.0f,1e-3f,&r);
        chk_f (ctx,"fwd const = 0",       r, 0.0f,  1e-5f);

        numx_diff_forward(f_xlin,5.0f,1e-3f,&r);
        chk_f (ctx,"fwd linear = 1",      r, 1.0f,  1e-2f);

        numx_diff_forward(f_xsq,3.0f,1e-3f,&r);
        chk_f (ctx,"fwd x^2 at 3 = 6",   r, 6.0f,  1e-2f);

        chk_rc(ctx, "fwd null-f",   numx_diff_forward(NULL,1.0f,1e-3f,&r),    NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "fwd null-out", numx_diff_forward(f_x3,1.0f,1e-3f,NULL),  NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "fwd h=0",      numx_diff_forward(f_x3,1.0f,0.0f,&r),     NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "fwd h<0",      numx_diff_forward(f_x3,1.0f,-1e-3f,&r),   NUMX_ERR_INVALID_ARG);
    }

    suite("diff / central");
    {
        numx_diff_central(f_x3,3.0f,1e-3f,&r);
        chk_f (ctx,"central x^3 at 3 = 27", r, 27.0f, 1e-2f);

        numx_diff_central(f_xsq,-2.0f,1e-3f,&r);
        chk_f (ctx,"central x^2 at -2 = -4",r,-4.0f,  1e-3f);

        numx_diff_central(f_one,5.0f,1e-3f,&r);
        chk_f (ctx,"central const = 0",      r, 0.0f,  1e-5f);

        numx_diff_central(f_xlin,1.0f,1e-3f,&r);
        chk_f (ctx,"central linear = 1",     r, 1.0f,  1e-3f);

        numx_diff_central(f_xsq,3.0f,1e-3f,&r);
        chk_f (ctx,"central x^2 at 3 = 6",  r, 6.0f,  1e-3f);

        numx_diff_central(f_xcb,2.0f,1e-3f,&r);
        chk_f (ctx,"central x^3 at 2 = 12", r, 12.0f, 1e-2f);

        chk_rc(ctx, "central null-f",   numx_diff_central(NULL,1.0f,1e-3f,&r),   NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "central null-out", numx_diff_central(f_x3,1.0f,1e-3f,NULL), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "central h=0",      numx_diff_central(f_x3,1.0f,0.0f,&r),    NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "central h<0",      numx_diff_central(f_x3,1.0f,-1e-3f,&r),  NUMX_ERR_INVALID_ARG);
    }

    suite("diff / richardson");
    {
        numx_diff_richardson(f_x3,3.0f,1e-3f,&r);
        chk_f (ctx,"rich x^3 at 3 = 27", r, 27.0f, 1e-2f);

        numx_diff_richardson(f_x3,2.0f,1e-3f,&r);
        chk_f (ctx,"rich x^3 at 2 = 12", r, 12.0f, 1e-2f);

        numx_real_t rc2, rr2;
        numx_diff_central(f_xcb,2.0f,0.1f,&rc2);
        numx_diff_richardson(f_xcb,2.0f,0.1f,&rr2);
        chk_true(ctx, "rich more accurate than central (large h)",
                  numxt_absf(rr2-12.0f) < numxt_absf(rc2-12.0f));

        numx_diff_richardson(f_xsq,3.0f,1e-3f,&r);
        chk_f (ctx,"rich x^2 at 3 = 6",  r, 6.0f,  1e-3f);

        numx_diff_richardson(f_one,5.0f,1e-3f,&r);
        chk_f (ctx,"rich const = 0",      r, 0.0f,  1e-5f);

        chk_rc(ctx, "rich null-f",   numx_diff_richardson(NULL,1.0f,1e-3f,&r),   NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "rich null-out", numx_diff_richardson(f_x3,1.0f,1e-3f,NULL), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "rich h=0",      numx_diff_richardson(f_x3,1.0f,0.0f,&r),    NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "rich h<0",      numx_diff_richardson(f_x3,1.0f,-1e-3f,&r),  NUMX_ERR_INVALID_ARG);
    }
}
