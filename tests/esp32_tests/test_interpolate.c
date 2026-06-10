#include "test_helpers.h"
#include "math_helpers.h"
#include "numx/interpolate.h"

void test_interpolate(numxt_ctx_t *ctx)
{
    numx_real_t r;

    suite("interp / linear");
    {
        numx_real_t xs[3]={0,1,2}, ys[3]={0,1,4};
        chk_rc(ctx, "linear rc", numx_interp_linear(xs,ys,3,0.5f,&r), NUMX_OK);
        chk_f (ctx,"linear x=0.5 → 0.5",  r, 0.5f, 1e-5f);

        numx_interp_linear(xs,ys,3,1.5f,&r);
        chk_f (ctx,"linear x=1.5 → 2.5",  r, 2.5f, 1e-5f);

        numx_real_t xs4[4]={0,1,3,6}, ys4[4]={1,3,2,5};
        int ok=1;
        for(int i=0;i<4;i++){
            numx_interp_linear(xs4,ys4,4,xs4[i],&r);
            if(numxt_absf(r-ys4[i])>1e-4f){ok=0;break;}
        }
        chk_true(ctx, "linear at knots", ok);

        numx_real_t xsc[3]={1,2,3}, ysc[3]={10,20,30};
        numx_interp_linear(xsc,ysc,3,0.0f,&r);
        chk_f (ctx,"linear clamp-below", r, 10.0f, 1e-5f);
        numx_interp_linear(xsc,ysc,3,5.0f,&r);
        chk_f (ctx,"linear clamp-above", r, 30.0f, 1e-5f);

        numx_real_t x2[2]={0,4}, y2[2]={0,8};
        numx_interp_linear(x2,y2,2,2.0f,&r);
        chk_f (ctx,"linear n=2 midpoint=4", r, 4.0f, 1e-5f);

        chk_rc(ctx, "linear null-xs",  numx_interp_linear(NULL,ys,3,0.5f,&r),  NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "linear null-ys",  numx_interp_linear(xs,NULL,3,0.5f,&r),  NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "linear null-out", numx_interp_linear(xs,ys,3,0.5f,NULL),  NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "linear n<2",      numx_interp_linear(xs,ys,1,0.5f,&r),    NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "linear n=0",      numx_interp_linear(xs,ys,0,0.5f,&r),    NUMX_ERR_INVALID_ARG);
    }

    suite("interp / spline_cubic");
    {
        numx_real_t xsl[4]={0,1,2,3}, ysl[4]={0,1,2,3}, m[4];
        chk_rc(ctx, "spline precompute rc", numx_interp_spline_precompute(xsl,ysl,4,m), NUMX_OK);
        numx_interp_spline_eval(xsl,ysl,m,4,0.5f,&r);
        chk_f (ctx,"spline linear data x=0.5", r, 0.5f, 1e-4f);
        numx_interp_spline_eval(xsl,ysl,m,4,1.7f,&r);
        chk_f (ctx,"spline linear data x=1.7", r, 1.7f, 1e-4f);

        numx_real_t xsc[4]={0,1,2,3}, ysc[4]={0,1,4,9};
        chk_rc(ctx, "spline cubic oneshot rc", numx_interp_spline_cubic(xsc,ysc,4,0.5f,&r), NUMX_OK);
        chk_f (ctx,"spline cubic at 0.5",     r, 0.35f, 5e-3f);

        numx_real_t xsb[5]={0,1,2,3,4}, ysb[5]={1,3,2,5,4}, mb[5];
        numx_interp_spline_precompute(xsb,ysb,5,mb);
        int ok=1;
        for(int i=0;i<5;i++){
            numx_interp_spline_eval(xsb,ysb,mb,5,xsb[i],&r);
            if(numxt_absf(r-ysb[i])>1e-3f){ok=0;break;}
        }
        chk_true(ctx, "spline at knots", ok);

        numx_real_t xscl[3]={1,2,3}, yscl[3]={1,4,9};
        numx_interp_spline_cubic(xscl,yscl,3,0.0f,&r);
        chk_f (ctx,"spline clamp-below", r, 1.0f, 1e-3f);
        numx_interp_spline_cubic(xscl,yscl,3,5.0f,&r);
        chk_f (ctx,"spline clamp-above", r, 9.0f, 1e-3f);

        numx_real_t xs2[2]={0,2}, ys2[2]={0,4};
        numx_interp_spline_cubic(xs2,ys2,2,1.0f,&r);
        chk_f (ctx,"spline n=2 linear midpoint=2", r, 2.0f, 1e-4f);

        chk_rc(ctx, "spline precompute null-xs",
               numx_interp_spline_precompute(NULL,ysl,4,m), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "spline precompute null-ys",
               numx_interp_spline_precompute(xsl,NULL,4,m), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "spline precompute null-m",
               numx_interp_spline_precompute(xsl,ysl,4,NULL),NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "spline precompute n<2",
               numx_interp_spline_precompute(xsl,ysl,1,m),   NUMX_ERR_INVALID_ARG);
    }

    suite("interp / chebyshev");
    {
        chk_rc(ctx, "cheb const rc", numx_interp_chebyshev(f_c3,4,0,1,0.5f,&r), NUMX_OK);
        chk_f (ctx,"cheb constant=3",  r, 3.0f, 1e-4f);

        numx_interp_chebyshev(f_xlin,4,0,2,1.0f,&r);
        chk_f (ctx,"cheb linear=1.0",  r, 1.0f, 1e-4f);

        numx_interp_chebyshev(f_sq,4,-1,1,0.5f,&r);
        chk_f (ctx,"cheb x^2 at 0.5",  r, 0.25f,1e-4f);

        numx_interp_chebyshev(f_sq,4,-1,1,0.0f,&r);
        chk_f (ctx,"cheb x^2 at 0",    r, 0.0f, 1e-4f);

        numx_interp_chebyshev(f_sq,2,-1,1,0.0f,&r);
        chk_f (ctx,"cheb n=2 x^2 at 0 ~ 0.5", r, 0.5f, 1e-4f);

        chk_rc(ctx, "cheb null-f",   numx_interp_chebyshev(NULL,4,0,1,0.5f,&r),    NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "cheb null-out", numx_interp_chebyshev(f_sq,4,0,1,0.5f,NULL),  NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "cheb b<=a",     numx_interp_chebyshev(f_sq,4,1,0,0.5f,&r),    NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "cheb a=b",      numx_interp_chebyshev(f_sq,4,1,1,0.5f,&r),    NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "cheb n<2",      numx_interp_chebyshev(f_sq,1,0,1,0.5f,&r),    NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "cheb n=0",      numx_interp_chebyshev(f_sq,0,0,1,0.5f,&r),    NUMX_ERR_INVALID_ARG);
    }
}
