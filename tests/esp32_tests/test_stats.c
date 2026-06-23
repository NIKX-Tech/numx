#include "test_helpers.h"
#include "numx/stats.h"

void test_stats(numxt_ctx_t *ctx)
{
    numx_real_t r;

    suite("stats / mean");
    {
        numx_real_t a5[5]={1,2,3,4,5};
        chk_rc(ctx, "mean rc", numx_stats_mean(a5,5,&r), NUMX_OK);
        chk_f (ctx,"mean [1..5]=3",      r, (numx_real_t)3.0, (numx_real_t)1e-5);

        numx_real_t neg[4]={-3,-1,1,3};
        numx_stats_mean(neg,4,&r);
        chk_f (ctx,"mean negatives=0",   r, (numx_real_t)0.0, (numx_real_t)1e-5);

        numx_real_t c4[4]={5,5,5,5};
        numx_stats_mean(c4,4,&r);
        chk_f (ctx,"mean constant=5",    r, (numx_real_t)5.0, (numx_real_t)1e-5);

        numx_real_t a7[7]={1,2,3,4,5,6,7};
        numx_stats_mean(a7,7,&r);
        chk_f (ctx,"mean arith-seq [1..7]=4",  r, (numx_real_t)4.0, (numx_real_t)1e-5);

        numx_real_t s1[1]={42};
        numx_stats_mean(s1,1,&r);
        chk_f (ctx,"mean n=1=42",        r, (numx_real_t)42.0,(numx_real_t)1e-5);

        chk_rc(ctx, "mean null-a",   numx_stats_mean(NULL,3,&r),  NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "mean null-out", numx_stats_mean(a5,5,NULL),  NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "mean n=0",      numx_stats_mean(a5,0,&r),    NUMX_ERR_INVALID_ARG);
    }

    suite("stats / variance");
    {
        numx_real_t d[8]={2,4,4,4,5,5,7,9};
        chk_rc(ctx, "var pop rc", numx_stats_variance(d,8,NUMX_VAR_POPULATION,&r), NUMX_OK);
        chk_f (ctx,"var pop=4",       r, (numx_real_t)4.0,           (numx_real_t)1e-4);

        chk_rc(ctx, "var samp rc", numx_stats_variance(d,8,NUMX_VAR_SAMPLE,&r), NUMX_OK);
        chk_f (ctx,"var sample=32/7", r, (numx_real_t)32.0/(numx_real_t)7.0,     (numx_real_t)1e-3);

        numx_real_t c4[4]={7,7,7,7};
        numx_stats_variance(c4,4,NUMX_VAR_POPULATION,&r);
        chk_f (ctx,"var constant=0",  r, (numx_real_t)0.0,            (numx_real_t)1e-5);

        numx_real_t a5[5]={1,2,3,4,5};
        numx_real_t rp, rs;
        numx_stats_variance(a5,5,NUMX_VAR_POPULATION,&rp);
        numx_stats_variance(a5,5,NUMX_VAR_SAMPLE,&rs);
        chk_true(ctx, "var sample > population", rs > rp);

        chk_rc(ctx, "var sample n=1 rejected",
               numx_stats_variance(a5,1,NUMX_VAR_SAMPLE,&r), NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "var null-a",  numx_stats_variance(NULL,4,NUMX_VAR_POPULATION,&r), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "var null-out",numx_stats_variance(d,8,NUMX_VAR_POPULATION,NULL),  NUMX_ERR_NULL_PTR);
    }

    suite("stats / median");
    {
        numx_real_t odd[5]={5,3,1,4,2};
        chk_rc(ctx, "median odd rc", numx_stats_median(odd,5,&r), NUMX_OK);
        chk_f (ctx,"median odd=3",    r, (numx_real_t)3.0, (numx_real_t)1e-5);

        numx_real_t even[4]={4,1,3,2};
        numx_stats_median(even,4,&r);
        chk_f (ctx,"median even=2.5", r, (numx_real_t)2.5, (numx_real_t)1e-5);

        numx_real_t inp[3]={5,3,1};
        numx_stats_median(inp,3,&r);
        chk_f (ctx,"median no-modify [0]", inp[0], (numx_real_t)5.0, (numx_real_t)1e-6);
        chk_f (ctx,"median no-modify [1]", inp[1], (numx_real_t)3.0, (numx_real_t)1e-6);
        chk_f (ctx,"median no-modify [2]", inp[2], (numx_real_t)1.0, (numx_real_t)1e-6);

        numx_real_t sorted5[5]={10,20,30,40,50};
        numx_stats_median(sorted5,5,&r);
        chk_f (ctx,"median sorted-mid=30",       r, (numx_real_t)30.0,(numx_real_t)1e-5);

        numx_real_t s1[1]={7};
        numx_stats_median(s1,1,&r);
        chk_f (ctx,"median n=1=7",  r, (numx_real_t)7.0, (numx_real_t)1e-5);

        numx_real_t s2[2]={3,1};
        numx_stats_median(s2,2,&r);
        chk_f (ctx,"median n=2=2",  r, (numx_real_t)2.0, (numx_real_t)1e-5);

        chk_rc(ctx, "median null-a",  numx_stats_median(NULL,3,&r), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "median null-out",numx_stats_median(odd,5,NULL),NUMX_ERR_NULL_PTR);
    }

    suite("stats / percentile");
    {
        numx_real_t a[5]={5,3,1,4,2};
        chk_rc(ctx, "pct p0 rc", numx_stats_percentile(a,5,(numx_real_t)0.0,&r),   NUMX_OK);
        chk_f (ctx,"pct p0=min=1",  r, (numx_real_t)1.0, (numx_real_t)1e-5);
        numx_stats_percentile(a,5,(numx_real_t)100.0,&r);
        chk_f (ctx,"pct p100=max=5",r, (numx_real_t)5.0, (numx_real_t)1e-5);

        numx_real_t inp[3]={3,1,2};
        numx_stats_percentile(inp,3,(numx_real_t)50.0,&r);
        chk_f (ctx,"pct no-modify [0]", inp[0], (numx_real_t)3.0, (numx_real_t)1e-6);

        chk_rc(ctx, "pct null-a",  numx_stats_percentile(NULL,3,(numx_real_t)50.0,&r), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "pct null-out",numx_stats_percentile(a,5,(numx_real_t)50.0,NULL),  NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "pct p<0",     numx_stats_percentile(a,5,-(numx_real_t)1.0,&r),    NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "pct p>100",   numx_stats_percentile(a,5,(numx_real_t)101.0,&r),   NUMX_ERR_INVALID_ARG);
    }
}
