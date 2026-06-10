#include "test_helpers.h"
#include "math_helpers.h"
#include "numx/roots.h"

void test_roots(numxt_ctx_t *ctx)
{
    numx_real_t r;

    suite("roots / bisect");
    {
        chk_rc(ctx, "bisect linear rc", numx_root_bisect(f_lin,0,5,1e-5f,&r), NUMX_OK);
        chk_f (ctx,"bisect linear root=2",     r, 2.0f, 1e-5f);

        numx_root_bisect(f_quad,0,5,1e-5f,&r);
        chk_f (ctx,"bisect quad+ root=2",      r, 2.0f, 1e-5f);

        numx_root_bisect(f_quad,-5,0,1e-5f,&r);
        chk_f (ctx,"bisect quad- root=-2",     r,-2.0f, 1e-5f);

        numx_root_bisect(f_cubic,0.5f,2.0f,1e-5f,&r);
        chk_true(ctx, "bisect cubic residual",  numxt_absf(f_cubic(r)) < 1e-3f);

        numx_root_bisect(f_lin,2,5,1e-5f,&r);
        chk_f (ctx,"bisect root@left=2",       r, 2.0f, 1e-5f);

        numx_root_bisect(f_lin,0,2,1e-5f,&r);
        chk_f (ctx,"bisect root@right=2",      r, 2.0f, 1e-5f);

        chk_rc(ctx, "bisect null-f",    numx_root_bisect(NULL,0,5,1e-5f,&r),   NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "bisect null-out",  numx_root_bisect(f_lin,0,5,1e-5f,NULL),NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "bisect no-bracket",numx_root_bisect(f_quad,3,5,1e-5f,&r), NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "bisect tol=0",     numx_root_bisect(f_lin,0,5,0.0f,&r),   NUMX_ERR_INVALID_ARG);
    }

    suite("roots / newton");
    {
        chk_rc(ctx, "newton linear rc", numx_root_newton(f_lin,df_lin,0,1e-5f,&r), NUMX_OK);
        chk_f (ctx,"newton linear=2",   r, 2.0f, 1e-5f);

        numx_root_newton(f_quad,df_quad,3,1e-5f,&r);
        chk_f (ctx,"newton quad+=2",    r, 2.0f, 1e-5f);

        numx_root_newton(f_lin,df_lin,2.0f,1e-5f,&r);
        chk_f (ctx,"newton at-root=2",  r, 2.0f, 1e-5f);

        numx_root_newton(f_cubic,df_cubic,2,1e-5f,&r);
        chk_true(ctx, "newton cubic residual", numxt_absf(f_cubic(r)) < 1e-3f);

        numx_status_t st = numx_root_newton(f_dbl,df_dbl,1.0f,1e-5f,&r);
        chk_true(ctx, "newton double-root ok/sing", st==NUMX_OK || st==NUMX_ERR_SINGULAR);

        chk_rc(ctx, "newton null-f",  numx_root_newton(NULL,df_lin,0,1e-5f,&r),    NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "newton null-df", numx_root_newton(f_lin,NULL,0,1e-5f,&r),     NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "newton tol<0",   numx_root_newton(f_lin,df_lin,0,-1.0f,&r),   NUMX_ERR_INVALID_ARG);
    }

    suite("roots / brent");
    {
        chk_rc(ctx, "brent linear rc", numx_root_brent(f_lin,0,5,1e-5f,&r), NUMX_OK);
        chk_f (ctx,"brent linear=2",       r, 2.0f, 1e-5f);

        numx_root_brent(f_quad,0,5,1e-5f,&r);
        chk_f (ctx,"brent quad+=2",        r, 2.0f, 1e-5f);

        numx_root_brent(f_cubic,0.5f,1.5f,1e-5f,&r);
        chk_f (ctx,"brent cubic root=1",   r, 1.0f, 1e-5f);

        numx_root_brent(f_cubic,-1.5f,-0.5f,1e-5f,&r);
        chk_true(ctx, "brent cubic- residual", numxt_absf(f_cubic(r)) < 1e-3f);

        chk_rc(ctx, "brent null-f",    numx_root_brent(NULL,0,5,1e-5f,&r),    NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "brent null-out",  numx_root_brent(f_lin,0,5,1e-5f,NULL), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "brent no-bracket",numx_root_brent(f_quad,3,5,1e-5f,&r),  NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "brent tol=0",     numx_root_brent(f_lin,0,5,0.0f,&r),    NUMX_ERR_INVALID_ARG);
    }
}
