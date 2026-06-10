#include "esp32_tests.h"
#include "tests.h"

void run_all_tests(numxt_ctx_t *ctx)
{
    test_linalg(ctx);
    test_stats(ctx);
    test_roots(ctx);
    test_integrate(ctx);
    test_differentiate(ctx);
    test_interpolate(ctx);
    test_poly(ctx);
    test_ode(ctx);
    test_signal(ctx);
    test_fft(ctx);
    test_autodiff(ctx);
    test_compressed_sensing(ctx);
    test_sketch(ctx);
}
