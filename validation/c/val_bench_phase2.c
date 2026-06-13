/**
 * val_bench_phase2.c — Phase 2/3 timing benchmarks (autodiff, fft, signal, sketch, cs).
 * Build:  cmake --build build --target numx_val_bench_phase2
 * Run:    ./build/numx_val_bench_phase2
 */

#ifndef _WIN32
#  define _POSIX_C_SOURCE 199309L
#endif
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#ifdef _WIN32
#  include <windows.h>
#endif

#include "numx/signal.h"
#include "numx/fft.h"
#include "numx/autodiff.h"
#include "numx/compressed_sensing.h"
#include "numx/sketch.h"

static volatile float g_sink = 0.0f;

#ifdef _WIN32
static long long now_ns(void)
{
    LARGE_INTEGER freq, count;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&count);
    return (long long)(count.QuadPart * 1000000000LL / freq.QuadPart);
}
#else
static long long now_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000000000LL + (long long)ts.tv_nsec;
}
#endif

static void bench(const char *label, int N, long long total_ns)
{
    printf("%-52s | N=%7d | total=%8lld us | per_call=%7lld ns\n",
           label, N, total_ns / 1000LL, total_ns / (long long)N);
}

/* ── AUTODIFF ─────────────────────────────────────────────────────── */
static void bench_autodiff(void)
{
    puts("\n--- AUTODIFF ---");
    long long t0, dt;
    int N, i, d;

    /* forward mul-chain depth 10 */
    N = 100000;
    t0 = now_ns();
    for (i = 0; i < N; i++) {
        numx_dual_t x = numx_dual_var(2.0f);
        numx_dual_t r = x;
        for (d = 0; d < 10; d++) r = numx_dual_mul(r, x);
        g_sink = r.re;
    }
    dt = now_ns() - t0;
    bench("dual fwd: mul-chain depth=10", N, dt);

    /* reverse grad x^2 + y^2 */
    N = 10000;
    t0 = now_ns();
    for (i = 0; i < N; i++) {
        numx_ad_tape_t tape;
        numx_size_t xi, yi, x2i, y2i, zi;
        numx_ad_init(&tape);
        numx_ad_var(&tape, 3.0f, &xi);
        numx_ad_var(&tape, 4.0f, &yi);
        numx_ad_mul(&tape, xi, xi, &x2i);
        numx_ad_mul(&tape, yi, yi, &y2i);
        numx_ad_add(&tape, x2i, y2i, &zi);
        numx_ad_backward(&tape, zi);
        g_sink = numx_ad_grad(&tape, xi);
    }
    dt = now_ns() - t0;
    bench("tape: rev grad x2+y2", N, dt);

    /* reverse grad x^2 + y^2 + z^2 (3 vars) */
    N = 10000;
    t0 = now_ns();
    for (i = 0; i < N; i++) {
        numx_ad_tape_t tape;
        numx_size_t xi, yi, zvi, x2i, y2i, z2i, s1, s2;
        numx_ad_init(&tape);
        numx_ad_var(&tape, 3.0f, &xi);
        numx_ad_var(&tape, 4.0f, &yi);
        numx_ad_var(&tape, 5.0f, &zvi);
        numx_ad_mul(&tape, xi,  xi,  &x2i);
        numx_ad_mul(&tape, yi,  yi,  &y2i);
        numx_ad_mul(&tape, zvi, zvi, &z2i);
        numx_ad_add(&tape, x2i, y2i, &s1);
        numx_ad_add(&tape, s1,  z2i, &s2);
        numx_ad_backward(&tape, s2);
        g_sink = numx_ad_grad(&tape, xi);
    }
    dt = now_ns() - t0;
    bench("tape: rev grad x2+y2+z2 (3 vars)", N, dt);
}

/* ── FFT ──────────────────────────────────────────────────────────── */
static void bench_fft(void)
{
    puts("\n--- FFT ---");
    long long t0, dt;
    int N, i, k;

    /* fft_f32 N=64 — interleaved float[2*64] */
    static float buf64[128];
    N = 10000;
    for (k = 0; k < 128; k++) buf64[k] = (k % 2 == 0) ? 1.0f : 0.0f;
    t0 = now_ns();
    for (i = 0; i < N; i++) {
        float tmp[128];
        memcpy(tmp, buf64, sizeof(tmp));
        numx_fft_f32(tmp, 64);
        g_sink = tmp[0];
    }
    dt = now_ns() - t0;
    bench("fft_f32 N=64", N, dt);

    /* fft_f32 N=256 */
    static float buf256[512];
    N = 5000;
    for (k = 0; k < 512; k++) buf256[k] = (k % 2 == 0) ? 1.0f : 0.0f;
    t0 = now_ns();
    for (i = 0; i < N; i++) {
        float tmp[512];
        memcpy(tmp, buf256, sizeof(tmp));
        numx_fft_f32(tmp, 256);
        g_sink = tmp[0];
    }
    dt = now_ns() - t0;
    bench("fft_f32 N=256", N, dt);

    /* fft_f32 N=512 */
    static float buf512[1024];
    N = 1000;
    for (k = 0; k < 1024; k++) buf512[k] = (k % 2 == 0) ? 1.0f : 0.0f;
    t0 = now_ns();
    for (i = 0; i < N; i++) {
        float tmp[1024];
        memcpy(tmp, buf512, sizeof(tmp));
        numx_fft_f32(tmp, 512);
        g_sink = tmp[0];
    }
    dt = now_ns() - t0;
    bench("fft_f32 N=512", N, dt);

    /* ifft_f32 N=256 */
    static float dc256[512];
    N = 5000;
    for (k = 0; k < 512; k++) dc256[k] = 0.0f;
    dc256[0] = 256.0f;
    t0 = now_ns();
    for (i = 0; i < N; i++) {
        float tmp[512];
        memcpy(tmp, dc256, sizeof(tmp));
        numx_ifft_f32(tmp, 256);
        g_sink = tmp[0];
    }
    dt = now_ns() - t0;
    bench("ifft_f32 N=256", N, dt);

    /* fft_q15 N=256 — interleaved int16_t[2*256] */
    static int16_t qbuf[512];
    N = 5000;
    for (k = 0; k < 512; k++) qbuf[k] = 0;
    qbuf[0] = 16384;
    t0 = now_ns();
    for (i = 0; i < N; i++) {
        int16_t tmp[512];
        memcpy(tmp, qbuf, sizeof(tmp));
        numx_fft_q15(tmp, 256);
        g_sink = tmp[0];
    }
    dt = now_ns() - t0;
    bench("fft_q15 N=256", N, dt);

    /* fft_magnitude N=256 — input interleaved float[512], output float[129] */
    static float spec[512], mag[129];
    N = 100000;
    for (k = 0; k < 512; k++) spec[k] = 1.0f;
    t0 = now_ns();
    for (i = 0; i < N; i++) {
        numx_fft_magnitude(spec, 256, mag);
        g_sink = mag[0];
    }
    dt = now_ns() - t0;
    bench("fft_magnitude N=256", N, dt);
}

/* ── SIGNAL ───────────────────────────────────────────────────────── */
static void bench_signal(void)
{
    puts("\n--- SIGNAL ---");
    long long t0, dt;
    int N, i, k;

    /* window_hann n=512 */
    static float wbuf[512];
    N = 100000;
    t0 = now_ns();
    for (i = 0; i < N; i++) {
        numx_signal_window_hann(512, wbuf);
        g_sink = wbuf[0];
    }
    dt = now_ns() - t0;
    bench("signal_window_hann n=512", N, dt);

    /* convolve xn=256 hn=32 — output length = 256+32-1 = 287 */
    static float x256[256], h32[32], out287[287];
    for (k = 0; k < 256; k++) x256[k] = (float)k;
    for (k = 0; k < 32;  k++) h32[k]  = 1.0f / 32.0f;
    N = 10000;
    t0 = now_ns();
    for (i = 0; i < N; i++) {
        numx_signal_convolve(x256, 256, h32, 32, out287);
        g_sink = out287[0];
    }
    dt = now_ns() - t0;
    bench("signal_convolve xn=256 hn=32", N, dt);

    /* fir xn=256 ntaps=32 */
    static float fir_out[256];
    N = 10000;
    t0 = now_ns();
    for (i = 0; i < N; i++) {
        numx_signal_fir(x256, 256, h32, 32, fir_out);
        g_sink = fir_out[0];
    }
    dt = now_ns() - t0;
    bench("signal_fir xn=256 ntaps=32", N, dt);

    /* iir_biquad n=256 — all-pass */
    static float iir_b[3] = {1.0f, 0.0f, 0.0f};
    static float iir_a[2] = {0.0f, 0.0f};
    static float iir_out[256];
    N = 50000;
    t0 = now_ns();
    for (i = 0; i < N; i++) {
        numx_signal_iir_biquad(x256, 256, iir_b, iir_a, iir_out);
        g_sink = iir_out[0];
    }
    dt = now_ns() - t0;
    bench("signal_iir_biquad n=256", N, dt);

    /* ema n=256 alpha=0.1 */
    static float ema_out[256];
    N = 50000;
    t0 = now_ns();
    for (i = 0; i < N; i++) {
        numx_signal_ema(x256, 256, 0.1f, ema_out);
        g_sink = ema_out[0];
    }
    dt = now_ns() - t0;
    bench("signal_ema n=256 alpha=0.1", N, dt);
}

/* ── SKETCH ───────────────────────────────────────────────────────── */
static void bench_sketch(void)
{
    puts("\n--- SKETCH ---");
    long long t0, dt;
    int N, i, k;

    /* 16×16 rank=4 os=4 */
    static float A16[16*16], U16[16*4], S4[4], Vt16[4*16];
    for (k = 0; k < 16*16; k++) A16[k] = (k % 17 == 0) ? (float)(k/17+1) : 0.0f;
    N = 100;
    t0 = now_ns();
    for (i = 0; i < N; i++) {
        numx_sketch_rsvd(A16, 16, 16, 4, 4, (uint32_t)(i+1), U16, S4, Vt16);
        g_sink = S4[0];
    }
    dt = now_ns() - t0;
    bench("sketch_rsvd 16x16 rank=4 os=4", N, dt);

    /* 32×32 rank=8 os=4 */
    static float A32[32*32], U32[32*8], S8[8], Vt32[8*32];
    for (k = 0; k < 32*32; k++) A32[k] = (k % 33 == 0) ? (float)(k/33+1) : 0.0f;
    N = 100;
    t0 = now_ns();
    for (i = 0; i < N; i++) {
        numx_sketch_rsvd(A32, 32, 32, 8, 4, (uint32_t)(i+1), U32, S8, Vt32);
        g_sink = S8[0];
    }
    dt = now_ns() - t0;
    bench("sketch_rsvd 32x32 rank=8 os=4", N, dt);

    /* 64×64 rank=8 os=4 */
    static float A64[64*64], U64[64*8], Vt64[8*64];
    for (k = 0; k < 64*64; k++) A64[k] = (k % 65 == 0) ? (float)(k/65+1) : 0.0f;
    N = 100;
    t0 = now_ns();
    for (i = 0; i < N; i++) {
        numx_sketch_rsvd(A64, 64, 64, 8, 4, (uint32_t)(i+1), U64, S8, Vt64);
        g_sink = S8[0];
    }
    dt = now_ns() - t0;
    bench("sketch_rsvd 64x64 rank=8 os=4", N, dt);
}

/* ── COMPRESSED SENSING ───────────────────────────────────────────── */
static void bench_cs(void)
{
    puts("\n--- COMPRESSED SENSING ---");
    long long t0, dt;
    int N, i, k;

    static float A1632[16*32], y16[16], x32[32], sn_out;

    /* sensing matrix: identity-like entries spread across 16×32 */
    for (k = 0; k < 16*32; k++) A1632[k] = 0.0f;
    for (k = 0; k < 16; k++) A1632[k*32 + k] = 1.0f;  /* identity sub-block */
    for (k = 0; k < 16; k++) y16[k] = (float)(k % 4);

    /* spectral_norm 16×32 iter=32 */
    N = 100;
    t0 = now_ns();
    for (i = 0; i < N; i++) {
        numx_cs_spectral_norm(A1632, 16, 32, 32, &sn_out);
        g_sink = sn_out;
    }
    dt = now_ns() - t0;
    bench("cs_spectral_norm 16x32 iter=32", N, dt);

    /* omp A=16×32 k=4 */
    N = 100;
    t0 = now_ns();
    for (i = 0; i < N; i++) {
        numx_cs_omp(A1632, y16, 16, 32, 4, x32);
        g_sink = x32[0];
    }
    dt = now_ns() - t0;
    bench("cs_omp 16x32 k=4", N, dt);

    /* ista A=16×32 lam=0.1 step=0.9 iter=500 */
    N = 100;
    t0 = now_ns();
    for (i = 0; i < N; i++) {
        memset(x32, 0, sizeof(x32));
        numx_cs_ista(A1632, y16, 16, 32, 0.1f, 0.9f, 500, x32);
        g_sink = x32[0];
    }
    dt = now_ns() - t0;
    bench("cs_ista 16x32 lam=0.1 iter=500", N, dt);
}

int main(void)
{
    printf("numx Phase 2/3 timing benchmark\n");
    printf("Platform: ARM64 / macOS 26.2 / Apple clang 21.0.0 / float32\n");
    printf("=================================================\n");
    bench_autodiff();
    bench_fft();
    bench_signal();
    bench_sketch();
    bench_cs();
    printf("\nDone.\n");
    return 0;
}
