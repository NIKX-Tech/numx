#ifndef NUMX_ESP32_TESTS_H
#define NUMX_ESP32_TESTS_H

/**
 * @brief  Test execution context.  Declare one on the stack in app_main,
 *         zero-initialise it, then pass it to every test function.
 *         No global state — fully reentrant (Rule 4).
 */
typedef struct {
    int pass; /**< Assertions passed. */
    int fail; /**< Assertions failed. */
} numxt_ctx_t;

/**
 * @brief  Run the linear-algebra test suite.
 * @param[in,out] ctx  Test context accumulating pass/fail counts.
 */
void test_linalg(numxt_ctx_t *ctx);

/**
 * @brief  Run the statistics test suite.
 * @param[in,out] ctx  Test context accumulating pass/fail counts.
 */
void test_stats(numxt_ctx_t *ctx);

/**
 * @brief  Run the root-finding test suite.
 * @param[in,out] ctx  Test context accumulating pass/fail counts.
 */
void test_roots(numxt_ctx_t *ctx);

/**
 * @brief  Run the numerical integration test suite.
 * @param[in,out] ctx  Test context accumulating pass/fail counts.
 */
void test_integrate(numxt_ctx_t *ctx);

/**
 * @brief  Run the numerical differentiation test suite.
 * @param[in,out] ctx  Test context accumulating pass/fail counts.
 */
void test_differentiate(numxt_ctx_t *ctx);

/**
 * @brief  Run the interpolation test suite.
 * @param[in,out] ctx  Test context accumulating pass/fail counts.
 */
void test_interpolate(numxt_ctx_t *ctx);

/**
 * @brief  Run the polynomial test suite.
 * @param[in,out] ctx  Test context accumulating pass/fail counts.
 */
void test_poly(numxt_ctx_t *ctx);

/**
 * @brief  Run the ODE solver test suite.
 * @param[in,out] ctx  Test context accumulating pass/fail counts.
 */
void test_ode(numxt_ctx_t *ctx);

/**
 * @brief  Run the signal processing test suite.
 * @param[in,out] ctx  Test context accumulating pass/fail counts.
 */
void test_signal(numxt_ctx_t *ctx);

/**
 * @brief  Run the FFT/IFFT test suite.
 * @param[in,out] ctx  Test context accumulating pass/fail counts.
 */
void test_fft(numxt_ctx_t *ctx);

/**
 * @brief  Run the automatic differentiation test suite.
 * @param[in,out] ctx  Test context accumulating pass/fail counts.
 */
void test_autodiff(numxt_ctx_t *ctx);

/**
 * @brief  Run the compressed sensing (OMP/ISTA) test suite.
 * @param[in,out] ctx  Test context accumulating pass/fail counts.
 */
void test_compressed_sensing(numxt_ctx_t *ctx);

/**
 * @brief  Run the randomized SVD sketch test suite.
 * @param[in,out] ctx  Test context accumulating pass/fail counts.
 */
void test_sketch(numxt_ctx_t *ctx);

/**
 * @brief  Run all thirteen test suites in sequence.
 * @param[in,out] ctx  Test context accumulating pass/fail counts.
 */
void run_all_tests(numxt_ctx_t *ctx);

/**
 * @brief  Run the benchmark suite and print timing results to stdout.
 *         Does not affect the test context.
 */
void run_benchmarks(void);

#endif /* NUMX_ESP32_TESTS_H */
