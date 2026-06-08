#include <stdio.h>
#include "bench_common.h"
#include "math_helpers.h"
#include "numx/linalg.h"
#include "numx/stats.h"
#include "numx/roots.h"
#include "numx/integrate.h"
#include "numx/differentiate.h"
#include "numx/interpolate.h"
#include "numx/poly.h"
#include "numx/ode.h"

void bench_phase1(void)
{
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  PHASE 1 — linalg / stats / roots / integrate /             ║\n");
    printf("║            differentiate / interpolate / poly / ode          ║\n");
    printf("║  ESP32-S3 @ 240 MHz · float32                               ║\n");
    printf("║  Copy each >>> block into the matching results/*.md file.   ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    /* ── shared data ─────────────────────────────────────────────────── */
    numx_real_t a64[64], b64[64], r;
    for (int i = 0; i < 64; i++) {
        a64[i] = (numx_real_t)i;
        b64[i] = (numx_real_t)(63 - i);
    }

    /* ══════════════════════════════════════════════════════════════════ */
    printf("\n[linalg]\n");

    bench_section("linalg/vec_dot.md");
    BENCH("vec_dot n=64", BENCH_N_BIG,
          numx_vec_dot(a64, b64, 64, &r));

    bench_section("linalg/vec_norm.md");
    BENCH("vec_norm L2 n=64", BENCH_N_BIG,
          numx_vec_norm(a64, 64, NUMX_NORM_L2, &r));
    BENCH("vec_norm L1 n=64", BENCH_N_BIG,
          numx_vec_norm(a64, 64, NUMX_NORM_L1, &r));

    bench_section("linalg/vec_cross3.md");
    numx_real_t ax3[3] = {1.0f, 2.0f, 3.0f};
    numx_real_t bx3[3] = {4.0f, 5.0f, 6.0f};
    numx_real_t cx3[3];
    BENCH("vec_cross3", BENCH_N_BIG,
          numx_vec_cross3(ax3, bx3, cx3));

    bench_section("linalg/mat_mul.md");
    numx_real_t A4[16], B4[16], C4[16];
    for (int i = 0; i < 16; i++) { A4[i] = (numx_real_t)i; B4[i] = (numx_real_t)(15 - i); }
    BENCH("mat_mul 4x4", BENCH_N_BIG,
          numx_mat_mul(A4, 4, 4, B4, 4, 4, C4));
    numx_real_t A8[64], B8[64], C8[64];
    for (int i = 0; i < 64; i++) { A8[i] = (numx_real_t)i * 0.1f; B8[i] = (numx_real_t)(63 - i) * 0.1f; }
    BENCH("mat_mul 8x8", BENCH_N_TINY,
          numx_mat_mul(A8, 8, 8, B8, 8, 8, C8));

    bench_section("linalg/mat_transpose.md");
    numx_real_t AT8[64];
    for (int i = 0; i < 64; i++) A8[i] = (numx_real_t)i;
    BENCH("mat_transpose 8x8",    BENCH_N_BIG, numx_mat_transpose(A8, 8, 8, AT8));
    BENCH("mat_transpose_sq 8x8", BENCH_N_BIG, numx_mat_transpose_sq(A8, 8));

    bench_section("linalg/mat_det.md");
    numx_real_t Md[16] = {4, 3, 2, 1, 3, 4, 3, 2, 2, 3, 4, 3, 1, 2, 3, 4};
    BENCH("mat_det 4x4", BENCH_N_BIG,
          numx_mat_det(Md, 4, &r));

    bench_section("linalg/lu_solve.md");
    numx_real_t Alu[16] = {2, 1, 1, 0, 4, 3, 3, 1, 8, 7, 9, 5, 6, 7, 9, 8};
    numx_real_t LU[16], bv[4] = {1, 2, 4, 5}, xv[4];
    numx_idx_t  piv[4];
    numx_lu_decompose(Alu, 4, LU, piv);
    BENCH("lu_decompose 4x4",        BENCH_N_BIG, numx_lu_decompose(Alu, 4, LU, piv));
    BENCH("lu_solve 4x4 (factored)", BENCH_N_BIG, numx_lu_solve(LU, piv, 4, bv, xv));

    /* ══════════════════════════════════════════════════════════════════ */
    printf("\n[stats]\n");

    bench_section("stats/stats.md");
    numx_real_t data[128];
    for (int i = 0; i < 128; i++) data[i] = (numx_real_t)(i + 1) * 0.5f;
    BENCH("stats_mean n=128",            BENCH_N_BIG,   numx_stats_mean(data, 128, &r));
    BENCH("stats_variance pop n=128",    BENCH_N_MED,   numx_stats_variance(data, 128, NUMX_VAR_POPULATION, &r));
    BENCH("stats_variance sample n=128", BENCH_N_MED,   numx_stats_variance(data, 128, NUMX_VAR_SAMPLE, &r));
    BENCH("stats_median n=128",          BENCH_N_SMALL, numx_stats_median(data, 128, &r));
    BENCH("stats_percentile p50 n=128",  BENCH_N_SMALL, numx_stats_percentile(data, 128, 50.0f, &r));
    BENCH("stats_percentile p95 n=128",  BENCH_N_SMALL, numx_stats_percentile(data, 128, 95.0f, &r));

    /* ══════════════════════════════════════════════════════════════════ */
    printf("\n[roots]\n");

    bench_section("roots/roots.md");
    BENCH("root_bisect x^2-4 tol=1e-5", BENCH_N_SMALL,
          numx_root_bisect(f_quad, 0.0f, 5.0f, 1e-5f, &r));
    BENCH("root_newton x^2-4 tol=1e-5", BENCH_N_SMALL,
          numx_root_newton(f_quad, df_quad, 3.0f, 1e-5f, &r));
    BENCH("root_brent  x^2-4 tol=1e-5", BENCH_N_SMALL,
          numx_root_brent(f_quad, 0.0f, 5.0f, 1e-5f, &r));
    BENCH("root_brent  x^3-x tol=1e-5", BENCH_N_SMALL,
          numx_root_brent(f_cubic, 0.5f, 1.5f, 1e-5f, &r));

    /* ══════════════════════════════════════════════════════════════════ */
    printf("\n[integrate]\n");

    bench_section("integrate/integrate.md");
    BENCH("integrate_trap n=100",    BENCH_N_MED,   numx_integrate_trap(f_xsq, 0.0f, 1.0f, 100, &r));
    BENCH("integrate_trap n=1000",   BENCH_N_SMALL, numx_integrate_trap(f_xsq, 0.0f, 1.0f, 1000, &r));
    BENCH("integrate_simpson n=100", BENCH_N_MED,   numx_integrate_simpson(f_xsq, 0.0f, 1.0f, 100, &r));
    BENCH("integrate_gauss npts=2",  BENCH_N_MED,   numx_integrate_gauss(f_xsq, 0.0f, 1.0f, 2, &r));
    BENCH("integrate_gauss npts=4",  BENCH_N_MED,   numx_integrate_gauss(f_xsq, 0.0f, 1.0f, 4, &r));
    BENCH("integrate_gauss npts=8",  BENCH_N_MED,   numx_integrate_gauss(f_xsq, 0.0f, 1.0f, 8, &r));

    /* ══════════════════════════════════════════════════════════════════ */
    printf("\n[differentiate]\n");

    bench_section("differentiate/differentiate.md");
    BENCH("diff_forward  h=1e-3",   BENCH_N_BIG, numx_diff_forward(f_x3,    2.0f, 1e-3f, &r));
    BENCH("diff_central  h=1e-3",   BENCH_N_BIG, numx_diff_central(f_x3,    2.0f, 1e-3f, &r));
    BENCH("diff_richardson h=1e-3", BENCH_N_BIG, numx_diff_richardson(f_x3, 2.0f, 1e-3f, &r));

    /* ══════════════════════════════════════════════════════════════════ */
    printf("\n[interpolate]\n");

    bench_section("interpolate/interpolate.md");
    numx_real_t xsi[16], ysi[16], mi[16];
    for (int i = 0; i < 16; i++) {
        xsi[i] = (numx_real_t)i * (3.14159f / 15.0f);
        ysi[i] = xsi[i] * xsi[i];
    }
    BENCH("interp_linear n=16",            BENCH_N_MED,
          numx_interp_linear(xsi, ysi, 16, 1.5f, &r));
    BENCH("interp_spline_cubic n=16",      BENCH_N_MED,
          numx_interp_spline_cubic(xsi, ysi, 16, 1.5f, &r));
    numx_interp_spline_precompute(xsi, ysi, 16, mi);
    BENCH("interp_spline_eval (precomp)",  BENCH_N_MED,
          numx_interp_spline_eval(xsi, ysi, mi, 16, 1.5f, &r));
    BENCH("interp_spline_precompute n=16", BENCH_N_MED,
          numx_interp_spline_precompute(xsi, ysi, 16, mi));
    BENCH("interp_chebyshev n=8",          BENCH_N_MED,
          numx_interp_chebyshev(f_sq, 8, 0.0f, 3.14f, 1.5f, &r));
    BENCH("interp_chebyshev n=16",         BENCH_N_SMALL,
          numx_interp_chebyshev(f_sq, 16, 0.0f, 3.14f, 1.5f, &r));

    /* ══════════════════════════════════════════════════════════════════ */
    printf("\n[poly]\n");

    bench_section("poly/poly.md");
    numx_real_t cp8[9] = {1, 0, 0, 0, -3, 0, 0, 0, 2};
    numx_real_t cp3[4] = {1, -6, 11, -6};
    numx_real_t pro[3];
    numx_size_t pnr;
    BENCH("poly_eval degree=8",           BENCH_N_BIG,
          numx_poly_eval(cp8, 8, 1.5f, &r));
    BENCH("poly_eval degree=3",           BENCH_N_BIG,
          numx_poly_eval(cp3, 3, 2.5f, &r));
    BENCH("poly_roots degree=3 tol=1e-4", BENCH_N_TINY,
          numx_poly_roots(cp3, 3, pro, &pnr, 1e-4f));

    /* ══════════════════════════════════════════════════════════════════ */
    printf("\n[ode]\n");

    bench_section("ode/ode.md");
    numx_real_t yo[1]  = {1.0f};
    numx_real_t yho[2] = {1.0f, 0.0f};
    numx_real_t yor[1], yor2[2];
    BENCH("ode_rk4  decay   n=1 100-steps",  BENCH_N_SMALL,
          numx_ode_rk4(ode_decay, 0.0f, yo,  1, 0.01f, 100, yo));
    BENCH("ode_rk4  harmonic n=2 100-steps", BENCH_N_SMALL,
          numx_ode_rk4(ode_harm,  0.0f, yho, 2, 0.01f, 100, yho));
    BENCH("ode_rk45 decay   n=1 tol=1e-4",   BENCH_N_SMALL,
          numx_ode_rk45(ode_decay, 0.0f, 1.0f, yo,  1, 1e-4f, yor));
    BENCH("ode_rk45 harmonic n=2 tol=1e-4",  BENCH_N_SMALL,
          numx_ode_rk45(ode_harm,  0.0f, 1.0f, yho, 2, 1e-4f, yor2));
}
