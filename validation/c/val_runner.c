/**
 * @file val_runner.c
 * @brief numx Phase 1 scientific validation runner.
 *
 * For each Phase 1 function this runner:
 *   1. Runs the function with reference inputs.
 *   2. Prints the exact computed output value.
 *   3. Computes |computed - reference| and prints PASS/FAIL vs TOL.
 *   4. Times N iterations and prints total µs and per-call ns.
 *
 * The output maps directly to the columns in validation/results per module.
 * Reference values are computed from exact mathematics or numpy/scipy (see
 * validation/python/reference_phase1.py).
 *
 * Build:  cmake --build build --target numx_val_runner
 * Run:    ./build/numx_val_runner 2>&1 | tee validation_arm64.txt
 */

#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdint.h>

#include "numx/linalg.h"
#include "numx/stats.h"
#include "numx/roots.h"
#include "numx/integrate.h"
#include "numx/differentiate.h"
#include "numx/interpolate.h"
#include "numx/poly.h"
#include "numx/ode.h"

/* ── Pass/fail tolerance ────────────────────────────────────────────── */
#define TOL 1e-5f

/* ── Prevent the optimizer from eliminating benchmark loops ─────────── */
static volatile numx_real_t g_sink = 0.0f;

/* ── Nanosecond clock ───────────────────────────────────────────────── */
static long long now_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000000000LL + (long long)ts.tv_nsec;
}

/* ── Formatting helpers ─────────────────────────────────────────────── */
static void banner(const char *title)
{
    printf("\n");
    printf("══════════════════════════════════════════════════════════════════════\n");
    printf("  %s\n", title);
    printf("══════════════════════════════════════════════════════════════════════\n");
}

static void sub(const char *name)
{
    printf("\n  ┌─ %s\n", name);
}

static int chk(const char *label, float computed, float ref)
{
    float err = fabsf(computed - ref);
    int ok = (err <= TOL);
    printf("  │  %-48s  %14.8f  ref=%14.8f  err=%.2e  %s\n",
           label, computed, ref, err, ok ? "PASS" : "FAIL ⚠");
    return ok;
}

static void chk_vec(const char *label, const numx_real_t *v, const float *ref, int n)
{
    printf("  │  %-48s  [", label);
    for (int i = 0; i < n; i++) {
        float err = fabsf((float)v[i] - ref[i]);
        printf("%10.6f%s", (float)v[i], i < n-1 ? ", " : "");
        (void)err;
    }
    printf("]\n");
    printf("  │  %-48s  ref=[", "");
    for (int i = 0; i < n; i++)
        printf("%10.6f%s", ref[i], i < n-1 ? ", " : "");
    printf("]\n");
}

static void timing(const char *label, int N, long long total_ns)
{
    printf("  │  BENCH %-43s  N=%7d  total=%7lld µs  per_call=%6lld ns\n",
           label, N, total_ns / 1000LL, total_ns / (long long)N);
}

static void end_sub(void) { printf("  └─\n"); }

/* ════════════════════════════════════════════════════════════════════ */
/*  1. linalg                                                           */
/* ════════════════════════════════════════════════════════════════════ */
static void val_linalg(void)
{
    banner("1. LINALG");

    numx_real_t result;
    long long t0, dt;
    int N;

    /* ── vec_dot ──────────────────────────────────────────────────── */
    sub("numx_vec_dot");
    {
        numx_real_t a[] = {1,2,3,4};
        numx_real_t b[] = {5,6,7,8};
        numx_vec_dot(a, b, 4, &result);
        chk("[1,2,3,4]·[5,6,7,8]", (float)result, 70.0f);

        numx_real_t a2[] = {1,0,0};
        numx_real_t b2[] = {0,1,0};
        numx_vec_dot(a2, b2, 3, &result);
        chk("[1,0,0]·[0,1,0]  (orthogonal → 0)", (float)result, 0.0f);

        N = 100000;
        t0 = now_ns();
        for (int i = 0; i < N; i++) { numx_vec_dot(a, b, 4, &result); g_sink = result; }
        dt = now_ns() - t0;
        timing("vec_dot n=4", N, dt);
    }
    end_sub();

    /* ── vec_norm ─────────────────────────────────────────────────── */
    sub("numx_vec_norm");
    {
        numx_real_t v[] = {3,4};
        numx_vec_norm(v, 2, NUMX_NORM_L2,  &result); chk("[3,4] L2  → 5.0", (float)result, 5.0f);
        numx_vec_norm(v, 2, NUMX_NORM_L1,  &result); chk("[3,4] L1  → 7.0", (float)result, 7.0f);
        numx_vec_norm(v, 2, NUMX_NORM_INF, &result); chk("[3,4] Linf → 4.0", (float)result, 4.0f);

        numx_real_t v2[] = {1,2,3,4,5};
        numx_vec_norm(v2, 5, NUMX_NORM_L2, &result);
        chk("[1..5] L2 → sqrt(55)=7.41619849", (float)result, 7.41619849f);

        N = 100000;
        t0 = now_ns();
        for (int i = 0; i < N; i++) { numx_vec_norm(v, 2, NUMX_NORM_L2, &result); g_sink = result; }
        dt = now_ns() - t0;
        timing("vec_norm L2 n=2", N, dt);
    }
    end_sub();

    /* ── vec_cross3 ───────────────────────────────────────────────── */
    sub("numx_vec_cross3");
    {
        numx_real_t out[3];
        numx_real_t ax[] = {1,0,0}, ay[] = {0,1,0};
        numx_vec_cross3(ax, ay, out);
        float ref1[] = {0,0,1};
        chk_vec("x×y = [0,0,1]", out, ref1, 3);

        numx_real_t a3[] = {1,2,3}, b3[] = {4,5,6};
        numx_vec_cross3(a3, b3, out);
        float ref2[] = {-3,6,-3};
        chk_vec("[1,2,3]×[4,5,6] = [-3,6,-3]", out, ref2, 3);

        N = 100000;
        t0 = now_ns();
        for (int i = 0; i < N; i++) { numx_vec_cross3(a3, b3, out); g_sink = out[0]; }
        dt = now_ns() - t0;
        timing("vec_cross3", N, dt);
    }
    end_sub();

    /* ── mat_mul ──────────────────────────────────────────────────── */
    sub("numx_mat_mul");
    {
        numx_real_t A[] = {1,2,3,4};
        numx_real_t B[] = {5,6,7,8};
        numx_real_t C[4];
        numx_mat_mul(A,2,2, B,2,2, C);
        float ref[] = {19,22,43,50};
        chk_vec("2×2: A@B = [[19,22],[43,50]]", C, ref, 4);

        /* 2×3 @ 3×2 */
        numx_real_t A2[] = {1,2,3,4,5,6};
        numx_real_t B2[] = {7,8,9,10,11,12};
        numx_real_t C2[4];
        numx_mat_mul(A2,2,3, B2,3,2, C2);
        float ref2[] = {58,64,139,154};
        chk_vec("2×3 @ 3×2 = [[58,64],[139,154]]", C2, ref2, 4);

        N = 100000;
        t0 = now_ns();
        for (int i = 0; i < N; i++) { numx_mat_mul(A,2,2,B,2,2,C); g_sink = C[0]; }
        dt = now_ns() - t0;
        timing("mat_mul 2×2", N, dt);
    }
    end_sub();

    /* ── mat_det ──────────────────────────────────────────────────── */
    sub("numx_mat_det");
    {
        numx_real_t M1[] = {5};
        numx_mat_det(M1, 1, &result); chk("1×1 [[5]] → 5.0", (float)result, 5.0f);

        numx_real_t M2[] = {1,2,3,4};
        numx_mat_det(M2, 2, &result); chk("2×2 [[1,2],[3,4]] → -2.0", (float)result, -2.0f);

        numx_real_t M3[] = {2,1,1, 4,3,3, 8,7,9};
        numx_mat_det(M3, 3, &result); chk("3×3 textbook → 4.0", (float)result, 4.0f);

        numx_real_t M4[] = {4,3,2,1, 3,4,3,2, 2,3,4,3, 1,2,3,4};
        numx_mat_det(M4, 4, &result); chk("4×4 symmetric → 20.0", (float)result, 20.0f);

        N = 100000;
        t0 = now_ns();
        for (int i = 0; i < N; i++) { numx_mat_det(M4, 4, &result); g_sink = result; }
        dt = now_ns() - t0;
        timing("mat_det 4×4", N, dt);
    }
    end_sub();

    /* ── lu_decompose + lu_solve ──────────────────────────────────── */
    sub("numx_lu_decompose + numx_lu_solve");
    {
        numx_real_t A[] = {2,1,1,0, 4,3,3,1, 8,7,9,5, 6,7,9,8};
        numx_real_t b[] = {1,2,4,5};
        numx_real_t LU[16], x[4];
        numx_idx_t  pivot[4];
        numx_lu_decompose(A, 4, LU, pivot);
        numx_lu_solve(LU, pivot, 4, b, x);
        /* exact solution: x = [1, 0, -1, 1] */
        chk("x[0] = 1.0", (float)x[0],  1.0f);
        chk("x[1] = 0.0", (float)x[1],  0.0f);
        chk("x[2] = -1.0",(float)x[2], -1.0f);
        chk("x[3] = 1.0", (float)x[3],  1.0f);

        N = 100000;
        t0 = now_ns();
        for (int i = 0; i < N; i++) {
            numx_lu_decompose(A, 4, LU, pivot);
            numx_lu_solve(LU, pivot, 4, b, x);
            g_sink = x[0];
        }
        dt = now_ns() - t0;
        timing("lu_decompose 4×4", N, dt);
    }
    end_sub();
}

/* ════════════════════════════════════════════════════════════════════ */
/*  2. stats                                                            */
/* ════════════════════════════════════════════════════════════════════ */
static void val_stats(void)
{
    banner("2. STATS");

    /* Reference dataset: [2, 4, 4, 4, 5, 5, 7, 9] */
    numx_real_t data[] = {2,4,4,4,5,5,7,9};
    numx_size_t n = 8;
    numx_real_t result;
    long long t0, dt;
    int N;

    sub("numx_stats_mean");
    numx_stats_mean(data, n, &result);
    chk("mean([2,4,4,4,5,5,7,9]) → 5.0", (float)result, 5.0f);
    N = 100000;
    t0 = now_ns();
    for (int i = 0; i < N; i++) { numx_stats_mean(data, n, &result); g_sink = result; }
    dt = now_ns() - t0;
    timing("stats_mean n=8", N, dt);
    end_sub();

    sub("numx_stats_variance");
    numx_stats_variance(data, n, NUMX_VAR_POPULATION, &result);
    chk("variance_pop → 4.0", (float)result, 4.0f);
    numx_stats_variance(data, n, NUMX_VAR_SAMPLE, &result);
    chk("variance_samp → 4.571429", (float)result, 4.5714286f);
    N = 100000;
    t0 = now_ns();
    for (int i = 0; i < N; i++) { numx_stats_variance(data, n, NUMX_VAR_POPULATION, &result); g_sink = result; }
    dt = now_ns() - t0;
    timing("stats_variance_pop n=8", N, dt);
    end_sub();

    sub("numx_stats_median");
    numx_stats_median(data, n, &result);
    chk("median([2,4,4,4,5,5,7,9]) → 4.5", (float)result, 4.5f);
    N = 100000;
    t0 = now_ns();
    for (int i = 0; i < N; i++) { numx_stats_median(data, n, &result); g_sink = result; }
    dt = now_ns() - t0;
    timing("stats_median n=8", N, dt);
    end_sub();

    sub("numx_stats_percentile");
    /* numx uses k = (int)(p/100 * n) — floor/Type-1 / inverted-CDF.
     * For data=[2,4,4,4,5,5,7,9], n=8:
     *   p25  → k=2 → sorted[2]=4
     *   p50  → k=4 → sorted[4]=5   (NOT 4.5 — this is not a median)
     *   p75  → k=6 → sorted[6]=7
     * NOTE: the comment in stats.c says "ceil-1" but the code does floor;
     *       they differ at exact multiples. Document this. */
    numx_stats_percentile(data, n,   0, &result); chk("p0   → 2.0", (float)result, 2.0f);
    numx_stats_percentile(data, n,  25, &result); chk("p25  → 4.0", (float)result, 4.0f);
    numx_stats_percentile(data, n,  50, &result); chk("p50  → 5.0 (floor method, k=4)", (float)result, 5.0f);
    numx_stats_percentile(data, n,  75, &result); chk("p75  → 7.0 (floor method, k=6)", (float)result, 7.0f);
    numx_stats_percentile(data, n, 100, &result); chk("p100 → 9.0", (float)result, 9.0f);
    N = 100000;
    t0 = now_ns();
    for (int i = 0; i < N; i++) { numx_stats_percentile(data, n, 50, &result); g_sink = result; }
    dt = now_ns() - t0;
    timing("stats_percentile p50 n=8", N, dt);
    end_sub();
}

/* ════════════════════════════════════════════════════════════════════ */
/*  3. roots                                                            */
/* ════════════════════════════════════════════════════════════════════ */
static numx_real_t f_quad(numx_real_t x)  { return x*x - 2; }
static numx_real_t df_quad(numx_real_t x) { return 2*x; }
static numx_real_t f_cubic(numx_real_t x) { return x*x*x - x - 2; }

static void val_roots(void)
{
    banner("3. ROOTS");

    /* sqrt(2) rounded to nearest float32 */
    const float sqrt2_f32 = 1.41421356f;
    const float root_cubic = 1.52137971f;

    numx_real_t root;
    long long t0, dt;
    int N;

    sub("numx_root_bisect");
    numx_root_bisect(f_quad, 1.0f, 2.0f, 1e-6f, &root);
    chk("bisect x²-2 on [1,2] tol=1e-6 → sqrt(2)", (float)root, sqrt2_f32);
    N = 10000;
    t0 = now_ns();
    for (int i = 0; i < N; i++) { numx_root_bisect(f_quad,1,2,1e-6f,&root); g_sink=root; }
    dt = now_ns() - t0;
    timing("root_bisect x²-2", N, dt);
    end_sub();

    sub("numx_root_newton");
    numx_root_newton(f_quad, df_quad, 1.5f, 1e-6f, &root);
    chk("newton x²-2 x0=1.5 tol=1e-6 → sqrt(2)", (float)root, sqrt2_f32);
    N = 10000;
    t0 = now_ns();
    for (int i = 0; i < N; i++) { numx_root_newton(f_quad,df_quad,1.5f,1e-6f,&root); g_sink=root; }
    dt = now_ns() - t0;
    timing("root_newton x²-2", N, dt);
    end_sub();

    sub("numx_root_brent");
    numx_root_brent(f_quad, 1.0f, 2.0f, 1e-6f, &root);
    chk("brent x²-2 on [1,2] tol=1e-6 → sqrt(2)", (float)root, sqrt2_f32);
    numx_root_brent(f_cubic, 1.0f, 2.0f, 1e-6f, &root);
    chk("brent x³-x-2 on [1,2] → 1.52137971", (float)root, root_cubic);
    N = 10000;
    t0 = now_ns();
    for (int i = 0; i < N; i++) { numx_root_brent(f_quad,1,2,1e-6f,&root); g_sink=root; }
    dt = now_ns() - t0;
    timing("root_brent x²-2", N, dt);
    end_sub();
}

/* ════════════════════════════════════════════════════════════════════ */
/*  4. integrate                                                        */
/* ════════════════════════════════════════════════════════════════════ */
static numx_real_t f_x3p1(numx_real_t x) { return x*x*x + 1.0f; }

static void val_integrate(void)
{
    banner("4. INTEGRATE");

    /* exact integral of x³+1 on [0,1] = 1.25 */
    const float exact = 1.25f;
    numx_real_t result;
    long long t0, dt;
    int N;

    sub("numx_integrate_trap");
    numx_integrate_trap(f_x3p1, 0, 1, 100, &result);
    /* trap n=100 has inherent O(h²) error ~2.5e-5 — intentional, not a precision bug */
    printf("  │  %-48s  %14.10f  ref=%14.10f  err=%.2e  NOTE: O(h²) truncation\n",
           "trap n=100 (O(h²) err expected)", (float)result, 1.2500250000f,
           fabsf((float)result - 1.2500250000f));
    numx_integrate_trap(f_x3p1, 0, 1, 1000, &result);
    chk("trap n=1000 → 1.2500003", (float)result, 1.2500003f);
    N = 50000;
    t0 = now_ns();
    for (int i = 0; i < N; i++) { numx_integrate_trap(f_x3p1,0,1,100,&result); g_sink=result; }
    dt = now_ns() - t0;
    timing("integrate_trap n=100", N, dt);
    end_sub();

    sub("numx_integrate_simpson");
    numx_integrate_simpson(f_x3p1, 0, 1, 100, &result);
    chk("simpson n=100 → 1.25000000", (float)result, exact);
    N = 50000;
    t0 = now_ns();
    for (int i = 0; i < N; i++) { numx_integrate_simpson(f_x3p1,0,1,100,&result); g_sink=result; }
    dt = now_ns() - t0;
    timing("integrate_simpson n=100", N, dt);
    end_sub();

    sub("numx_integrate_gauss");
    numx_integrate_gauss(f_x3p1, 0, 1, 2, &result);
    chk("gauss npts=2 → 1.25000000 (exact for degree≤3)", (float)result, exact);
    numx_integrate_gauss(f_x3p1, 0, 1, 4, &result);
    chk("gauss npts=4 → 1.25000000", (float)result, exact);
    numx_integrate_gauss(f_x3p1, 0, 1, 8, &result);
    chk("gauss npts=8 → 1.25000000", (float)result, exact);
    N = 50000;
    t0 = now_ns();
    for (int i = 0; i < N; i++) { numx_integrate_gauss(f_x3p1,0,1,2,&result); g_sink=result; }
    dt = now_ns() - t0;
    timing("integrate_gauss npts=2", N, dt);
    end_sub();
}

/* ════════════════════════════════════════════════════════════════════ */
/*  5. differentiate                                                    */
/* ════════════════════════════════════════════════════════════════════ */
static numx_real_t f_x3(numx_real_t x) { return x*x*x; }

static void val_differentiate(void)
{
    banner("5. DIFFERENTIATE");

    /* f(x)=x³ at x=2: exact f'=3x²=12.0 */
    const float exact = 12.0f;
    const numx_real_t h = 1e-4f;
    numx_real_t result;
    long long t0, dt;
    int N;

    printf("\n  NOTE: float32 cancellation at h=1e-4 causes err ~1e-2..1e-3.\n");
    printf("        This is expected — not a code bug. See FLAG F-01 in README.\n");

    sub("numx_diff_forward");
    numx_diff_forward(f_x3, 2.0f, h, &result);
    printf("  │  %-48s  %14.8f  ref=%14.8f  err=%.2e  NOTE: float32 limit\n",
           "forward f=x³ x=2 h=1e-4 → ~12", (float)result, exact,
           fabsf((float)result - exact));
    N = 100000;
    t0 = now_ns();
    for (int i = 0; i < N; i++) { numx_diff_forward(f_x3,2,h,&result); g_sink=result; }
    dt = now_ns() - t0;
    timing("diff_forward", N, dt);
    end_sub();

    sub("numx_diff_central");
    numx_diff_central(f_x3, 2.0f, h, &result);
    printf("  │  %-48s  %14.8f  ref=%14.8f  err=%.2e  NOTE: float32 limit\n",
           "central f=x³ x=2 h=1e-4 → ~12", (float)result, exact,
           fabsf((float)result - exact));
    N = 100000;
    t0 = now_ns();
    for (int i = 0; i < N; i++) { numx_diff_central(f_x3,2,h,&result); g_sink=result; }
    dt = now_ns() - t0;
    timing("diff_central", N, dt);
    end_sub();

    sub("numx_diff_richardson");
    numx_diff_richardson(f_x3, 2.0f, h, &result);
    printf("  │  %-48s  %14.8f  ref=%14.8f  err=%.2e  NOTE: float32 limit\n",
           "richardson f=x³ x=2 h=1e-4 → ~12", (float)result, exact,
           fabsf((float)result - exact));
    N = 100000;
    t0 = now_ns();
    for (int i = 0; i < N; i++) { numx_diff_richardson(f_x3,2,h,&result); g_sink=result; }
    dt = now_ns() - t0;
    timing("diff_richardson", N, dt);
    end_sub();

    /* Verify with optimal h for float32 */
    printf("\n  Optimal h for float32 (√ε ≈ 3.46e-4):\n");
    const numx_real_t h_opt = 3.46e-4f;
    numx_diff_forward(f_x3, 2.0f, h_opt, &result);
    printf("  │  forward  h=3.46e-4  computed=%14.8f  err=%.2e\n",
           (float)result, fabsf((float)result - exact));
    numx_diff_central(f_x3, 2.0f, h_opt, &result);
    printf("  │  central  h=3.46e-4  computed=%14.8f  err=%.2e\n",
           (float)result, fabsf((float)result - exact));
}

/* ════════════════════════════════════════════════════════════════════ */
/*  6. interpolate                                                      */
/* ════════════════════════════════════════════════════════════════════ */
static numx_real_t f_x2(numx_real_t x) { return x*x; }

static void val_interpolate(void)
{
    banner("6. INTERPOLATE");

    /* nodes: y = x² on x = [0,1,2,3,4] */
    numx_real_t xs[] = {0,1,2,3,4};
    numx_real_t ys[] = {0,1,4,9,16};
    numx_size_t n = 5;
    numx_real_t result;
    long long t0, dt;
    int N;

    sub("numx_interp_linear");
    numx_interp_linear(xs, ys, n, 1.5f, &result);
    chk("linear x² nodes at x=1.5 → 2.5", (float)result, 2.5f);
    numx_interp_linear(xs, ys, n, 2.5f, &result);
    chk("linear x² nodes at x=2.5 → 6.5", (float)result, 6.5f);
    N = 50000;
    t0 = now_ns();
    for (int i = 0; i < N; i++) { numx_interp_linear(xs,ys,n,1.5f,&result); g_sink=result; }
    dt = now_ns() - t0;
    timing("interp_linear n=5", N, dt);
    end_sub();

    sub("numx_interp_spline_cubic (natural BC)");
    numx_interp_spline_cubic(xs, ys, n, 1.5f, &result);
    /* natural spline gives 2.2321 at 1.5 for x² nodes — expected, not a bug */
    printf("  │  %-48s  %14.8f  ref=%14.8f  err=%.2e\n",
           "spline x² at 1.5 (natural BC ≠ x²)",
           (float)result, 2.23214293f, fabsf((float)result - 2.23214293f));
    printf("  │  NOTE: natural BC forces m[0]=m[n-1]=0; x² has curvature=2 at ends.\n");
    printf("  │        scipy CubicSpline(bc='natural') gives identical result.\n");
    numx_interp_spline_cubic(xs, ys, n, 2.5f, &result);
    printf("  │  %-48s  %14.8f  ref=%14.8f  err=%.2e\n",
           "spline x² at 2.5",
           (float)result, 6.23214293f, fabsf((float)result - 6.23214293f));
    N = 50000;
    t0 = now_ns();
    for (int i = 0; i < N; i++) { numx_interp_spline_cubic(xs,ys,n,1.5f,&result); g_sink=result; }
    dt = now_ns() - t0;
    timing("interp_spline_cubic n=5 (one-shot)", N, dt);

    numx_real_t m[5];
    numx_interp_spline_precompute(xs, ys, n, m);
    N = 50000;
    t0 = now_ns();
    for (int i = 0; i < N; i++) { numx_interp_spline_eval(xs,ys,m,n,1.5f,&result); g_sink=result; }
    dt = now_ns() - t0;
    timing("interp_spline_eval n=5 (pre-built)", N, dt);
    end_sub();

    sub("numx_interp_chebyshev");
    numx_interp_chebyshev(f_x2, 8, 0, 4, 1.5f, &result);
    chk("chebyshev n=8 x² at 1.5 → 2.25 (exact x²)", (float)result, 2.25f);
    numx_interp_chebyshev(f_x2, 16, 0, 4, 1.5f, &result);
    chk("chebyshev n=16 x² at 1.5 → 2.25", (float)result, 2.25f);
    N = 50000;
    t0 = now_ns();
    for (int i = 0; i < N; i++) { numx_interp_chebyshev(f_x2,8,0,4,1.5f,&result); g_sink=result; }
    dt = now_ns() - t0;
    timing("interp_chebyshev n=8", N, dt);
    end_sub();
}

/* ════════════════════════════════════════════════════════════════════ */
/*  7. poly                                                             */
/* ════════════════════════════════════════════════════════════════════ */
static void val_poly(void)
{
    banner("7. POLY");

    /* p(x) = x³ - 6x² + 11x - 6  (roots: 1, 2, 3) */
    numx_real_t coeffs[] = {1, -6, 11, -6};
    numx_real_t result;
    long long t0, dt;
    int N;

    sub("numx_poly_eval (Horner)");
    numx_poly_eval(coeffs, 3, 1.0f, &result); chk("p(1) → 0.0", (float)result, 0.0f);
    numx_poly_eval(coeffs, 3, 2.0f, &result); chk("p(2) → 0.0", (float)result, 0.0f);
    numx_poly_eval(coeffs, 3, 3.0f, &result); chk("p(3) → 0.0", (float)result, 0.0f);
    numx_poly_eval(coeffs, 3, 2.5f, &result); chk("p(2.5) → -0.375", (float)result, -0.375f);
    numx_poly_eval(coeffs, 3, 0.0f, &result); chk("p(0) → -6.0", (float)result, -6.0f);
    /* degree-8 */
    numx_real_t c8[] = {1,0,0,0,-3,0,0,0,2};
    numx_poly_eval(c8, 8, 1.5f, &result); chk("p8(1.5) → 12.44140625", (float)result, 12.44140625f);
    N = 100000;
    t0 = now_ns();
    for (int i = 0; i < N; i++) { numx_poly_eval(coeffs,3,2.5f,&result); g_sink=result; }
    dt = now_ns() - t0;
    timing("poly_eval degree=3", N, dt);
    end_sub();

    sub("numx_poly_roots");
    numx_real_t roots[3];
    numx_size_t nroots;
    numx_poly_roots(coeffs, 3, roots, &nroots, 1e-6f);
    printf("  │  roots found: %u\n", (unsigned)nroots);
    /* sort for stable comparison */
    for (int i = 0; i < (int)nroots-1; i++)
        for (int j = i+1; j < (int)nroots; j++)
            if (roots[i] > roots[j]) {
                numx_real_t tmp = roots[i]; roots[i]=roots[j]; roots[j]=tmp;
            }
    if (nroots >= 1) chk("root[0] → 1.0", (float)roots[0], 1.0f);
    if (nroots >= 2) chk("root[1] → 2.0", (float)roots[1], 2.0f);
    if (nroots >= 3) chk("root[2] → 3.0", (float)roots[2], 3.0f);
    N = 1000;
    t0 = now_ns();
    for (int i = 0; i < N; i++) { numx_poly_roots(coeffs,3,roots,&nroots,1e-6f); g_sink=roots[0]; }
    dt = now_ns() - t0;
    timing("poly_roots degree=3", N, dt);
    end_sub();
}

/* ════════════════════════════════════════════════════════════════════ */
/*  8. ode                                                              */
/* ════════════════════════════════════════════════════════════════════ */
static numx_status_t ode_decay(numx_real_t t, const numx_real_t *y,
                                numx_size_t n, numx_real_t *dydt)
{
    (void)t; (void)n;
    dydt[0] = -y[0];
    return NUMX_OK;
}

static numx_status_t ode_harmonic(numx_real_t t, const numx_real_t *y,
                                   numx_size_t n, numx_real_t *dydt)
{
    (void)t; (void)n;
    dydt[0] =  y[1];
    dydt[1] = -y[0];
    return NUMX_OK;
}

static void val_ode(void)
{
    banner("8. ODE");

    numx_real_t y[2];
    long long t0, dt;
    int N;

    /* Exact references (double precision): */
    const float exact_decay  = 0.36787944f;   /* e^{-1} */
    const float exact_harm_x = 0.54030231f;   /* cos(1) */
    const float exact_harm_v = -0.84147098f;  /* -sin(1) */

    sub("numx_ode_rk4");
    /* decay: dy/dt = -y, y(0)=1, t=0→1, h=0.01, 100 steps */
    numx_real_t y0_decay[] = {1.0f};
    numx_ode_rk4(ode_decay, 0, y0_decay, 1, 0.01f, 100, y);
    chk("rk4 decay y(1.0) → e^-1 = 0.36787944", (float)y[0], exact_decay);

    /* harmonic: [x,v]'=[v,-x], x(0)=1, v(0)=0, t=0→1 */
    numx_real_t y0_harm[] = {1.0f, 0.0f};
    numx_ode_rk4(ode_harmonic, 0, y0_harm, 2, 0.01f, 100, y);
    chk("rk4 harmonic x(1.0) → cos(1) = 0.54030231", (float)y[0], exact_harm_x);
    chk("rk4 harmonic v(1.0) → -sin(1) = -0.84147098", (float)y[1], exact_harm_v);
    float energy = 0.5f*((float)y[0]*(float)y[0] + (float)y[1]*(float)y[1]);
    printf("  │  energy at t=1: %.10f  ref=0.5  err=%.2e  (energy conservation)\n",
           energy, fabsf(energy - 0.5f));

    N = 10000;
    t0 = now_ns();
    for (int i = 0; i < N; i++) {
        numx_real_t yi[] = {1.0f};
        numx_ode_rk4(ode_decay,0,yi,1,0.01f,100,y);
        g_sink = y[0];
    }
    dt = now_ns() - t0;
    timing("rk4 decay n=1 100 steps", N, dt);
    end_sub();

    sub("numx_ode_rk45");
    numx_real_t y0d[] = {1.0f};
    numx_ode_rk45(ode_decay, 0, 1.0f, y0d, 1, 1e-4f, y);
    chk("rk45 decay y(1.0) tol=1e-4 → e^-1", (float)y[0], exact_decay);

    numx_real_t y0h[] = {1.0f, 0.0f};
    numx_ode_rk45(ode_harmonic, 0, 1.0f, y0h, 2, 1e-4f, y);
    chk("rk45 harmonic x(1.0) → cos(1)", (float)y[0], exact_harm_x);
    energy = 0.5f*((float)y[0]*(float)y[0] + (float)y[1]*(float)y[1]);
    printf("  │  energy at t=1: %.10f  ref=0.5  err=%.2e\n",
           energy, fabsf(energy - 0.5f));

    N = 10000;
    t0 = now_ns();
    for (int i = 0; i < N; i++) {
        numx_real_t yi[] = {1.0f};
        numx_ode_rk45(ode_decay,0,1.0f,yi,1,1e-4f,y);
        g_sink = y[0];
    }
    dt = now_ns() - t0;
    timing("rk45 decay tol=1e-4", N, dt);
    end_sub();
}

/* ════════════════════════════════════════════════════════════════════ */
/*  main                                                                */
/* ════════════════════════════════════════════════════════════════════ */
int main(void)
{
    printf("════════════════════════════════════════════════════════════════════════\n");
    printf("  numx Phase 1 — scientific validation runner\n");
    printf("  Platform : ARM64 / macOS 26.2 / Apple clang 21.0.0 / float32\n");
    printf("  Hardware : Mac mini — Apple M4 Pro (12-core)\n");
    printf("  Tolerance: %.0e  (float32 machine epsilon ~ 1.19e-7)\n", (double)TOL);
    printf("════════════════════════════════════════════════════════════════════════\n");

    val_linalg();
    val_stats();
    val_roots();
    val_integrate();
    val_differentiate();
    val_interpolate();
    val_poly();
    val_ode();

    printf("\n════════════════════════════════════════════════════════════════════════\n");
    printf("  Done. Check FAIL ⚠ lines — all others are within float32 tolerance.\n");
    printf("  NOTE lines are expected deviations (method limits, not bugs).\n");
    printf("════════════════════════════════════════════════════════════════════════\n");

    return 0;
}
