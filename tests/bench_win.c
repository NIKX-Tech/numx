/**
 * @file bench_win.c
 * @brief Windows performance benchmarks — all 13 numx modules.
 *        Works for both float32 (x86 build) and float64 (x64 build).
 *        Uses QueryPerformanceCounter for sub-microsecond resolution.
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "numx/linalg.h"
#include "numx/stats.h"
#include "numx/roots.h"
#include "numx/integrate.h"
#include "numx/differentiate.h"
#include "numx/interpolate.h"
#include "numx/poly.h"
#include "numx/ode.h"
#include "numx/autodiff.h"
#include "numx/fft.h"
#include "numx/signal.h"
#include "numx/sketch.h"
#include "numx/compressed_sensing.h"

/* ── timing ─────────────────────────────────────────────────────────── */

static LARGE_INTEGER s_freq;

static void bench_init(void)
{
    QueryPerformanceFrequency(&s_freq);
}

static int64_t now_us(void)
{
    LARGE_INTEGER t;
    QueryPerformanceCounter(&t);
    return (int64_t)(t.QuadPart * 1000000LL / s_freq.QuadPart);
}

/* ── output ──────────────────────────────────────────────────────────── */

static void bprint(const char *lbl, int64_t us, int n)
{
    int64_t ns = (us * 1000LL) / n;
    printf("| %-44s | %6d | %8lld us | %7lld ns |\n",
           lbl, n, (long long)us, (long long)ns);
}

static void bench_section(const char *target_md)
{
    printf("\n  >>> results/%s\n", target_md);
    printf("  | %-44s | N      | Total us | ns/call  |\n", "Function");
    printf("  |%-46s|--------|----------|----------|\n",
           "----------------------------------------------");
}

#define BENCH(lbl, n, body) do { \
    int64_t _t0 = now_us(); \
    for (int _i = 0; _i < (n); _i++) { body; } \
    int64_t _t1 = now_us(); \
    printf("  "); bprint((lbl), _t1 - _t0, (n)); \
} while (0)

/* ── math helpers (match ESP32 bench / test helpers) ─────────────────── */

static numx_real_t f_quad(numx_real_t x)  { return x * x - (numx_real_t)4.0; }
static numx_real_t df_quad(numx_real_t x) { return (numx_real_t)2.0 * x; }
static numx_real_t f_cubic(numx_real_t x) { return x * x * x - x; }
static numx_real_t f_xsq(numx_real_t x)   { return x * x; }
static numx_real_t f_x3(numx_real_t x)    { return x * x * x; }
static numx_real_t f_sq(numx_real_t x)    { return x * x; }

static numx_status_t ode_decay(numx_real_t t, const numx_real_t *y,
                                numx_size_t n, numx_real_t *dydt)
{
    (void)t; (void)n;
    dydt[0] = -y[0];
    return NUMX_OK;
}

static numx_status_t ode_harm(numx_real_t t, const numx_real_t *y,
                               numx_size_t n, numx_real_t *dydt)
{
    (void)t; (void)n;
    dydt[0] =  y[1];
    dydt[1] = -y[0];
    return NUMX_OK;
}

/* ── autodiff tape: static to avoid stack overflow (82 KB) ─────────── */
static numx_ad_tape_t s_tape;

static void ad_bench_cycle(void)
{
    numx_size_t ix, ix2, isin, iout;
    numx_ad_init(&s_tape);
    numx_ad_var(&s_tape, (numx_real_t)2.0, &ix);
    numx_ad_mul(&s_tape, ix, ix, &ix2);
    numx_ad_sin(&s_tape, ix2, &isin);
    numx_ad_backward(&s_tape, isin);
    (void)iout;
}

/* ═══════════════════════════════════════════════════════════════════════
 * Main benchmark entry point
 * ═══════════════════════════════════════════════════════════════════════ */

int main(void)
{
    bench_init();

#ifdef NUMX_USE_DOUBLE
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  Windows x64 Benchmarks (MSVC, float64 / double)            ║\n");
    printf("║  Rows formatted as markdown table rows.                      ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
#else
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  Windows x86 Benchmarks (MSVC, float32 / float)             ║\n");
    printf("║  Rows formatted as markdown table rows.                      ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
#endif

    numx_real_t r;

    /* ══════════════════════════════════════════════════════════════════
     * linalg
     * ══════════════════════════════════════════════════════════════════ */
    printf("\n[linalg]\n");

    numx_real_t a64[64], b64[64];
    for (int i = 0; i < 64; i++) {
        a64[i] = (numx_real_t)i;
        b64[i] = (numx_real_t)(63 - i);
    }

    bench_section("linalg/vec_dot.md");
    BENCH("vec_dot n=64", 100000, numx_vec_dot(a64, b64, 64, &r));

    bench_section("linalg/vec_norm.md");
    BENCH("vec_norm L2 n=64", 100000, numx_vec_norm(a64, 64, NUMX_NORM_L2, &r));
    BENCH("vec_norm L1 n=64", 100000, numx_vec_norm(a64, 64, NUMX_NORM_L1, &r));

    bench_section("linalg/vec_cross3.md");
    numx_real_t ax[3] = {(numx_real_t)1, (numx_real_t)2, (numx_real_t)3};
    numx_real_t bx[3] = {(numx_real_t)4, (numx_real_t)5, (numx_real_t)6};
    numx_real_t cx[3];
    BENCH("vec_cross3", 100000, numx_vec_cross3(ax, bx, cx));

    bench_section("linalg/mat_mul.md");
    numx_real_t A4[16], B4[16], C4[16];
    for (int i = 0; i < 16; i++) { A4[i] = (numx_real_t)i; B4[i] = (numx_real_t)(15 - i); }
    BENCH("mat_mul 4x4", 100000, numx_mat_mul(A4, 4, 4, B4, 4, 4, C4));
    numx_real_t A8[64], B8[64], C8[64], AT8[64];
    for (int i = 0; i < 64; i++) { A8[i] = (numx_real_t)i * (numx_real_t)0.1; B8[i] = (numx_real_t)(63 - i) * (numx_real_t)0.1; }
    BENCH("mat_mul 8x8", 10000, numx_mat_mul(A8, 8, 8, B8, 8, 8, C8));

    bench_section("linalg/mat_transpose.md");
    BENCH("mat_transpose 8x8",    100000, numx_mat_transpose(A8, 8, 8, AT8));
    BENCH("mat_transpose_sq 8x8", 100000, numx_mat_transpose_sq(A8, 8));

    bench_section("linalg/mat_det.md");
    numx_real_t Md[16] = {
        (numx_real_t)4,(numx_real_t)3,(numx_real_t)2,(numx_real_t)1,
        (numx_real_t)3,(numx_real_t)4,(numx_real_t)3,(numx_real_t)2,
        (numx_real_t)2,(numx_real_t)3,(numx_real_t)4,(numx_real_t)3,
        (numx_real_t)1,(numx_real_t)2,(numx_real_t)3,(numx_real_t)4
    };
    BENCH("mat_det 4x4", 100000, numx_mat_det(Md, 4, &r));

    bench_section("linalg/lu_solve.md");
    numx_real_t Alu[16] = {
        (numx_real_t)2,(numx_real_t)1,(numx_real_t)1,(numx_real_t)0,
        (numx_real_t)4,(numx_real_t)3,(numx_real_t)3,(numx_real_t)1,
        (numx_real_t)8,(numx_real_t)7,(numx_real_t)9,(numx_real_t)5,
        (numx_real_t)6,(numx_real_t)7,(numx_real_t)9,(numx_real_t)8
    };
    numx_real_t LU[16], bv[4] = {(numx_real_t)1,(numx_real_t)2,(numx_real_t)4,(numx_real_t)5}, xv[4];
    numx_idx_t  piv[4];
    numx_lu_decompose(Alu, 4, LU, piv);
    BENCH("lu_decompose 4x4",         100000, numx_lu_decompose(Alu, 4, LU, piv));
    BENCH("lu_solve 4x4 (factored)",  100000, numx_lu_solve(LU, piv, 4, bv, xv));

    /* ══════════════════════════════════════════════════════════════════
     * stats
     * ══════════════════════════════════════════════════════════════════ */
    printf("\n[stats]\n");

    bench_section("stats/stats.md");
    numx_real_t data[128];
    for (int i = 0; i < 128; i++) data[i] = (numx_real_t)(i + 1) * (numx_real_t)0.5;
    BENCH("stats_mean n=128",            100000, numx_stats_mean(data, 128, &r));
    BENCH("stats_variance pop n=128",    50000,  numx_stats_variance(data, 128, NUMX_VAR_POPULATION, &r));
    BENCH("stats_variance sample n=128", 50000,  numx_stats_variance(data, 128, NUMX_VAR_SAMPLE, &r));
    BENCH("stats_median n=128",          10000,  numx_stats_median(data, 128, &r));
    BENCH("stats_percentile p50 n=128",  10000,  numx_stats_percentile(data, 128, (numx_real_t)50.0, &r));
    BENCH("stats_percentile p95 n=128",  10000,  numx_stats_percentile(data, 128, (numx_real_t)95.0, &r));

    /* ══════════════════════════════════════════════════════════════════
     * roots
     * ══════════════════════════════════════════════════════════════════ */
    printf("\n[roots]\n");

    bench_section("roots/roots.md");
    BENCH("root_bisect x^2-4 tol=1e-5", 10000, numx_root_bisect(f_quad, (numx_real_t)0, (numx_real_t)5, (numx_real_t)1e-5, &r));
    BENCH("root_newton x^2-4 tol=1e-5", 10000, numx_root_newton(f_quad, df_quad, (numx_real_t)3, (numx_real_t)1e-5, &r));
    BENCH("root_brent  x^2-4 tol=1e-5", 10000, numx_root_brent(f_quad, (numx_real_t)0, (numx_real_t)5, (numx_real_t)1e-5, &r));
    BENCH("root_brent  x^3-x tol=1e-5", 10000, numx_root_brent(f_cubic, (numx_real_t)0.5, (numx_real_t)1.5, (numx_real_t)1e-5, &r));

    /* ══════════════════════════════════════════════════════════════════
     * integrate
     * ══════════════════════════════════════════════════════════════════ */
    printf("\n[integrate]\n");

    bench_section("integrate/integrate.md");
    BENCH("integrate_trap n=100",     50000, numx_integrate_trap(f_xsq, (numx_real_t)0, (numx_real_t)1, 100, &r));
    BENCH("integrate_trap n=1000",    10000, numx_integrate_trap(f_xsq, (numx_real_t)0, (numx_real_t)1, 1000, &r));
    BENCH("integrate_simpson n=100",  50000, numx_integrate_simpson(f_xsq, (numx_real_t)0, (numx_real_t)1, 100, &r));
    BENCH("integrate_gauss npts=2",   50000, numx_integrate_gauss(f_xsq, (numx_real_t)0, (numx_real_t)1, 2, &r));
    BENCH("integrate_gauss npts=4",   50000, numx_integrate_gauss(f_xsq, (numx_real_t)0, (numx_real_t)1, 4, &r));
    BENCH("integrate_gauss npts=8",   50000, numx_integrate_gauss(f_xsq, (numx_real_t)0, (numx_real_t)1, 8, &r));

    /* ══════════════════════════════════════════════════════════════════
     * differentiate
     * ══════════════════════════════════════════════════════════════════ */
    printf("\n[differentiate]\n");

    bench_section("differentiate/differentiate.md");
    BENCH("diff_forward  h=1e-3",   100000, numx_diff_forward(f_x3, (numx_real_t)2.0, (numx_real_t)1e-3, &r));
    BENCH("diff_central  h=1e-3",   100000, numx_diff_central(f_x3, (numx_real_t)2.0, (numx_real_t)1e-3, &r));
    BENCH("diff_richardson h=1e-3", 100000, numx_diff_richardson(f_x3, (numx_real_t)2.0, (numx_real_t)1e-3, &r));

    /* ══════════════════════════════════════════════════════════════════
     * interpolate
     * ══════════════════════════════════════════════════════════════════ */
    printf("\n[interpolate]\n");

    bench_section("interpolate/interpolate.md");
    numx_real_t xsi[16], ysi[16], mi[16];
    for (int i = 0; i < 16; i++) {
        xsi[i] = (numx_real_t)i * (numx_real_t)(3.14159265 / 15.0);
        ysi[i] = xsi[i] * xsi[i];
    }
    BENCH("interp_linear n=16",            50000, numx_interp_linear(xsi, ysi, 16, (numx_real_t)1.5, &r));
    BENCH("interp_spline_cubic n=16",      50000, numx_interp_spline_cubic(xsi, ysi, 16, (numx_real_t)1.5, &r));
    numx_interp_spline_precompute(xsi, ysi, 16, mi);
    BENCH("interp_spline_eval (precomp)",  50000, numx_interp_spline_eval(xsi, ysi, mi, 16, (numx_real_t)1.5, &r));
    BENCH("interp_spline_precompute n=16", 50000, numx_interp_spline_precompute(xsi, ysi, 16, mi));
    BENCH("interp_chebyshev n=8",          50000, numx_interp_chebyshev(f_sq, 8, (numx_real_t)0, (numx_real_t)3.14, (numx_real_t)1.5, &r));
    BENCH("interp_chebyshev n=16",         10000, numx_interp_chebyshev(f_sq, 16, (numx_real_t)0, (numx_real_t)3.14, (numx_real_t)1.5, &r));

    /* ══════════════════════════════════════════════════════════════════
     * poly
     * ══════════════════════════════════════════════════════════════════ */
    printf("\n[poly]\n");

    bench_section("poly/poly.md");
    numx_real_t cp8[9] = {
        (numx_real_t)1,(numx_real_t)0,(numx_real_t)0,(numx_real_t)0,
        (numx_real_t)-3,(numx_real_t)0,(numx_real_t)0,(numx_real_t)0,(numx_real_t)2
    };
    numx_real_t cp3[4] = {(numx_real_t)1,(numx_real_t)-6,(numx_real_t)11,(numx_real_t)-6};
    numx_real_t pro[3]; numx_size_t pnr;
    BENCH("poly_eval degree=8",           100000, numx_poly_eval(cp8, 8, (numx_real_t)1.5, &r));
    BENCH("poly_eval degree=3",           100000, numx_poly_eval(cp3, 3, (numx_real_t)2.5, &r));
    BENCH("poly_roots degree=3 tol=1e-4",   1000, numx_poly_roots(cp3, 3, pro, &pnr, (numx_real_t)1e-4));

    /* ══════════════════════════════════════════════════════════════════
     * ode
     * ══════════════════════════════════════════════════════════════════ */
    printf("\n[ode]\n");

    bench_section("ode/ode.md");
    numx_real_t yo[1]  = {(numx_real_t)1.0};
    numx_real_t yho[2] = {(numx_real_t)1.0, (numx_real_t)0.0};
    numx_real_t yor[1], yor2[2];
    BENCH("ode_rk4  decay   n=1 100-steps",  1000, numx_ode_rk4(ode_decay, (numx_real_t)0, yo,  1, (numx_real_t)0.01, 100, yor));
    BENCH("ode_rk4  harmonic n=2 100-steps", 1000, numx_ode_rk4(ode_harm,  (numx_real_t)0, yho, 2, (numx_real_t)0.01, 100, yor2));
    BENCH("ode_rk45 decay   n=1 tol=1e-4",  1000, numx_ode_rk45(ode_decay, (numx_real_t)0, (numx_real_t)1.0, yo,  1, (numx_real_t)1e-4, yor));
    BENCH("ode_rk45 harmonic n=2 tol=1e-4", 1000, numx_ode_rk45(ode_harm,  (numx_real_t)0, (numx_real_t)1.0, yho, 2, (numx_real_t)1e-4, yor2));

    /* ══════════════════════════════════════════════════════════════════
     * autodiff
     * ══════════════════════════════════════════════════════════════════ */
    printf("\n[autodiff]\n");

    bench_section("autodiff/autodiff.md");
    numx_dual_t dx = numx_dual_var((numx_real_t)2.0);
    numx_dual_t dc = numx_dual_const((numx_real_t)2.0);
    BENCH("dual_mul (x*x fwd-mode)",       100000, numx_dual_mul(dx, dx));
    BENCH("dual_sin (sin(x) fwd-mode)",    100000, numx_dual_sin(dx));
    BENCH("dual fwd: sin(x^2) full expr",  100000, numx_dual_sin(numx_dual_mul(dx, dx)));
    BENCH("tape: init+x,x^2,sin+bwd",      10000, ad_bench_cycle());
    (void)dc;

    /* ══════════════════════════════════════════════════════════════════
     * fft
     * ══════════════════════════════════════════════════════════════════ */
    printf("\n[fft]\n");

    bench_section("fft/fft.md");
    static numx_real_t fft64[128], fft256[512], fft512[1024], fft256mag[256];
    static numx_q15_t  fftq256[512];
    for (int i = 0; i < 128; i++) fft64[i]   = (i & 1) ? (numx_real_t)0 : (numx_real_t)1;
    for (int i = 0; i < 512; i++) fft256[i]  = (i & 1) ? (numx_real_t)0 : (numx_real_t)1;
    for (int i = 0; i < 1024; i++) fft512[i] = (i & 1) ? (numx_real_t)0 : (numx_real_t)1;
    for (int i = 0; i < 512; i++) fftq256[i] = (i & 1) ? 0 : 16384;
    BENCH("fft_f32 N=64",       10000, numx_fft_f32(fft64,  64));
    BENCH("fft_f32 N=256",       5000, numx_fft_f32(fft256, 256));
    BENCH("fft_f32 N=512",       1000, numx_fft_f32(fft512, 512));
    BENCH("ifft_f32 N=256",      5000, numx_ifft_f32(fft256, 256));
    BENCH("fft_q15 N=256",       5000, numx_fft_q15(fftq256, 256));
    for (int i = 0; i < 256; i++) {
        fft256mag[i] = (numx_real_t)(i % 4 == 0 ? 1 : 0);
    }
    BENCH("fft_magnitude N=256", 100000, numx_fft_magnitude(fft256, 256, fft256mag));

    /* ══════════════════════════════════════════════════════════════════
     * signal
     * ══════════════════════════════════════════════════════════════════ */
    printf("\n[signal]\n");

    bench_section("signal/signal.md");
    static numx_real_t sig_win[64], sig_x64[64], sig_h8[8];
    static numx_real_t sig_out[128], sig_fir_out[64], sig_iir_out[128];
    static numx_real_t sig_ema_out[128];
    static numx_real_t sig_x128[128];
    static numx_idx_t  sig_peaks[32];
    static numx_size_t sig_npeaks;
    numx_real_t sig_biquad_b[3] = {(numx_real_t)1,(numx_real_t)0,(numx_real_t)0};
    numx_real_t sig_biquad_a[2] = {(numx_real_t)0,(numx_real_t)0};
    for (int i = 0; i < 64; i++) { sig_x64[i] = (numx_real_t)i; }
    for (int i = 0; i < 8;  i++) { sig_h8[i]  = (numx_real_t)(i + 1); }
    for (int i = 0; i < 128; i++) { sig_x128[i] = (numx_real_t)(i % 7); }

    BENCH("signal_window_hann n=64",       100000, numx_signal_window_hann(64, sig_win));
    BENCH("signal_convolve x=64 h=8",      10000,  numx_signal_convolve(sig_x64, 64, sig_h8, 8, sig_out));
    BENCH("signal_correlate x=64 y=8",     10000,  numx_signal_correlate(sig_x64, 64, sig_h8, 8, sig_out));
    BENCH("signal_fir x=128 ntaps=8",      10000,  numx_signal_fir(sig_x128, 128, sig_h8, 8, sig_fir_out));
    BENCH("signal_iir_biquad n=128",       50000,  numx_signal_iir_biquad(sig_x128, 128, sig_biquad_b, sig_biquad_a, sig_iir_out));
    BENCH("signal_peaks n=64",             50000,  numx_signal_peaks(sig_x64, 64, sig_peaks, 32, &sig_npeaks));
    BENCH("signal_ema n=128 alpha=0.1",    50000,  numx_signal_ema(sig_x128, 128, (numx_real_t)0.1, sig_ema_out));

    /* ══════════════════════════════════════════════════════════════════
     * sketch
     * ══════════════════════════════════════════════════════════════════ */
    printf("\n[sketch]\n");

    bench_section("sketch/sketch.md");
    static numx_real_t sk_A88[64], sk_U88[16], sk_S88[2], sk_Vt88[16];
    static numx_real_t sk_A1616[256], sk_U1616[64], sk_S1616[4], sk_Vt1616[64];
    for (int i = 0; i < 64;  i++) sk_A88[i]   = (i == i/8*8+i%8) ? (numx_real_t)(i/8+1) : (numx_real_t)0;
    for (int i = 0; i < 256; i++) sk_A1616[i] = (i % 16 == i / 16) ? (numx_real_t)(i/16+1) : (numx_real_t)0;
    BENCH("sketch_rsvd 8x8 rank=2 os=2",   100, numx_sketch_rsvd(sk_A88,   8,  8,  2, 2, 42, sk_U88,   sk_S88,   sk_Vt88));
    BENCH("sketch_rsvd 16x16 rank=4 os=4", 100, numx_sketch_rsvd(sk_A1616, 16, 16, 4, 4, 42, sk_U1616, sk_S1616, sk_Vt1616));

    /* ══════════════════════════════════════════════════════════════════
     * compressed_sensing
     * ══════════════════════════════════════════════════════════════════ */
    printf("\n[compressed_sensing]\n");

    bench_section("compressed_sensing/compressed_sensing.md");
    static numx_real_t cs_A[512], cs_y[16], cs_x[32];
    /* 16x32 sensing matrix: first 16 rows of identity-like structure */
    memset(cs_A, 0, sizeof(cs_A));
    for (int i = 0; i < 16; i++) {
        cs_A[i * 32 + i] = (numx_real_t)1.0;
        cs_A[i * 32 + i + 16] = (numx_real_t)0.5;
    }
    for (int i = 0; i < 16; i++) cs_y[i] = (numx_real_t)(i % 3 == 0 ? 1 : 0);

    numx_real_t cs_sigma;
    BENCH("cs_spectral_norm 16x32 iter=32", 100, numx_cs_spectral_norm(cs_A, 16, 32, 32, &cs_sigma));
    BENCH("cs_omp 16x32 k=4",              100, numx_cs_omp(cs_A, cs_y, 16, 32, 4, cs_x));
    BENCH("cs_ista 16x32 lam=0.1 iter=100", 100, numx_cs_ista(cs_A, cs_y, 16, 32, (numx_real_t)0.1, (numx_real_t)0.9, 100, cs_x));

    printf("\n--- done ---\n");
    return 0;
}
