#ifndef NUMX_TESTS_H
#define NUMX_TESTS_H

#include "test_helpers.h"

void test_linalg(numxt_ctx_t *ctx);
void test_stats(numxt_ctx_t *ctx);
void test_roots(numxt_ctx_t *ctx);
void test_integrate(numxt_ctx_t *ctx);
void test_differentiate(numxt_ctx_t *ctx);
void test_interpolate(numxt_ctx_t *ctx);
void test_poly(numxt_ctx_t *ctx);
void test_ode(numxt_ctx_t *ctx);
void test_signal(numxt_ctx_t *ctx);
void test_fft(numxt_ctx_t *ctx);
void test_autodiff(numxt_ctx_t *ctx);
void test_compressed_sensing(numxt_ctx_t *ctx);
void test_sketch(numxt_ctx_t *ctx);

#endif /* NUMX_TESTS_H */
