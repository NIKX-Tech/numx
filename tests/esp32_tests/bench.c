#include <stdio.h>
#include "esp_timer.h"
#include "math_helpers.h"
#include "numx/linalg.h"
#include "numx/stats.h"
#include "numx/roots.h"
#include "numx/integrate.h"
#include "numx/differentiate.h"
#include "numx/interpolate.h"
#include "numx/poly.h"
#include "numx/ode.h"

#define BENCH_N_BIG   10000
#define BENCH_N_MED    5000
#define BENCH_N_SMALL  1000
#define BENCH_N_TINY    100

/* Print one markdown table row: | function | N | total_us | ns/call | */
static void bprint(const char *lbl, int64_t us, int n)
{
    int64_t ns = (us * 1000LL) / n;
    printf("| %-44s | %6d | %8lld us | %7lld ns |\n",
           lbl, n, (long long)us, (long long)ns);
}

/* Print section header pointing at the target .md file */
static void bench_section(const char *target_md)
{
    printf("\n  >>> results/%s\n", target_md);
    printf("  | %-44s | N      | Total us | ns/call  |\n",
           "Function");
    printf("  |%-46s|--------|----------|----------|\n",
           "----------------------------------------------");
}

#define BENCH(lbl, n, body) do { \
    int64_t _t0 = esp_timer_get_time(); \
    for (int _i = 0; _i < (n); _i++) { body; } \
    int64_t _t1 = esp_timer_get_time(); \
    printf("  "); bprint((lbl), _t1 - _t0, (n)); \
} while (0)

void run_benchmarks(void)
{
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  SECTION 2 — Benchmarks (ESP32-S3 @ 240 MHz, float32)        ║\n");
    printf("║  Rows below are formatted as markdown table rows.            ║\n");
    printf("║  Copy each >>> block into the matching results/*.md file.    ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    /* ── shared data ──────────────────────────────────────────────── */
    numx_real_t a64[64], b64[64], r;
    for (int i = 0; i < 64; i++) { a64[i] = (numx_real_t)i; b64[i] = (float)(63 - i); }

    /* ══════════════════════════════════════════════════════════════ */
    printf("\n[linalg]\n");

    bench_section("linalg/vec_dot.md");
    BENCH("vec_dot n=64", BENCH_N_BIG, numx_vec_dot(a64, b64, 64, &r));

    bench_section("linalg/vec_norm.md");
    BENCH("vec_norm L2 n=64", BENCH_N_BIG, numx_vec_norm(a64, 64, NUMX_NORM_L2, &r));
    BENCH("vec_norm L1 n=64", BENCH_N_BIG, numx_vec_norm(a64, 64, NUMX_NORM_L1, &r));

    bench_section("linalg/vec_cross3.md");
    numx_real_t ax3[3] = {1, 2, 3}, bx3[3] = {4, 5, 6}, cx[3];
    BENCH("vec_cross3", BENCH_N_BIG, numx_vec_cross3(ax3, bx3, cx));

    bench_section("linalg/mat_mul.md");
    numx_real_t A4[16], B4[16], C4[16];
    for (int i = 0; i < 16; i++) { A4[i] = (numx_real_t)i; B4[i] = (float)(15 - i); }
    BENCH("mat_mul 4x4", BENCH_N_BIG, numx_mat_mul(A4, 4, 4, B4, 4, 4, C4));
    numx_real_t A8[64], B8[64], C8[64];
    for (int i = 0; i < 64; i++) { A8[i] = (numx_real_t)i * (numx_real_t)0.1; B8[i] = (float)(63 - i) * (numx_real_t)0.1; }
    BENCH("mat_mul 8x8", BENCH_N_TINY, numx_mat_mul(A8, 8, 8, B8, 8, 8, C8));

    /* mat_transpose — no dedicated .md file, included for completeness */
    printf("\n  >>> linalg/mat_mul.md (mat_transpose extras)\n");
    printf("  | %-44s | N      | Total us | ns/call  |\n", "Function");
    printf("  |%-46s|--------|----------|----------|\n", "----------------------------------------------");
    numx_real_t AT[64];
    for (int i = 0; i < 64; i++) A8[i] = (numx_real_t)i;
    BENCH("mat_transpose 8x8",    BENCH_N_BIG, numx_mat_transpose(A8, 8, 8, AT));
    BENCH("mat_transpose_sq 8x8", BENCH_N_BIG, numx_mat_transpose_sq(A8, 8));

    bench_section("linalg/mat_det.md");
    numx_real_t Md[16] = {4, 3, 2, 1, 3, 4, 3, 2, 2, 3, 4, 3, 1, 2, 3, 4};
    BENCH("mat_det 4x4", BENCH_N_BIG, numx_mat_det(Md, 4, &r));

    bench_section("linalg/lu_solve.md");
    numx_real_t Alu[16] = {2, 1, 1, 0, 4, 3, 3, 1, 8, 7, 9, 5, 6, 7, 9, 8};
    numx_real_t LU[16], bv[4] = {1, 2, 4, 5}, xv[4];
    numx_idx_t  piv[4];
    numx_lu_decompose(Alu, 4, LU, piv);
    BENCH("lu_decompose 4x4",      BENCH_N_BIG, numx_lu_decompose(Alu, 4, LU, piv));
    BENCH("lu_solve 4x4 (factored)", BENCH_N_BIG, numx_lu_solve(LU, piv, 4, bv, xv));

    /* ══════════════════════════════════════════════════════════════ */
    printf("\n[stats]\n");

    bench_section("stats/stats.md");
    numx_real_t data[128];
    for (int i = 0; i < 128; i++) data[i] = (float)(i + 1) * (numx_real_t)0.5;
    BENCH("stats_mean n=128",            BENCH_N_BIG,   numx_stats_mean(data, 128, &r));
    BENCH("stats_variance pop n=128",    BENCH_N_MED,   numx_stats_variance(data, 128, NUMX_VAR_POPULATION, &r));
    BENCH("stats_variance sample n=128", BENCH_N_MED,   numx_stats_variance(data, 128, NUMX_VAR_SAMPLE, &r));
    BENCH("stats_median n=128",          BENCH_N_SMALL, numx_stats_median(data, 128, &r));
    BENCH("stats_percentile p50 n=128",  BENCH_N_SMALL, numx_stats_percentile(data, 128, (numx_real_t)50.0, &r));
    BENCH("stats_percentile p95 n=128",  BENCH_N_SMALL, numx_stats_percentile(data, 128, (numx_real_t)95.0, &r));

    /* ══════════════════════════════════════════════════════════════ */
    printf("\n[roots]\n");

    bench_section("roots/roots.md");
    BENCH("root_bisect x^2-4 tol=1e-5", BENCH_N_SMALL, numx_root_bisect(f_quad, 0, 5, (numx_real_t)1e-5, &r));
    BENCH("root_newton x^2-4 tol=1e-5", BENCH_N_SMALL, numx_root_newton(f_quad, df_quad, 3, (numx_real_t)1e-5, &r));
    BENCH("root_brent  x^2-4 tol=1e-5", BENCH_N_SMALL, numx_root_brent(f_quad, 0, 5, (numx_real_t)1e-5, &r));
    BENCH("root_brent  x^3-x tol=1e-5", BENCH_N_SMALL, numx_root_brent(f_cubic, (numx_real_t)0.5, (numx_real_t)1.5, (numx_real_t)1e-5, &r));

    /* ══════════════════════════════════════════════════════════════ */
    printf("\n[integrate]\n");

    bench_section("integrate/integrate.md");
    BENCH("integrate_trap n=100",     BENCH_N_MED,   numx_integrate_trap(f_xsq, 0, 1, 100, &r));
    BENCH("integrate_trap n=1000",    BENCH_N_SMALL, numx_integrate_trap(f_xsq, 0, 1, 1000, &r));
    BENCH("integrate_simpson n=100",  BENCH_N_MED,   numx_integrate_simpson(f_xsq, 0, 1, 100, &r));
    BENCH("integrate_gauss npts=2",   BENCH_N_MED,   numx_integrate_gauss(f_xsq, 0, 1, 2, &r));
    BENCH("integrate_gauss npts=4",   BENCH_N_MED,   numx_integrate_gauss(f_xsq, 0, 1, 4, &r));
    BENCH("integrate_gauss npts=8",   BENCH_N_MED,   numx_integrate_gauss(f_xsq, 0, 1, 8, &r));

    /* ══════════════════════════════════════════════════════════════ */
    printf("\n[differentiate]\n");

    bench_section("differentiate/differentiate.md");
    BENCH("diff_forward  h=1e-3",   BENCH_N_BIG, numx_diff_forward(f_x3, (numx_real_t)2.0, (numx_real_t)1e-3, &r));
    BENCH("diff_central  h=1e-3",   BENCH_N_BIG, numx_diff_central(f_x3, (numx_real_t)2.0, (numx_real_t)1e-3, &r));
    BENCH("diff_richardson h=1e-3", BENCH_N_BIG, numx_diff_richardson(f_x3, (numx_real_t)2.0, (numx_real_t)1e-3, &r));

    /* ══════════════════════════════════════════════════════════════ */
    printf("\n[interpolate]\n");

    bench_section("interpolate/interpolate.md");
    numx_real_t xsi[16], ysi[16], mi[16];
    for (int i = 0; i < 16; i++) { xsi[i] = (numx_real_t)i * ((numx_real_t)3.14159 / (numx_real_t)15.0); ysi[i] = xsi[i] * xsi[i]; }
    BENCH("interp_linear n=16",             BENCH_N_MED,   numx_interp_linear(xsi, ysi, 16, (numx_real_t)1.5, &r));
    BENCH("interp_spline_cubic n=16",       BENCH_N_MED,   numx_interp_spline_cubic(xsi, ysi, 16, (numx_real_t)1.5, &r));
    numx_interp_spline_precompute(xsi, ysi, 16, mi);
    BENCH("interp_spline_eval (precomp)",   BENCH_N_MED,   numx_interp_spline_eval(xsi, ysi, mi, 16, (numx_real_t)1.5, &r));
    BENCH("interp_spline_precompute n=16",  BENCH_N_MED,   numx_interp_spline_precompute(xsi, ysi, 16, mi));
    BENCH("interp_chebyshev n=8",           BENCH_N_MED,   numx_interp_chebyshev(f_sq, 8, 0, (numx_real_t)3.14, (numx_real_t)1.5, &r));
    BENCH("interp_chebyshev n=16",          BENCH_N_SMALL, numx_interp_chebyshev(f_sq, 16, 0, (numx_real_t)3.14, (numx_real_t)1.5, &r));

    /* ══════════════════════════════════════════════════════════════ */
    printf("\n[poly]\n");

    bench_section("poly/poly.md");
    numx_real_t cp8[9] = {1, 0, 0, 0, -3, 0, 0, 0, 2}, cp3[4] = {1, -6, 11, -6};
    numx_real_t pro[3]; numx_size_t pnr;
    BENCH("poly_eval degree=8",           BENCH_N_BIG,  numx_poly_eval(cp8, 8, (numx_real_t)1.5, &r));
    BENCH("poly_eval degree=3",           BENCH_N_BIG,  numx_poly_eval(cp3, 3, (numx_real_t)2.5, &r));
    BENCH("poly_roots degree=3 tol=1e-4", BENCH_N_TINY, numx_poly_roots(cp3, 3, pro, &pnr, (numx_real_t)1e-4));

    /* ══════════════════════════════════════════════════════════════ */
    printf("\n[ode]\n");

    bench_section("ode/ode.md");
    numx_real_t yo[1] = {(numx_real_t)1.0}, yho[2] = {(numx_real_t)1.0, (numx_real_t)0.0}, yor[1], yor2[2];
    BENCH("ode_rk4  decay   n=1 100-steps",    BENCH_N_SMALL, numx_ode_rk4(ode_decay, 0, yo,  1, (numx_real_t)0.01, 100, yo));
    BENCH("ode_rk4  harmonic n=2 100-steps",   BENCH_N_SMALL, numx_ode_rk4(ode_harm,  0, yho, 2, (numx_real_t)0.01, 100, yho));
    BENCH("ode_rk45 decay   n=1 tol=1e-4",     BENCH_N_SMALL, numx_ode_rk45(ode_decay, 0, (numx_real_t)1.0, yo,  1, (numx_real_t)1e-4, yor));
    BENCH("ode_rk45 harmonic n=2 tol=1e-4",    BENCH_N_SMALL, numx_ode_rk45(ode_harm,  0, (numx_real_t)1.0, yho, 2, (numx_real_t)1e-4, yor2));
}
