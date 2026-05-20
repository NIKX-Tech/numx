/**
 * @file test_runner.c
 * @brief numx master test runner.
 *
 * Calls each module's test suite in order. Add a numx_test_<module>()
 * declaration and call as each Phase 1–3 module is implemented.
 *
 * Each test_<module>.c must expose:
 *   void numx_test_<module>(void);
 * rather than its own main(). The UNITY_BEGIN / UNITY_END pair here
 * accumulates results across all suites into one exit code for CTest.
 */

#include "unity.h"

/* Phase 1 suite declarations:                                        */
void numx_test_linalg(void);
void numx_test_stats(void);
void numx_test_roots(void);
void numx_test_integrate(void);
void numx_test_differentiate(void);
void numx_test_interpolate(void);
void numx_test_poly(void);
void numx_test_ode(void);

/* Phase 2 suite declarations:                                        */
void numx_test_signal(void);
void numx_test_fft(void);

/* Phase 3 suite declarations:                                        */
void numx_test_autodiff(void);
void numx_test_compressed_sensing(void);
void numx_test_sketch(void);
/* void numx_test_ntt(void);                */

void setUp(void) {}
void tearDown(void) {}

int main(void)
{
    UNITY_BEGIN();

    /* Phase 1: */
    numx_test_linalg();
    numx_test_stats();
    numx_test_roots();
    numx_test_integrate();
    numx_test_differentiate();
    numx_test_interpolate();
    numx_test_poly();
    numx_test_ode();

    /* Phase 2: */
    numx_test_signal();
    numx_test_fft();

    /* Phase 3: */
    numx_test_autodiff();
    numx_test_compressed_sensing();
    numx_test_sketch();
    /* numx_test_ntt();                */

    return UNITY_END();
}
