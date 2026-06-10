#ifndef NUMX_ESP32_PERFORMANCE_TESTS_H
#define NUMX_ESP32_PERFORMANCE_TESTS_H

/* Run all Phase 1 benchmarks (linalg, stats, roots, integrate,
   differentiate, interpolate, poly, ode). */
void bench_phase1(void);

/* Run all Phase 2 benchmarks (autodiff, fft, signal, sketch,
   compressed_sensing). */
void bench_phase2(void);

/* Run Phase 1 + Phase 2 in sequence. */
void run_performance_tests(void);

#endif /* NUMX_ESP32_PERFORMANCE_TESTS_H */
