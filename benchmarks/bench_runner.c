/**
 * @file bench_runner.c
 * @brief numx master benchmark runner.
 *
 * Calls each module's benchmark suite in order. Add a numx_bench_<module>()
 * declaration and call as each Phase 1–3 module is implemented.
 *
 * Each bench_<module>.c must expose:
 *   void numx_bench_<module>(void);
 *
 * Output format (one line per benchmark):
 *   [function_name] | N=[count] | [total_us] us | [per_call_ns] ns/call
 */

#include <stdio.h>

/* Phase 1 suite declarations (uncomment as modules are added):      */
/* void numx_bench_linalg(void); */

/* Phase 2 suite declarations:                                        */
/* void numx_bench_fft(void);    */

/* Phase 3 suite declarations:                                        */
/* void numx_bench_autodiff(void);           */
/* void numx_bench_compressed_sensing(void); */

int main(void)
{
    printf("numx benchmark suite\n");
    printf("%-40s | %-8s | %-12s | %s\n",
           "function", "N", "total", "per call");
    printf("%.72s\n",
           "------------------------------------------------"
           "------------------------");

    /* Phase 1 (uncomment as modules are added): */
    /* numx_bench_linalg(); */

    /* Phase 2: */
    /* numx_bench_fft(); */

    /* Phase 3: */
    /* numx_bench_autodiff();           */
    /* numx_bench_compressed_sensing(); */

    printf("\n(no benchmarks registered yet - add modules to reach Phase 1)\n");
    return 0;
}
