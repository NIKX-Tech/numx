cmake_minimum_required(VERSION 3.16)
project(numx VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)  # no GCC extensions — pure C99

# ── Compiler warnings ─────────────────────────────────────────────────
if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Werror
                      -Wno-unused-parameter)
endif()

# ── numx static library ───────────────────────────────────────────────
# Add each new module's .c file here as it is implemented.
set(NUMX_SOURCES
    src/numx.c
    # Phase 1 — Foundation (add as implemented):
    src/linalg.c
    src/stats.c
    src/roots.c
    src/integrate.c
    src/differentiate.c
    src/interpolate.c
    src/poly.c
    src/ode.c
    # Phase 2 — Signal Processing:
    src/signal.c
    src/fft.c
    # Phase 3 — Unicorn Algorithms:
    src/autodiff.c
    src/compressed_sensing.c
    src/sketch.c
    # src/ntt.c
)

add_library(numx STATIC ${NUMX_SOURCES})
target_include_directories(numx PUBLIC include)

# ── Tests ─────────────────────────────────────────────────────────────
enable_testing()

# Add each new test_<module>.c file here as it is implemented.
set(TEST_SOURCES
    tests/test_runner.c
    tests/unity/unity.c
    # Phase 1 tests (add as implemented):
    tests/test_linalg.c
    tests/test_stats.c
    tests/test_roots.c
    tests/test_integrate.c
    tests/test_differentiate.c
    tests/test_interpolate.c
    tests/test_poly.c
    tests/test_ode.c
    # Phase 2 tests:
    tests/test_signal.c
    tests/test_fft.c
    # Phase 3 tests:
    tests/test_autodiff.c
    tests/test_compressed_sensing.c
    tests/test_sketch.c
    # tests/test_ntt.c
)

add_executable(numx_tests ${TEST_SOURCES})
target_include_directories(numx_tests PRIVATE include tests/unity)
target_link_libraries(numx_tests PRIVATE numx)

add_test(NAME numx_tests COMMAND numx_tests)

# ── Benchmarks ────────────────────────────────────────────────────────
# Add each new bench_<module>.c file here as it is implemented.
set(BENCH_SOURCES
    benchmarks/bench_runner.c
    # Phase 1 benchmarks (add as implemented):
    # benchmarks/bench_linalg.c
    # Phase 2 benchmarks:
    # benchmarks/bench_fft.c
    # Phase 3 benchmarks:
    # benchmarks/bench_autodiff.c
    # benchmarks/bench_compressed_sensing.c
)

add_executable(numx_bench ${BENCH_SOURCES})
target_include_directories(numx_bench PRIVATE include)
target_link_libraries(numx_bench PRIVATE numx)
