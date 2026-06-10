#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "bench_common.h"
#include "numx/autodiff.h"
#include "numx/fft.h"
#include "numx/signal.h"
#include "numx/sketch.h"
#include "numx/compressed_sensing.h"

/* ── autodiff: tape is ~82 KB — must be static ──────────────────────── */
static numx_ad_tape_t s_ad_tape;

static void ad_bench_cycle(void)
{
    numx_size_t xi, qi, si;
    numx_ad_init(&s_ad_tape);
    numx_ad_var(&s_ad_tape, 1.5f, &xi);
    numx_ad_mul(&s_ad_tape, xi, xi, &qi);
    numx_ad_sin(&s_ad_tape, qi, &si);
    numx_ad_backward(&s_ad_tape, si);
}

/* ── fft buffers ─────────────────────────────────────────────────────── */
static numx_real_t fft64[128];      /* 64 complex pairs */
static numx_real_t fft256[512];     /* 256 complex pairs */
static numx_real_t fft512[1024];    /* 512 complex pairs */
static numx_q15_t  q15_256[512];    /* 256 complex Q15 pairs */
static numx_real_t fft_mag[257];    /* magnitude bins for n=256 */

/* ── signal buffers ──────────────────────────────────────────────────── */
static numx_real_t sig_x64[64];
static numx_real_t sig_h8[8];
static numx_real_t sig_conv_out[71];   /* 64 + 8 - 1 */
static numx_real_t sig_corr_out[71];
static numx_real_t sig_x128[128];
static numx_real_t sig_out128[128];
static numx_real_t sig_win64[64];
static numx_size_t sig_peaks[32];
static numx_size_t sig_npeaks;

static const numx_real_t fir_taps[8] = {
    0.0625f, 0.125f, 0.25f, 0.5f, 0.5f, 0.25f, 0.125f, 0.0625f
};
static const numx_real_t iir_b[3] = { 0.0675f,  0.1349f,  0.0675f };
static const numx_real_t iir_a[2] = {-1.1430f,  0.4128f };

/* ── sketch buffers ──────────────────────────────────────────────────── */
static numx_real_t sk_A8[64];    /* 8x8 input */
static numx_real_t sk_U8[16];    /* 8x2 */
static numx_real_t sk_S8[2];
static numx_real_t sk_Vt8[16];   /* 2x8 */

static numx_real_t sk_A16[256];  /* 16x16 input */
static numx_real_t sk_U16[64];   /* 16x4 */
static numx_real_t sk_S16[4];
static numx_real_t sk_Vt16[64];  /* 4x16 */

/* ── compressed-sensing buffers ──────────────────────────────────────── */
static numx_real_t cs_A[512];    /* 16x32 */
static numx_real_t cs_y[16];
static numx_real_t cs_x[32];
static numx_real_t cs_sigma;

void bench_phase2(void)
{
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  PHASE 2 — autodiff / fft / signal / sketch /               ║\n");
    printf("║            compressed_sensing                                ║\n");
    printf("║  ESP32-S3 @ 240 MHz · float32                               ║\n");
    printf("║  Copy each >>> block into the matching results/*.md file.   ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    /* ══════════════════════════════════════════════════════════════════ */
    printf("\n[autodiff]\n");

    bench_section("autodiff/autodiff.md");
    {
        numx_dual_t xd, sq, sn;
        BENCH("dual_mul (x*x fwd-mode)",       BENCH_N_BIG, (
            xd = numx_dual_var(1.5f),
            sq = numx_dual_mul(xd, xd),
            (void)sq));
        BENCH("dual_sin (sin(x) fwd-mode)",    BENCH_N_BIG, (
            xd = numx_dual_var(1.5f),
            sn = numx_dual_sin(xd),
            (void)sn));
        BENCH("dual fwd: sin(x^2) full expr",  BENCH_N_BIG, (
            xd = numx_dual_var(1.5f),
            sq = numx_dual_mul(xd, xd),
            sn = numx_dual_sin(sq),
            (void)sn));
        BENCH("tape: init+x,x^2,sin+bwd", BENCH_N_SMALL,
              ad_bench_cycle());
    }

    /* ══════════════════════════════════════════════════════════════════ */
    printf("\n[fft]\n");

    /* Initialise buffers: real signal, zero imaginary */
    for (int i = 0; i < 128; i++) fft64[i]  = (i % 2 == 0) ? (numx_real_t)(i/2 + 1) * 0.1f : 0.0f;
    for (int i = 0; i < 512; i++) fft256[i] = (i % 2 == 0) ? (numx_real_t)(i/2 + 1) * 0.1f : 0.0f;
    for (int i = 0; i < 1024; i++) fft512[i] = (i % 2 == 0) ? (numx_real_t)(i/2 + 1) * 0.1f : 0.0f;
    for (int i = 0; i < 512; i++) q15_256[i] = (numx_q15_t)((i % 2 == 0) ? 1000 : 0);

    bench_section("fft/fft.md");
    BENCH("fft_f32 N=64",         BENCH_N_MED,   numx_fft_f32(fft64,  64));
    BENCH("fft_f32 N=256",        BENCH_N_SMALL, numx_fft_f32(fft256, 256));
    BENCH("fft_f32 N=512",        BENCH_N_TINY,  numx_fft_f32(fft512, 512));
    BENCH("ifft_f32 N=256",       BENCH_N_SMALL, numx_ifft_f32(fft256, 256));
    BENCH("fft_q15 N=256",        BENCH_N_SMALL, numx_fft_q15(q15_256, 256));
    BENCH("fft_magnitude N=256",  BENCH_N_BIG,   numx_fft_magnitude(fft256, 256, fft_mag));

    /* ══════════════════════════════════════════════════════════════════ */
    printf("\n[signal]\n");

    /* Initialise buffers */
    for (int i = 0; i < 64; i++)  sig_x64[i]  = (numx_real_t)(i % 8) * 0.5f;
    for (int i = 0; i < 8; i++)   sig_h8[i]   = 0.125f;
    for (int i = 0; i < 128; i++) sig_x128[i] = (numx_real_t)(i % 16) * 0.25f;

    bench_section("signal/signal.md");
    BENCH("signal_window_hann n=64",      BENCH_N_BIG,
          numx_signal_window_hann(64, sig_win64));
    BENCH("signal_convolve x=64 h=8",     BENCH_N_SMALL,
          numx_signal_convolve(sig_x64, 64, sig_h8, 8, sig_conv_out));
    BENCH("signal_correlate x=64 y=8",    BENCH_N_SMALL,
          numx_signal_correlate(sig_x64, 64, sig_h8, 8, sig_corr_out));
    BENCH("signal_fir x=128 ntaps=8",     BENCH_N_SMALL,
          numx_signal_fir(sig_x128, 128, fir_taps, 8, sig_out128));
    BENCH("signal_iir_biquad n=128",      BENCH_N_MED,
          numx_signal_iir_biquad(sig_x128, 128, iir_b, iir_a, sig_out128));
    BENCH("signal_peaks n=64",            BENCH_N_MED,
          numx_signal_peaks(sig_x64, 64, sig_peaks, 32, &sig_npeaks));
    BENCH("signal_ema n=128 alpha=0.1",   BENCH_N_MED,
          numx_signal_ema(sig_x128, 128, 0.1f, sig_out128));

    /* ══════════════════════════════════════════════════════════════════ */
    printf("\n[sketch]\n");

    /* Initialise A matrices: diagonal-dominant values */
    for (int i = 0; i < 64; i++)  sk_A8[i]  = (i % 9 == 0) ? 2.0f : 0.1f;
    for (int i = 0; i < 256; i++) sk_A16[i] = (i % 17 == 0) ? 3.0f : 0.1f;

    bench_section("sketch/sketch.md");
    BENCH("sketch_rsvd 8x8 rank=2 os=2",   BENCH_N_TINY,
          numx_sketch_rsvd(sk_A8, 8, 8, 2, 2, 42u, sk_U8, sk_S8, sk_Vt8));
    BENCH("sketch_rsvd 16x16 rank=4 os=4", BENCH_N_TINY,
          numx_sketch_rsvd(sk_A16, 16, 16, 4, 4, 42u, sk_U16, sk_S16, sk_Vt16));

    /* ══════════════════════════════════════════════════════════════════ */
    printf("\n[compressed_sensing]\n");

    /* Initialise A as a simple Bernoulli-like matrix, y as sparse signal */
    for (int i = 0; i < 512; i++) cs_A[i] = (i % 17 == 0) ? 1.0f : 0.1f;
    memset(cs_y, 0, sizeof(cs_y));
    cs_y[0] = 1.0f;
    cs_y[3] = 0.5f;

    bench_section("compressed_sensing/compressed_sensing.md");
    BENCH("cs_spectral_norm 16x32 iter=32", BENCH_N_TINY,
          numx_cs_spectral_norm(cs_A, 16, 32, 32, &cs_sigma));
    memset(cs_x, 0, sizeof(cs_x));
    BENCH("cs_omp 16x32 k=4",               BENCH_N_TINY,
          numx_cs_omp(cs_A, cs_y, 16, 32, 4, cs_x));
    memset(cs_x, 0, sizeof(cs_x));
    BENCH("cs_ista 16x32 lam=0.1 iter=100", BENCH_N_TINY,
          numx_cs_ista(cs_A, cs_y, 16, 32, 0.1f, 0.05f, 100, cs_x));
}
