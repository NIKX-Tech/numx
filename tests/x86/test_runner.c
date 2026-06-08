/**
 * @file test_runner.c
 * @brief numx master test runner — float32 / x86 host build.
 *
 * Compile with the CMakeLists.txt in this directory.
 * numx_real_t = float (NUMX_USE_DOUBLE is NOT defined).
 */

#include "unity.h"

void numx_test_linalg(void);
void numx_test_stats(void);
void numx_test_roots(void);
void numx_test_integrate(void);
void numx_test_differentiate(void);
void numx_test_interpolate(void);
void numx_test_poly(void);
void numx_test_ode(void);
void numx_test_signal(void);
void numx_test_fft(void);
void numx_test_autodiff(void);
void numx_test_compressed_sensing(void);
void numx_test_sketch(void);

void setUp(void) {}
void tearDown(void) {}

int main(void)
{
    UNITY_BEGIN();

    numx_test_linalg();
    numx_test_stats();
    numx_test_roots();
    numx_test_integrate();
    numx_test_differentiate();
    numx_test_interpolate();
    numx_test_poly();
    numx_test_ode();
    numx_test_signal();
    numx_test_fft();
    numx_test_autodiff();
    numx_test_compressed_sensing();
    numx_test_sketch();

    return UNITY_END();
}
