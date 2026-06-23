#include "test_helpers.h"
#include "math_helpers.h"
#include "numx/roots.h"

void test_roots(numxt_ctx_t *ctx)
{
    numx_real_t r;

    suite("roots / bisect");
    {
        chk_rc(ctx, "bisect linear rc", numx_root_bisect(f_lin,0,5,(numx_real_t)1e-5,&r), NUMX_OK);
        chk_f (ctx,"bisect linear root=2",     r, (numx_real_t)2.0, (numx_real_t)1e-5);

        numx_root_bisect(f_quad,0,5,(numx_real_t)1e-5,&r);
        chk_f (ctx,"bisect quad+ root=2",      r, (numx_real_t)2.0, (numx_real_t)1e-5);

        numx_root_bisect(f_quad,-5,0,(numx_real_t)1e-5,&r);
        chk_f (ctx,"bisect quad- root=-2",     r,-(numx_real_t)2.0, (numx_real_t)1e-5);

        numx_root_bisect(f_cubic,(numx_real_t)0.5,(numx_real_t)2.0,(numx_real_t)1e-5,&r);
        chk_true(ctx, "bisect cubic residual",  numxt_absf(f_cubic(r)) < (numx_real_t)1e-3);

        numx_root_bisect(f_lin,2,5,(numx_real_t)1e-5,&r);
        chk_f (ctx,"bisect root@left=2",       r, (numx_real_t)2.0, (numx_real_t)1e-5);

        numx_root_bisect(f_lin,0,2,(numx_real_t)1e-5,&r);
        chk_f (ctx,"bisect root@right=2",      r, (numx_real_t)2.0, (numx_real_t)1e-5);

        chk_rc(ctx, "bisect null-f",    numx_root_bisect(NULL,0,5,(numx_real_t)1e-5,&r),   NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "bisect null-out",  numx_root_bisect(f_lin,0,5,(numx_real_t)1e-5,NULL),NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "bisect no-bracket",numx_root_bisect(f_quad,3,5,(numx_real_t)1e-5,&r), NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "bisect tol=0",     numx_root_bisect(f_lin,0,5,(numx_real_t)0.0,&r),   NUMX_ERR_INVALID_ARG);
    }

    suite("roots / newton");
    {
        chk_rc(ctx, "newton linear rc", numx_root_newton(f_lin,df_lin,0,(numx_real_t)1e-5,&r), NUMX_OK);
        chk_f (ctx,"newton linear=2",   r, (numx_real_t)2.0, (numx_real_t)1e-5);

        numx_root_newton(f_quad,df_quad,3,(numx_real_t)1e-5,&r);
        chk_f (ctx,"newton quad+=2",    r, (numx_real_t)2.0, (numx_real_t)1e-5);

        numx_root_newton(f_lin,df_lin,(numx_real_t)2.0,(numx_real_t)1e-5,&r);
        chk_f (ctx,"newton at-root=2",  r, (numx_real_t)2.0, (numx_real_t)1e-5);

        numx_root_newton(f_cubic,df_cubic,2,(numx_real_t)1e-5,&r);
        chk_true(ctx, "newton cubic residual", numxt_absf(f_cubic(r)) < (numx_real_t)1e-3);

        numx_status_t st = numx_root_newton(f_dbl,df_dbl,(numx_real_t)1.0,(numx_real_t)1e-5,&r);
        chk_true(ctx, "newton double-root ok/sing", st==NUMX_OK || st==NUMX_ERR_SINGULAR);

        chk_rc(ctx, "newton null-f",  numx_root_newton(NULL,df_lin,0,(numx_real_t)1e-5,&r),    NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "newton null-df", numx_root_newton(f_lin,NULL,0,(numx_real_t)1e-5,&r),     NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "newton tol<0",   numx_root_newton(f_lin,df_lin,0,-(numx_real_t)1.0,&r),   NUMX_ERR_INVALID_ARG);
    }

    suite("roots / brent");
    {
        chk_rc(ctx, "brent linear rc", numx_root_brent(f_lin,0,5,(numx_real_t)1e-5,&r), NUMX_OK);
        chk_f (ctx,"brent linear=2",       r, (numx_real_t)2.0, (numx_real_t)1e-5);

        numx_root_brent(f_quad,0,5,(numx_real_t)1e-5,&r);
        chk_f (ctx,"brent quad+=2",        r, (numx_real_t)2.0, (numx_real_t)1e-5);

        numx_root_brent(f_cubic,(numx_real_t)0.5,(numx_real_t)1.5,(numx_real_t)1e-5,&r);
        chk_f (ctx,"brent cubic root=1",   r, (numx_real_t)1.0, (numx_real_t)1e-5);

        numx_root_brent(f_cubic,-(numx_real_t)1.5,-(numx_real_t)0.5,(numx_real_t)1e-5,&r);
        chk_true(ctx, "brent cubic- residual", numxt_absf(f_cubic(r)) < (numx_real_t)1e-3);

        chk_rc(ctx, "brent null-f",    numx_root_brent(NULL,0,5,(numx_real_t)1e-5,&r),    NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "brent null-out",  numx_root_brent(f_lin,0,5,(numx_real_t)1e-5,NULL), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "brent no-bracket",numx_root_brent(f_quad,3,5,(numx_real_t)1e-5,&r),  NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "brent tol=0",     numx_root_brent(f_lin,0,5,(numx_real_t)0.0,&r),    NUMX_ERR_INVALID_ARG);
    }
}
