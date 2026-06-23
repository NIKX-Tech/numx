#include "test_helpers.h"
#include "math_helpers.h"
#include "numx/integrate.h"

void test_integrate(numxt_ctx_t *ctx)
{
    numx_real_t r;

    suite("integrate / trap");
    {
        chk_rc(ctx, "trap const rc", numx_integrate_trap(f_one,0,1,100,&r), NUMX_OK);
        chk_f (ctx,"trap const=1",   r, (numx_real_t)1.0, (numx_real_t)1e-3);

        numx_integrate_trap(f_xlin,0,1,100,&r);
        chk_f (ctx,"trap linear=0.5",r, (numx_real_t)0.5, (numx_real_t)1e-3);

        numx_integrate_trap(f_xsq,0,1,1000,&r);
        chk_f (ctx,"trap x^2=1/3",   r, (numx_real_t)1.0/(numx_real_t)3.0, (numx_real_t)1e-3);

        numx_real_t r1,r2;
        numx_integrate_trap(f_xlin,0,2,100,&r1);
        numx_integrate_trap(f_neg_x,0,2,100,&r2);
        chk_f (ctx,"trap linearity", r1+r2, (numx_real_t)0.0, (numx_real_t)1e-3);

        numx_integrate_trap(f_one,0,1,1,&r);
        chk_f (ctx,"trap n=1",       r, (numx_real_t)1.0, (numx_real_t)1e-3);

        chk_rc(ctx, "trap null-f",   numx_integrate_trap(NULL,0,1,10,&r),    NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "trap null-out", numx_integrate_trap(f_one,0,1,10,NULL), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "trap a>b",      numx_integrate_trap(f_one,1,0,10,&r),   NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "trap a=b",      numx_integrate_trap(f_one,1,1,10,&r),   NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "trap n=0",      numx_integrate_trap(f_one,0,1,0,&r),    NUMX_ERR_INVALID_ARG);
    }

    suite("integrate / simpson");
    {
        chk_rc(ctx, "simp const rc", numx_integrate_simpson(f_one,0,1,2,&r), NUMX_OK);
        chk_f (ctx,"simp const=1",      r, (numx_real_t)1.0,       (numx_real_t)1e-5);

        numx_integrate_simpson(f_xlin,0,1,2,&r);
        chk_f (ctx,"simp linear=0.5",   r, (numx_real_t)0.5,       (numx_real_t)1e-5);

        numx_integrate_simpson(f_xsq,0,1,4,&r);
        chk_f (ctx,"simp x^2=1/3",      r, (numx_real_t)1.0/(numx_real_t)3.0, (numx_real_t)1e-5);

        numx_integrate_simpson(f_xcb,0,1,4,&r);
        chk_f (ctx,"simp x^3=0.25",     r, (numx_real_t)0.25,      (numx_real_t)1e-5);

        numx_integrate_simpson(f_xsq,0,3,6,&r);
        chk_f (ctx,"simp x^2 [0,3]=9",  r, (numx_real_t)9.0,       (numx_real_t)1e-4);

        chk_rc(ctx, "simp odd-n rejected",  numx_integrate_simpson(f_one,0,1,3,&r),  NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "simp n<2 rejected",    numx_integrate_simpson(f_one,0,1,0,&r),  NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "simp null-f",          numx_integrate_simpson(NULL,0,1,4,&r),   NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "simp null-out",        numx_integrate_simpson(f_one,0,1,4,NULL),NUMX_ERR_NULL_PTR);
    }

    suite("integrate / gauss");
    {
        chk_rc(ctx, "gauss2 linear rc", numx_integrate_gauss(f_xlin,0,1,2,&r), NUMX_OK);
        chk_f (ctx,"gauss2 linear=0.5",r, (numx_real_t)0.5,       (numx_real_t)1e-5);

        numx_integrate_gauss(f_xsq,0,1,4,&r);
        chk_f (ctx,"gauss4 x^2=1/3",   r, (numx_real_t)1.0/(numx_real_t)3.0, (numx_real_t)1e-5);

        numx_integrate_gauss(f_xcb,0,1,8,&r);
        chk_f (ctx,"gauss8 x^3=0.25",  r, (numx_real_t)0.25,      (numx_real_t)1e-5);

        numx_integrate_gauss(f_one,-3,3,8,&r);
        chk_f (ctx,"gauss8 const [-3,3]=6", r, (numx_real_t)6.0,  (numx_real_t)1e-5);

        chk_rc(ctx, "gauss null-f",       numx_integrate_gauss(NULL,0,1,4,&r),    NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "gauss null-out",     numx_integrate_gauss(f_one,0,1,4,NULL), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "gauss bad-npts=3",   numx_integrate_gauss(f_one,0,1,3,&r),   NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "gauss bad-npts=16",  numx_integrate_gauss(f_one,0,1,16,&r),  NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "gauss a>b",          numx_integrate_gauss(f_one,1,0,4,&r),   NUMX_ERR_INVALID_ARG);
    }
}
