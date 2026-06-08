#include "test_helpers.h"
#include "numx/stats.h"

void test_stats(numxt_ctx_t *ctx)
{
    numx_real_t r;

    suite("stats / mean");
    {
        numx_real_t a5[5]={1,2,3,4,5};
        chk_rc(ctx, "mean rc", numx_stats_mean(a5,5,&r), NUMX_OK);
        chk_f (ctx,"mean [1..5]=3",      r, 3.0f, 1e-5f);

        numx_real_t neg[4]={-3,-1,1,3};
        numx_stats_mean(neg,4,&r);
        chk_f (ctx,"mean negatives=0",   r, 0.0f, 1e-5f);

        numx_real_t c4[4]={5,5,5,5};
        numx_stats_mean(c4,4,&r);
        chk_f (ctx,"mean constant=5",    r, 5.0f, 1e-5f);

        numx_real_t a7[7]={1,2,3,4,5,6,7};
        numx_stats_mean(a7,7,&r);
        chk_f (ctx,"mean arith-seq [1..7]=4",  r, 4.0f, 1e-5f);

        numx_real_t s1[1]={42};
        numx_stats_mean(s1,1,&r);
        chk_f (ctx,"mean n=1=42",        r, 42.0f,1e-5f);

        chk_rc(ctx, "mean null-a",   numx_stats_mean(NULL,3,&r),  NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "mean null-out", numx_stats_mean(a5,5,NULL),  NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "mean n=0",      numx_stats_mean(a5,0,&r),    NUMX_ERR_INVALID_ARG);
    }

    suite("stats / variance");
    {
        numx_real_t d[8]={2,4,4,4,5,5,7,9};
        chk_rc(ctx, "var pop rc", numx_stats_variance(d,8,NUMX_VAR_POPULATION,&r), NUMX_OK);
        chk_f (ctx,"var pop=4",       r, 4.0f,           1e-4f);

        chk_rc(ctx, "var samp rc", numx_stats_variance(d,8,NUMX_VAR_SAMPLE,&r), NUMX_OK);
        chk_f (ctx,"var sample=32/7", r, 32.0f/7.0f,     1e-3f);

        numx_real_t c4[4]={7,7,7,7};
        numx_stats_variance(c4,4,NUMX_VAR_POPULATION,&r);
        chk_f (ctx,"var constant=0",  r, 0.0f,            1e-5f);

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
        chk_f (ctx,"median odd=3",    r, 3.0f, 1e-5f);

        numx_real_t even[4]={4,1,3,2};
        numx_stats_median(even,4,&r);
        chk_f (ctx,"median even=2.5", r, 2.5f, 1e-5f);

        numx_real_t inp[3]={5,3,1};
        numx_stats_median(inp,3,&r);
        chk_f (ctx,"median no-modify [0]", inp[0], 5.0f, 1e-6f);
        chk_f (ctx,"median no-modify [1]", inp[1], 3.0f, 1e-6f);
        chk_f (ctx,"median no-modify [2]", inp[2], 1.0f, 1e-6f);

        numx_real_t sorted5[5]={10,20,30,40,50};
        numx_stats_median(sorted5,5,&r);
        chk_f (ctx,"median sorted-mid=30",       r, 30.0f,1e-5f);

        numx_real_t s1[1]={7};
        numx_stats_median(s1,1,&r);
        chk_f (ctx,"median n=1=7",  r, 7.0f, 1e-5f);

        numx_real_t s2[2]={3,1};
        numx_stats_median(s2,2,&r);
        chk_f (ctx,"median n=2=2",  r, 2.0f, 1e-5f);

        chk_rc(ctx, "median null-a",  numx_stats_median(NULL,3,&r), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "median null-out",numx_stats_median(odd,5,NULL),NUMX_ERR_NULL_PTR);
    }

    suite("stats / percentile");
    {
        numx_real_t a[5]={5,3,1,4,2};
        chk_rc(ctx, "pct p0 rc", numx_stats_percentile(a,5,0.0f,&r),   NUMX_OK);
        chk_f (ctx,"pct p0=min=1",  r, 1.0f, 1e-5f);
        numx_stats_percentile(a,5,100.0f,&r);
        chk_f (ctx,"pct p100=max=5",r, 5.0f, 1e-5f);

        numx_real_t inp[3]={3,1,2};
        numx_stats_percentile(inp,3,50.0f,&r);
        chk_f (ctx,"pct no-modify [0]", inp[0], 3.0f, 1e-6f);

        chk_rc(ctx, "pct null-a",  numx_stats_percentile(NULL,3,50.0f,&r), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "pct null-out",numx_stats_percentile(a,5,50.0f,NULL),  NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "pct p<0",     numx_stats_percentile(a,5,-1.0f,&r),    NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "pct p>100",   numx_stats_percentile(a,5,101.0f,&r),   NUMX_ERR_INVALID_ARG);
    }
}
