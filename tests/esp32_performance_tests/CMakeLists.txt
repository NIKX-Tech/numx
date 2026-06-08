idf_component_register(
    SRCS
        "bench_runner.c"
        "math_helpers.c"
        "bench_phase1.c"
        "bench_phase2.c"
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS "."
    PRIV_REQUIRES numx esp_timer freertos esp_system
)
target_compile_options(${COMPONENT_LIB} PRIVATE -O2 -std=gnu99 -Wno-unused-parameter)
