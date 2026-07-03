/**
 * @file bench_ntt.c
 * @brief Benchmarks for the numx NTT module.
 *
 * Measures: forward NTT, inverse NTT, polymul (full pipeline),
 * poly_add, and poly_sub. All operations on n=256 arrays.
 * Uses POSIX clock_gettime(CLOCK_MONOTONIC) for sub-microsecond resolution.
 */

#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "numx/ntt.h"

#define BENCH_N 10000   /* iterations per benchmark */

static int64_t now_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (int64_t)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

static void bprint(const char *label, int64_t ns_total, int n)
{
    int64_t ns_per = ns_total / n;
    if (ns_per >= 1000)
        printf("  %-38s | %6d | %8.1f us | %7lld ns\n",
               label, n, (double)ns_total / 1000.0,
               (long long)ns_per);
    else
        printf("  %-38s | %6d | %8.3f us | %7lld ns\n",
               label, n, (double)ns_total / 1000.0,
               (long long)ns_per);
}

void numx_bench_ntt(void)
{
    numx_q15_t a[256], b[256], out[256];
    int64_t t0;
    int i;

    /* fill with stable non-zero data */
    for (i = 0; i < 256; i++) {
        a[i] = (numx_q15_t)((i * 17 + 3) % 3329);
        b[i] = (numx_q15_t)((i * 31 + 7) % 3329);
    }

    printf("\n  numx_ntt (n=256, q=3329)\n");
    printf("  %-38s | %-6s | %-9s | %s\n", "function", "N", "total", "per call");
    printf("  %.70s\n",
           "----------------------------------------------------------------------");

    /* numx_ntt_forward */
    t0 = now_ns();
    for (i = 0; i < BENCH_N; i++) {
        numx_q15_t tmp[256];
        memcpy(tmp, a, sizeof(tmp));
        numx_ntt_forward(tmp);
    }
    bprint("numx_ntt_forward", now_ns() - t0, BENCH_N);

    /* numx_ntt_inverse */
    {
        numx_q15_t ntt_a[256];
        memcpy(ntt_a, a, sizeof(ntt_a));
        numx_ntt_forward(ntt_a);
        t0 = now_ns();
        for (i = 0; i < BENCH_N; i++) {
            numx_q15_t tmp[256];
            memcpy(tmp, ntt_a, sizeof(tmp));
            numx_ntt_inverse(tmp);
        }
        bprint("numx_ntt_inverse", now_ns() - t0, BENCH_N);
    }

    /* numx_ntt_polymul (full pipeline) */
    t0 = now_ns();
    for (i = 0; i < BENCH_N; i++)
        numx_ntt_polymul(a, b, out);
    bprint("numx_ntt_polymul", now_ns() - t0, BENCH_N);

    /* numx_ntt_poly_add */
    t0 = now_ns();
    for (i = 0; i < BENCH_N; i++)
        numx_ntt_poly_add(a, b, out);
    bprint("numx_ntt_poly_add", now_ns() - t0, BENCH_N);

    /* numx_ntt_poly_sub */
    t0 = now_ns();
    for (i = 0; i < BENCH_N; i++)
        numx_ntt_poly_sub(a, b, out);
    bprint("numx_ntt_poly_sub", now_ns() - t0, BENCH_N);
}
