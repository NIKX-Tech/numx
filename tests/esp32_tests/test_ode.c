#include "test_helpers.h"
#include "math_helpers.h"
#include "numx/ode.h"

void test_ode(numxt_ctx_t *ctx)
{
    suite("ode / rk4");
    {
        numx_real_t y[1]={1.0f};
        chk_rc(ctx, "rk4 growth rc", numx_ode_rk4(ode_growth,0,y,1,1e-3f,1000,y), NUMX_OK);
        chk_f (ctx,"rk4 growth y(1)=e", y[0], 2.71828f, 1e-3f);

        numx_real_t yd[1]={1.0f};
        numx_ode_rk4(ode_decay,0,yd,1,1e-3f,1000,yd);
        chk_f (ctx,"rk4 decay y(1)=1/e", yd[0], 0.36788f, 1e-3f);

        numx_real_t yg2[1]={1.0f};
        numx_ode_rk4(ode_growth,0,yg2,1,1e-3f,2000,yg2);
        chk_f (ctx,"rk4 growth y(2)=e^2", yg2[0], 7.38906f, 1e-2f);

        numx_real_t yh[2]={1.0f,0.0f};
        numx_real_t E0 = 0.5f*(yh[0]*yh[0]+yh[1]*yh[1]);
        numx_ode_rk4(ode_harm,0,yh,2,1e-3f,1000,yh);
        numx_real_t E1 = 0.5f*(yh[0]*yh[0]+yh[1]*yh[1]);
        chk_true(ctx, "rk4 harmonic energy conserved", numxt_absf(E1-E0)<1e-2f);

        numx_real_t ys[1]={1.0f};
        numx_ode_rk4(ode_growth,0,ys,1,0.01f,1,ys);
        chk_f (ctx,"rk4 single step", ys[0], 1.01005f, 1e-4f);

        numx_real_t ye[1]={1.0f};
        chk_rc(ctx, "rk4 rhs-error propagated",
               numx_ode_rk4(ode_err,0,ye,1,0.1f,10,ye), NUMX_ERR_SINGULAR);

        numx_real_t y0[1]={1.0f}, yout[1];
        chk_rc(ctx, "rk4 null-f",    numx_ode_rk4(NULL,0,y0,1,0.1f,10,yout), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "rk4 null-y0",   numx_ode_rk4(ode_growth,0,NULL,1,0.1f,10,yout),NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "rk4 null-yout", numx_ode_rk4(ode_growth,0,y0,1,0.1f,10,NULL),  NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "rk4 h=0",       numx_ode_rk4(ode_growth,0,y0,1,0.0f,10,yout),  NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "rk4 h<0",       numx_ode_rk4(ode_growth,0,y0,1,-0.1f,10,yout), NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "rk4 steps=0",   numx_ode_rk4(ode_growth,0,y0,1,0.1f,0,yout),   NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "rk4 n=0",       numx_ode_rk4(ode_growth,0,y0,0,0.1f,10,yout),  NUMX_ERR_INVALID_ARG);
    }

    suite("ode / rk45");
    {
        numx_real_t yg[1]={1.0f};
        chk_rc(ctx, "rk45 growth rc", numx_ode_rk45(ode_growth,0,1.0f,yg,1,1e-4f,yg), NUMX_OK);
        chk_f (ctx,"rk45 growth y(1)=e", yg[0], 2.71828f, 1e-3f);

        numx_real_t yd[1]={1.0f};
        numx_ode_rk45(ode_decay,0,1.0f,yd,1,1e-4f,yd);
        chk_f (ctx,"rk45 decay y(1)=1/e", yd[0], 0.36788f, 1e-3f);

        numx_real_t yh[2]={1.0f,0.0f};
        numx_real_t E0 = 0.5f*(yh[0]*yh[0]+yh[1]*yh[1]);
        numx_ode_rk45(ode_harm,0,10.0f,yh,2,1e-4f,yh);
        numx_real_t E1 = 0.5f*(yh[0]*yh[0]+yh[1]*yh[1]);
        chk_true(ctx, "rk45 harmonic energy conserved", numxt_absf(E1-E0)<1e-2f);

        numx_real_t ye[1]={1.0f};
        chk_rc(ctx, "rk45 rhs-error propagated",
               numx_ode_rk45(ode_err,0,1.0f,ye,1,1e-4f,ye), NUMX_ERR_SINGULAR);

        numx_real_t y0[1]={1.0f}, yout[1];
        chk_rc(ctx, "rk45 null-f",    numx_ode_rk45(NULL,0,1,y0,1,1e-4f,yout),          NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "rk45 null-y0",   numx_ode_rk45(ode_growth,0,1,NULL,1,1e-4f,yout),  NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "rk45 null-yout", numx_ode_rk45(ode_growth,0,1,y0,1,1e-4f,NULL),    NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "rk45 t1<=t0",    numx_ode_rk45(ode_growth,1,0,y0,1,1e-4f,yout),    NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "rk45 t1=t0",     numx_ode_rk45(ode_growth,1,1,y0,1,1e-4f,yout),    NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "rk45 tol=0",     numx_ode_rk45(ode_growth,0,1,y0,1,0.0f,yout),     NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "rk45 tol<0",     numx_ode_rk45(ode_growth,0,1,y0,1,-1e-4f,yout),   NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "rk45 n=0",       numx_ode_rk45(ode_growth,0,1,y0,0,1e-4f,yout),    NUMX_ERR_INVALID_ARG);
    }
}
