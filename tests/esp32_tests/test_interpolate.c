#include "test_helpers.h"
#include "math_helpers.h"
#include "numx/interpolate.h"

void test_interpolate(numxt_ctx_t *ctx)
{
    numx_real_t r;

    suite("interp / linear");
    {
        numx_real_t xs[3]={0,1,2}, ys[3]={0,1,4};
        chk_rc(ctx, "linear rc", numx_interp_linear(xs,ys,3,(numx_real_t)0.5,&r), NUMX_OK);
        chk_f (ctx,"linear x=0.5 → 0.5",  r, (numx_real_t)0.5, (numx_real_t)1e-5);

        numx_interp_linear(xs,ys,3,(numx_real_t)1.5,&r);
        chk_f (ctx,"linear x=1.5 → 2.5",  r, (numx_real_t)2.5, (numx_real_t)1e-5);

        numx_real_t xs4[4]={0,1,3,6}, ys4[4]={1,3,2,5};
        int ok=1;
        for(int i=0;i<4;i++){
            numx_interp_linear(xs4,ys4,4,xs4[i],&r);
            if(numxt_absf(r-ys4[i])>(numx_real_t)1e-4){ok=0;break;}
        }
        chk_true(ctx, "linear at knots", ok);

        numx_real_t xsc[3]={1,2,3}, ysc[3]={10,20,30};
        numx_interp_linear(xsc,ysc,3,(numx_real_t)0.0,&r);
        chk_f (ctx,"linear clamp-below", r, (numx_real_t)10.0, (numx_real_t)1e-5);
        numx_interp_linear(xsc,ysc,3,(numx_real_t)5.0,&r);
        chk_f (ctx,"linear clamp-above", r, (numx_real_t)30.0, (numx_real_t)1e-5);

        numx_real_t x2[2]={0,4}, y2[2]={0,8};
        numx_interp_linear(x2,y2,2,(numx_real_t)2.0,&r);
        chk_f (ctx,"linear n=2 midpoint=4", r, (numx_real_t)4.0, (numx_real_t)1e-5);

        chk_rc(ctx, "linear null-xs",  numx_interp_linear(NULL,ys,3,(numx_real_t)0.5,&r),  NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "linear null-ys",  numx_interp_linear(xs,NULL,3,(numx_real_t)0.5,&r),  NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "linear null-out", numx_interp_linear(xs,ys,3,(numx_real_t)0.5,NULL),  NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "linear n<2",      numx_interp_linear(xs,ys,1,(numx_real_t)0.5,&r),    NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "linear n=0",      numx_interp_linear(xs,ys,0,(numx_real_t)0.5,&r),    NUMX_ERR_INVALID_ARG);
    }

    suite("interp / spline_cubic");
    {
        numx_real_t xsl[4]={0,1,2,3}, ysl[4]={0,1,2,3}, m[4];
        chk_rc(ctx, "spline precompute rc", numx_interp_spline_precompute(xsl,ysl,4,m), NUMX_OK);
        numx_interp_spline_eval(xsl,ysl,m,4,(numx_real_t)0.5,&r);
        chk_f (ctx,"spline linear data x=0.5", r, (numx_real_t)0.5, (numx_real_t)1e-4);
        numx_interp_spline_eval(xsl,ysl,m,4,(numx_real_t)1.7,&r);
        chk_f (ctx,"spline linear data x=1.7", r, (numx_real_t)1.7, (numx_real_t)1e-4);

        numx_real_t xsc[4]={0,1,2,3}, ysc[4]={0,1,4,9};
        chk_rc(ctx, "spline cubic oneshot rc", numx_interp_spline_cubic(xsc,ysc,4,(numx_real_t)0.5,&r), NUMX_OK);
        chk_f (ctx,"spline cubic at 0.5",     r, (numx_real_t)0.35, (numx_real_t)5e-3);

        numx_real_t xsb[5]={0,1,2,3,4}, ysb[5]={1,3,2,5,4}, mb[5];
        numx_interp_spline_precompute(xsb,ysb,5,mb);
        int ok=1;
        for(int i=0;i<5;i++){
            numx_interp_spline_eval(xsb,ysb,mb,5,xsb[i],&r);
            if(numxt_absf(r-ysb[i])>(numx_real_t)1e-3){ok=0;break;}
        }
        chk_true(ctx, "spline at knots", ok);

        numx_real_t xscl[3]={1,2,3}, yscl[3]={1,4,9};
        numx_interp_spline_cubic(xscl,yscl,3,(numx_real_t)0.0,&r);
        chk_f (ctx,"spline clamp-below", r, (numx_real_t)1.0, (numx_real_t)1e-3);
        numx_interp_spline_cubic(xscl,yscl,3,(numx_real_t)5.0,&r);
        chk_f (ctx,"spline clamp-above", r, (numx_real_t)9.0, (numx_real_t)1e-3);

        numx_real_t xs2[2]={0,2}, ys2[2]={0,4};
        numx_interp_spline_cubic(xs2,ys2,2,(numx_real_t)1.0,&r);
        chk_f (ctx,"spline n=2 linear midpoint=2", r, (numx_real_t)2.0, (numx_real_t)1e-4);

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
        chk_rc(ctx, "cheb const rc", numx_interp_chebyshev(f_c3,4,0,1,(numx_real_t)0.5,&r), NUMX_OK);
        chk_f (ctx,"cheb constant=3",  r, (numx_real_t)3.0, (numx_real_t)1e-4);

        numx_interp_chebyshev(f_xlin,4,0,2,(numx_real_t)1.0,&r);
        chk_f (ctx,"cheb linear=1.0",  r, (numx_real_t)1.0, (numx_real_t)1e-4);

        numx_interp_chebyshev(f_sq,4,-1,1,(numx_real_t)0.5,&r);
        chk_f (ctx,"cheb x^2 at 0.5",  r, (numx_real_t)0.25,(numx_real_t)1e-4);

        numx_interp_chebyshev(f_sq,4,-1,1,(numx_real_t)0.0,&r);
        chk_f (ctx,"cheb x^2 at 0",    r, (numx_real_t)0.0, (numx_real_t)1e-4);

        numx_interp_chebyshev(f_sq,2,-1,1,(numx_real_t)0.0,&r);
        chk_f (ctx,"cheb n=2 x^2 at 0 ~ 0.5", r, (numx_real_t)0.5, (numx_real_t)1e-4);

        chk_rc(ctx, "cheb null-f",   numx_interp_chebyshev(NULL,4,0,1,(numx_real_t)0.5,&r),    NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "cheb null-out", numx_interp_chebyshev(f_sq,4,0,1,(numx_real_t)0.5,NULL),  NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "cheb b<=a",     numx_interp_chebyshev(f_sq,4,1,0,(numx_real_t)0.5,&r),    NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "cheb a=b",      numx_interp_chebyshev(f_sq,4,1,1,(numx_real_t)0.5,&r),    NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "cheb n<2",      numx_interp_chebyshev(f_sq,1,0,1,(numx_real_t)0.5,&r),    NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "cheb n=0",      numx_interp_chebyshev(f_sq,0,0,1,(numx_real_t)0.5,&r),    NUMX_ERR_INVALID_ARG);
    }
}
