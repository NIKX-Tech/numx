#include "test_helpers.h"
#include "numx/signal.h"

void test_signal(numxt_ctx_t *ctx)
{
    /* ── window_rect ─────────────────────────────────────────────── */
    suite("signal / window_rect");
    {
        numx_real_t w[4];
        chk_rc(ctx, "rect n=4 rc",     numx_signal_window_rect(4, w),    NUMX_OK);
        chk_f (ctx, "rect w[0]=1",     w[0], (numx_real_t)1.0, (numx_real_t)1e-6);
        chk_f (ctx, "rect w[1]=1",     w[1], (numx_real_t)1.0, (numx_real_t)1e-6);
        chk_f (ctx, "rect w[3]=1",     w[3], (numx_real_t)1.0, (numx_real_t)1e-6);

        numx_real_t w1[1];
        chk_rc(ctx, "rect n=1 rc",     numx_signal_window_rect(1, w1),   NUMX_OK);
        chk_f (ctx, "rect n=1 w[0]=1", w1[0], (numx_real_t)1.0, (numx_real_t)1e-6);

        chk_rc(ctx, "rect null",        numx_signal_window_rect(4, NULL), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "rect n=0",         numx_signal_window_rect(0, w),    NUMX_ERR_INVALID_ARG);
    }

    /* ── window_hann ─────────────────────────────────────────────── */
    suite("signal / window_hann");
    {
        /* w[i] = 0.5*(1-cos(2*pi*i/(n-1))); n=5 */
        numx_real_t w[5];
        chk_rc(ctx, "hann n=5 rc",         numx_signal_window_hann(5, w),    NUMX_OK);
        chk_f (ctx, "hann w[0]=0 (left)",  w[0], (numx_real_t)0.0, (numx_real_t)1e-5);
        chk_f (ctx, "hann w[4]=0 (right)", w[4], (numx_real_t)0.0, (numx_real_t)1e-5);
        chk_f (ctx, "hann w[2]=1 (peak)",  w[2], (numx_real_t)1.0, (numx_real_t)1e-5);
        chk_f (ctx, "hann symmetric",      w[1], w[3], (numx_real_t)1e-6);

        chk_rc(ctx, "hann null",            numx_signal_window_hann(5, NULL), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "hann n=0",             numx_signal_window_hann(0, w),    NUMX_ERR_INVALID_ARG);
    }

    /* ── window_hamming ──────────────────────────────────────────── */
    suite("signal / window_hamming");
    {
        /* w[i] = 0.54 - 0.46*cos(2*pi*i/(n-1)); n=5 */
        numx_real_t w[5];
        chk_rc(ctx, "hamming n=5 rc",         numx_signal_window_hamming(5, w),    NUMX_OK);
        chk_f (ctx, "hamming w[0]=0.08",       w[0], (numx_real_t)0.08, (numx_real_t)1e-4);   /* 0.54-0.46 */
        chk_f (ctx, "hamming w[2]=1 (peak)",   w[2], (numx_real_t)1.0,  (numx_real_t)1e-5);   /* 0.54+0.46 */
        chk_f (ctx, "hamming symmetric",        w[1], w[3], (numx_real_t)1e-6);

        chk_rc(ctx, "hamming null",             numx_signal_window_hamming(5, NULL), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "hamming n=0",              numx_signal_window_hamming(0, w),    NUMX_ERR_INVALID_ARG);
    }

    /* ── window_blackman ─────────────────────────────────────────── */
    suite("signal / window_blackman");
    {
        /* w[i] = 0.42 - 0.5*cos(2pi*i/(n-1)) + 0.08*cos(4pi*i/(n-1)); n=5 */
        numx_real_t w[5];
        chk_rc(ctx, "blackman n=5 rc",         numx_signal_window_blackman(5, w),    NUMX_OK);
        chk_f (ctx, "blackman w[0]≈0",          w[0], (numx_real_t)0.0, (numx_real_t)1e-4);   /* 0.42-0.5+0.08 */
        chk_f (ctx, "blackman w[2]=1 (peak)",   w[2], (numx_real_t)1.0, (numx_real_t)1e-4);   /* 0.42+0.5+0.08 */
        chk_f (ctx, "blackman symmetric",        w[1], w[3], (numx_real_t)1e-5);

        chk_rc(ctx, "blackman null",             numx_signal_window_blackman(5, NULL), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "blackman n=0",              numx_signal_window_blackman(0, w),    NUMX_ERR_INVALID_ARG);
    }

    /* ── convolve ────────────────────────────────────────────────── */
    suite("signal / convolve");
    {
        /* impulse * h = h (zero-padded to xn+hn-1 = 5) */
        numx_real_t x[3] = {1,0,0}, h[3] = {1,2,3}, out[5];
        chk_rc(ctx, "convolve impulse rc",    numx_signal_convolve(x,3,h,3,out), NUMX_OK);
        chk_f (ctx, "convolve out[0]=1",      out[0], (numx_real_t)1.0, (numx_real_t)1e-5);
        chk_f (ctx, "convolve out[1]=2",      out[1], (numx_real_t)2.0, (numx_real_t)1e-5);
        chk_f (ctx, "convolve out[2]=3",      out[2], (numx_real_t)3.0, (numx_real_t)1e-5);
        chk_f (ctx, "convolve out[3]=0",      out[3], (numx_real_t)0.0, (numx_real_t)1e-5);
        chk_f (ctx, "convolve out[4]=0",      out[4], (numx_real_t)0.0, (numx_real_t)1e-5);

        /* [1,1] * [1,1] = [1,2,1] */
        numx_real_t xa[2] = {1,1}, hb[2] = {1,1}, ob[3];
        numx_signal_convolve(xa,2,hb,2,ob);
        chk_f (ctx, "convolve [1,1]*[1,1] ob[1]=2", ob[1], (numx_real_t)2.0, (numx_real_t)1e-5);

        chk_rc(ctx, "convolve null-x",        numx_signal_convolve(NULL,3,h,3,out), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "convolve null-h",        numx_signal_convolve(x,3,NULL,3,out), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "convolve null-out",      numx_signal_convolve(x,3,h,3,NULL),   NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "convolve xn=0",          numx_signal_convolve(x,0,h,3,out),    NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "convolve hn=0",          numx_signal_convolve(x,3,h,0,out),    NUMX_ERR_INVALID_ARG);
    }

    /* ── correlate ───────────────────────────────────────────────── */
    suite("signal / correlate");
    {
        /* auto-correlation peak at lag-0 (index yn-1 = 2) = sum x[i]^2 = 14 */
        numx_real_t x[3] = {1,2,3}, out[5];
        chk_rc(ctx, "correlate auto rc",       numx_signal_correlate(x,3,x,3,out), NUMX_OK);
        chk_f (ctx, "correlate auto peak=14",  out[2], (numx_real_t)14.0, (numx_real_t)1e-4);
        chk_f (ctx, "correlate auto symmetric",out[1], out[3], (numx_real_t)1e-4);

        chk_rc(ctx, "correlate null-x",        numx_signal_correlate(NULL,3,x,3,out), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "correlate null-y",        numx_signal_correlate(x,3,NULL,3,out), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "correlate null-out",      numx_signal_correlate(x,3,x,3,NULL),   NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "correlate xn=0",          numx_signal_correlate(x,0,x,3,out),    NUMX_ERR_INVALID_ARG);
    }

    /* ── fir ─────────────────────────────────────────────────────── */
    suite("signal / fir");
    {
        numx_real_t x[4] = {1,2,3,4}, out[4];

        /* identity tap [1]: pass-through */
        numx_real_t t1[1] = {(numx_real_t)1.0};
        chk_rc(ctx, "fir identity rc",   numx_signal_fir(x,4,t1,1,out), NUMX_OK);
        chk_f (ctx, "fir identity [0]",  out[0], (numx_real_t)1.0, (numx_real_t)1e-5);
        chk_f (ctx, "fir identity [2]",  out[2], (numx_real_t)3.0, (numx_real_t)1e-5);

        /* 2-tap box [0.5,0.5]: out[1]=0.5*x[1]+0.5*x[0]=1.5 */
        numx_real_t t2[2] = {(numx_real_t)0.5, (numx_real_t)0.5};
        numx_signal_fir(x,4,t2,2,out);
        chk_f (ctx, "fir box out[1]=1.5", out[1], (numx_real_t)1.5, (numx_real_t)1e-5);

        chk_rc(ctx, "fir null-x",         numx_signal_fir(NULL,4,t1,1,out), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "fir null-taps",       numx_signal_fir(x,4,NULL,1,out), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "fir null-out",        numx_signal_fir(x,4,t1,1,NULL),  NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "fir xn=0",            numx_signal_fir(x,0,t1,1,out),   NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "fir ntaps=0",         numx_signal_fir(x,4,t1,0,out),   NUMX_ERR_INVALID_ARG);
    }

    /* ── iir_biquad ──────────────────────────────────────────────── */
    suite("signal / iir_biquad");
    {
        numx_real_t x[4] = {1,2,3,4}, out[4];

        /* all-pass: b=[1,0,0], a=[0,0] → H(z)=1, output=input */
        numx_real_t b[3] = {1,0,0}, a[2] = {0,0};
        chk_rc(ctx, "biquad all-pass rc",  numx_signal_iir_biquad(x,4,b,a,out), NUMX_OK);
        chk_f (ctx, "biquad all-pass [0]", out[0], (numx_real_t)1.0, (numx_real_t)1e-5);
        chk_f (ctx, "biquad all-pass [2]", out[2], (numx_real_t)3.0, (numx_real_t)1e-5);

        chk_rc(ctx, "biquad null-x",       numx_signal_iir_biquad(NULL,4,b,a,out), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "biquad null-b",       numx_signal_iir_biquad(x,4,NULL,a,out), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "biquad null-a",       numx_signal_iir_biquad(x,4,b,NULL,out), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "biquad null-out",     numx_signal_iir_biquad(x,4,b,a,NULL),   NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "biquad n=0",          numx_signal_iir_biquad(x,0,b,a,out),    NUMX_ERR_INVALID_ARG);
    }

    /* ── peaks ───────────────────────────────────────────────────── */
    suite("signal / peaks");
    {
        /* [1,3,1,2,1] → peaks at indices 1 and 3 */
        numx_real_t xp[5] = {1,3,1,2,1};
        numx_size_t pk[4], np;
        chk_rc(ctx, "peaks 2-peak rc",     numx_signal_peaks(xp,5,pk,4,&np), NUMX_OK);
        chk_true(ctx, "peaks count=2",     np == 2);
        chk_true(ctx, "peaks pk[0]=1",     pk[0] == 1);
        chk_true(ctx, "peaks pk[1]=3",     pk[1] == 3);

        /* monotone increasing: no peaks */
        numx_real_t xm[4] = {1,2,3,4};
        numx_signal_peaks(xm,4,pk,4,&np);
        chk_true(ctx, "peaks monotone=0",  np == 0);

        /* n<3: no peaks regardless */
        numx_real_t xs[2] = {2,1};
        numx_signal_peaks(xs,2,pk,4,&np);
        chk_true(ctx, "peaks n=2 → 0",    np == 0);

        chk_rc(ctx, "peaks null-x",        numx_signal_peaks(NULL,5,pk,4,&np), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "peaks null-peaks",    numx_signal_peaks(xp,5,NULL,4,&np), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "peaks null-npeaks",   numx_signal_peaks(xp,5,pk,4,NULL),  NUMX_ERR_NULL_PTR);
    }

    /* ── ema ─────────────────────────────────────────────────────── */
    suite("signal / ema");
    {
        numx_real_t x[3] = {1,3,2}, out[3];

        /* alpha=1: out[i]=x[i] */
        chk_rc(ctx, "ema alpha=1 rc",    numx_signal_ema(x,3,(numx_real_t)1.0,out), NUMX_OK);
        chk_f (ctx, "ema alpha=1 [0]=1", out[0], (numx_real_t)1.0, (numx_real_t)1e-5);
        chk_f (ctx, "ema alpha=1 [1]=3", out[1], (numx_real_t)3.0, (numx_real_t)1e-5);
        chk_f (ctx, "ema alpha=1 [2]=2", out[2], (numx_real_t)2.0, (numx_real_t)1e-5);

        /* alpha=0: out[i]=x[0] for all i */
        numx_signal_ema(x,3,(numx_real_t)0.0,out);
        chk_f (ctx, "ema alpha=0 [1]=1", out[1], (numx_real_t)1.0, (numx_real_t)1e-5);
        chk_f (ctx, "ema alpha=0 [2]=1", out[2], (numx_real_t)1.0, (numx_real_t)1e-5);

        /* alpha=0.5: [1,2,3] → [1, 1.5, 2.25] */
        numx_real_t xa[3] = {1,2,3};
        numx_signal_ema(xa,3,(numx_real_t)0.5,out);
        chk_f (ctx, "ema alpha=0.5 [0]=1.0",  out[0], (numx_real_t)1.0,  (numx_real_t)1e-5);
        chk_f (ctx, "ema alpha=0.5 [1]=1.5",  out[1], (numx_real_t)1.5,  (numx_real_t)1e-5);
        chk_f (ctx, "ema alpha=0.5 [2]=2.25", out[2], (numx_real_t)2.25, (numx_real_t)1e-5);

        chk_rc(ctx, "ema null-x",    numx_signal_ema(NULL,3,(numx_real_t)0.5,out), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "ema null-out",  numx_signal_ema(x,3,(numx_real_t)0.5,NULL),   NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "ema n=0",       numx_signal_ema(x,0,(numx_real_t)0.5,out),    NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "ema alpha<0",   numx_signal_ema(x,3,-(numx_real_t)0.1,out),   NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "ema alpha>1",   numx_signal_ema(x,3,(numx_real_t)1.1,out),    NUMX_ERR_INVALID_ARG);
    }
}
