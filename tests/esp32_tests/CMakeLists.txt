idf_component_register(
    SRCS
        "esp32_tests.c"
        "test_helpers.c"
        "math_helpers.c"
        "test_linalg.c"
        "test_stats.c"
        "test_roots.c"
        "test_integrate.c"
        "test_differentiate.c"
        "test_interpolate.c"
        "test_poly.c"
        "test_ode.c"
        "test_signal.c"
        "test_fft.c"
        "test_autodiff.c"
        "test_compressed_sensing.c"
        "test_sketch.c"
        "bench.c"
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS "."
    PRIV_REQUIRES numx esp_timer freertos esp_system
)
target_compile_options(${COMPONENT_LIB} PRIVATE -O2 -std=c99 -Wno-unused-parameter)
