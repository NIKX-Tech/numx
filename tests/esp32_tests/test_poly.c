#include "test_helpers.h"
#include "numx/poly.h"

void test_poly(numxt_ctx_t *ctx)
{
    numx_real_t r;

    suite("poly / eval");
    {
        numx_real_t c1[3]={1,0,1};
        chk_rc(ctx, "poly_eval rc", numx_poly_eval(c1,2,(numx_real_t)3.0,&r), NUMX_OK);
        chk_f (ctx,"poly x^2+1 at 3=10",   r, (numx_real_t)10.0, (numx_real_t)1e-4);

        numx_real_t c2[4]={1,0,-2,1};
        numx_poly_eval(c2,3,(numx_real_t)2.0,&r);
        chk_f (ctx,"poly x^3-2x+1 at 2=5", r, (numx_real_t)5.0,  (numx_real_t)1e-4);

        numx_real_t c3[3]={3,7,-2};
        numx_poly_eval(c3,2,(numx_real_t)0.0,&r);
        chk_f (ctx,"poly at 0 = const=-2",  r,-(numx_real_t)2.0,  (numx_real_t)1e-5);

        numx_real_t c4[1]={5};
        numx_poly_eval(c4,0,(numx_real_t)99.0,&r);
        chk_f (ctx,"poly degree-0 = 5",      r, (numx_real_t)5.0,  (numx_real_t)1e-5);

        numx_real_t c5[3]={2,3,4};
        numx_poly_eval(c5,2,(numx_real_t)5.0,&r);
        chk_f (ctx,"poly Horner 2x^2+3x+4 at 5=69", r, (numx_real_t)69.0, (numx_real_t)1e-3);

        numx_real_t c6[4]={1,-6,11,-6};
        numx_poly_eval(c6,3,(numx_real_t)1.0,&r); chk_f(ctx,"poly (x-1)(x-2)(x-3) at 1=0", r, (numx_real_t)0.0, (numx_real_t)1e-4);
        numx_poly_eval(c6,3,(numx_real_t)2.0,&r); chk_f(ctx,"poly (x-1)(x-2)(x-3) at 2=0", r, (numx_real_t)0.0, (numx_real_t)1e-4);
        numx_poly_eval(c6,3,(numx_real_t)3.0,&r); chk_f(ctx,"poly (x-1)(x-2)(x-3) at 3=0", r, (numx_real_t)0.0, (numx_real_t)1e-4);

        numx_real_t c7[3]={1,-3,2};
        numx_poly_eval(c7,2,(numx_real_t)1.0,&r);
        chk_f (ctx,"poly x^2-3x+2 at 1=0",  r, (numx_real_t)0.0, (numx_real_t)1e-4);

        chk_rc(ctx, "poly_eval null-c",   numx_poly_eval(NULL,2,(numx_real_t)1.0,&r),   NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "poly_eval null-out", numx_poly_eval(c1,2,(numx_real_t)1.0,NULL),   NUMX_ERR_NULL_PTR);
    }

    suite("poly / roots");
    {
        numx_real_t c1[2]={1,1}, ro[3];
        numx_size_t nr;
        chk_rc(ctx, "poly_roots linear rc", numx_poly_roots(c1,1,ro,&nr,(numx_real_t)1e-4), NUMX_OK);
        chk_true(ctx, "poly_roots linear nroots=1", nr==1);
        chk_f(ctx, "poly_roots linear root=-1", ro[0], -(numx_real_t)1.0, (numx_real_t)1e-3);

        numx_real_t c2[3]={1,-5,6}, ro2[2];
        numx_poly_roots(c2,2,ro2,&nr,(numx_real_t)1e-4);
        chk_true(ctx, "poly_roots quad nroots=2", nr==2);
        numx_real_t r0,r1;
        numx_poly_eval(c2,2,ro2[0],&r0);
        numx_poly_eval(c2,2,ro2[1],&r1);
        chk_f(ctx, "poly_roots quad f(r0)≈0", r0, (numx_real_t)0.0, (numx_real_t)1e-3);
        chk_f(ctx, "poly_roots quad f(r1)≈0", r1, (numx_real_t)0.0, (numx_real_t)1e-3);
        {
            int found2 = (numxt_absf(ro2[0]-(numx_real_t)2.0)<(numx_real_t)1e-3 || numxt_absf(ro2[1]-(numx_real_t)2.0)<(numx_real_t)1e-3);
            int found3 = (numxt_absf(ro2[0]-(numx_real_t)3.0)<(numx_real_t)1e-3 || numxt_absf(ro2[1]-(numx_real_t)3.0)<(numx_real_t)1e-3);
            chk_true(ctx, "poly_roots quad root=2 found", found2);
            chk_true(ctx, "poly_roots quad root=3 found", found3);
        }

        numx_real_t c3[4]={1,-6,11,-6}, ro3[3];
        numx_poly_roots(c3,3,ro3,&nr,(numx_real_t)1e-4);
        int ok=1;
        for(numx_size_t i=0;i<nr;i++){
            numx_real_t fv; numx_poly_eval(c3,3,ro3[i],&fv);
            if(numxt_absf(fv)>(numx_real_t)1e-2){ok=0;break;}
        }
        chk_true(ctx, "poly_roots cubic residuals all ≈ 0", ok);

        numx_real_t c4[4]={1,0,0,-8}, ro4[3];
        numx_poly_roots(c4,3,ro4,&nr,(numx_real_t)1e-4);
        chk_true(ctx, "poly_roots x^3-8 has root", nr>=1);
        {
            int root2_found = 0;
            for(numx_size_t i=0;i<nr;i++)
                if(numxt_absf(ro4[i]-(numx_real_t)2.0)<(numx_real_t)1e-2){root2_found=1;break;}
            chk_true(ctx, "poly_roots x^3-8 real root≈2", root2_found);
        }

        chk_rc(ctx, "poly_roots null-c",   numx_poly_roots(NULL,2,ro2,&nr,(numx_real_t)1e-4), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "poly_roots null-out", numx_poly_roots(c2,2,NULL,&nr,(numx_real_t)1e-4),  NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "poly_roots null-nr",  numx_poly_roots(c2,2,ro2,NULL,(numx_real_t)1e-4),  NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "poly_roots degree=0", numx_poly_roots(c2,0,ro2,&nr,(numx_real_t)1e-4),   NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "poly_roots tol=0",    numx_poly_roots(c2,2,ro2,&nr,(numx_real_t)0.0),    NUMX_ERR_INVALID_ARG);
    }
}
