#include "test_helpers.h"
#include "numx/fft.h"

/*
 * Interleaved complex layout: inout = [re0,im0, re1,im1, ...]
 * Lengths below refer to the number of complex samples N, so the buffer
 * is always 2*N floats.
 */

void test_fft(numxt_ctx_t *ctx)
{
    /* ── fft_f32: DC input ───────────────────────────────────────── */
    suite("fft / f32_dc");
    {
        /* N=8, all re=1 im=0 → X[0].re=8, rest≈0 */
        numx_real_t buf[16] = {
            1,0, 1,0, 1,0, 1,0,
            1,0, 1,0, 1,0, 1,0
        };
        chk_rc(ctx, "fft_f32 dc n=8 rc",    numx_fft_f32(buf, 8), NUMX_OK);
        chk_f (ctx, "fft_f32 dc X[0].re=8", buf[0],  8.0f, 1e-3f);
        chk_f (ctx, "fft_f32 dc X[0].im=0", buf[1],  0.0f, 1e-3f);
        chk_f (ctx, "fft_f32 dc X[1].re≈0", buf[2],  0.0f, 1e-3f);
        chk_f (ctx, "fft_f32 dc X[4].re≈0", buf[8],  0.0f, 1e-3f);
    }

    /* ── fft_f32: unit impulse → flat spectrum ───────────────────── */
    suite("fft / f32_impulse");
    {
        /* N=4, x[0]=1 rest=0 → all bins re=1, im=0 */
        numx_real_t buf[8] = {1,0, 0,0, 0,0, 0,0};
        chk_rc(ctx, "fft_f32 impulse rc",     numx_fft_f32(buf, 4), NUMX_OK);
        chk_f (ctx, "fft_f32 impulse X[0]=1", buf[0], 1.0f, 1e-4f);
        chk_f (ctx, "fft_f32 impulse X[1]=1", buf[2], 1.0f, 1e-4f);
        chk_f (ctx, "fft_f32 impulse X[2]=1", buf[4], 1.0f, 1e-4f);
        chk_f (ctx, "fft_f32 impulse X[3]=1", buf[6], 1.0f, 1e-4f);
    }

    /* ── FFT/IFFT round-trip ─────────────────────────────────────── */
    suite("fft / f32_roundtrip");
    {
        /* IFFT(FFT(x)) == x for arbitrary real signal */
        numx_real_t orig[8] = {3,0, 1,0, 4,0, 1,0};
        numx_real_t buf[8];
        int i;
        for (i = 0; i < 8; i++) buf[i] = orig[i];
        numx_fft_f32(buf, 4);
        chk_rc(ctx, "ifft_f32 roundtrip rc", numx_ifft_f32(buf, 4), NUMX_OK);
        chk_f (ctx, "roundtrip re[0]",        buf[0], orig[0], 1e-3f);
        chk_f (ctx, "roundtrip re[1]",        buf[2], orig[2], 1e-3f);
        chk_f (ctx, "roundtrip re[2]",        buf[4], orig[4], 1e-3f);
        chk_f (ctx, "roundtrip re[3]",        buf[6], orig[6], 1e-3f);
    }

    /* ── fft_magnitude ───────────────────────────────────────────── */
    suite("fft / magnitude");
    {
        /* DC input N=4: after FFT, X[0]=4; mag[0]=4, mag[1..2]≈0 */
        numx_real_t buf[8] = {1,0, 1,0, 1,0, 1,0};
        numx_real_t mag[3];
        numx_fft_f32(buf, 4);
        chk_rc(ctx, "magnitude dc rc",        numx_fft_magnitude(buf, 4, mag), NUMX_OK);
        chk_f (ctx, "magnitude dc mag[0]=4",  mag[0], 4.0f, 1e-3f);
        chk_f (ctx, "magnitude dc mag[1]≈0",  mag[1], 0.0f, 1e-3f);
        chk_f (ctx, "magnitude dc mag[2]≈0",  mag[2], 0.0f, 1e-3f);

        chk_rc(ctx, "magnitude null-fft",     numx_fft_magnitude(NULL, 4, mag), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "magnitude null-mag",     numx_fft_magnitude(buf,  4, NULL),NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "magnitude n=1",          numx_fft_magnitude(buf,  1, mag), NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "magnitude n=3 (non-pow2)",numx_fft_magnitude(buf, 3, mag), NUMX_ERR_INVALID_ARG);
    }

    /* ── fft_q15 ─────────────────────────────────────────────────── */
    suite("fft / q15");
    {
        /*
         * N=2, DC input [A,0, A,0] in Q15.
         * One butterfly stage with >>1 shift:
         *   X[0] = (A+A)>>1 = A,  X[1] = (A-A)>>1 = 0
         */
        numx_q15_t buf[4] = {0x4000, 0, 0x4000, 0};
        chk_rc(ctx, "fft_q15 dc n=2 rc",      numx_fft_q15(buf, 2), NUMX_OK);
        chk_true(ctx, "fft_q15 dc X[0].re>0", buf[0] > 0);
        chk_true(ctx, "fft_q15 dc X[1].re=0", buf[2] == 0);

        /* impulse [0x7FFF,0, 0,0] N=2: X[0]=0x7FFF>>1, X[1]=0x7FFF>>1 */
        numx_q15_t ibuf[4] = {0x7FFF, 0, 0, 0};
        numx_fft_q15(ibuf, 2);
        chk_true(ctx, "fft_q15 impulse X[0]>0", ibuf[0] > 0);
        chk_true(ctx, "fft_q15 impulse X[1]>0", ibuf[2] > 0);

        chk_rc(ctx, "fft_q15 null",           numx_fft_q15(NULL, 4),    NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "fft_q15 n=0",            numx_fft_q15(buf,  0),    NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "fft_q15 n=3 (non-pow2)", numx_fft_q15(buf,  3),    NUMX_ERR_INVALID_ARG);
    }

    /* ── fft_f32 / ifft_f32: error cases ────────────────────────── */
    suite("fft / f32_errors");
    {
        numx_real_t buf[8] = {1,0, 0,0, 0,0, 0,0};
        chk_rc(ctx, "fft_f32 null",        numx_fft_f32(NULL, 4),                   NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "fft_f32 n=0",         numx_fft_f32(buf,  0),                   NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "fft_f32 n=3",         numx_fft_f32(buf,  3),                   NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "fft_f32 n>max",       numx_fft_f32(buf,  NUMX_MAX_FFT_SIZE*2), NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "ifft_f32 null",       numx_ifft_f32(NULL, 4),                  NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "ifft_f32 n=0",        numx_ifft_f32(buf,  0),                  NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "ifft_f32 n=3",        numx_ifft_f32(buf,  3),                  NUMX_ERR_INVALID_ARG);
    }
}
