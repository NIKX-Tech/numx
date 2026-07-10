cmake_minimum_required(VERSION 3.16)
project(numx VERSION 1.0.0 LANGUAGES C)

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

# ── Function/data-section garbage collection ──────────────────────────
# Puts each function and variable in its own linker section so that, combined
# with the equivalent dead-stripping flag at final link time, unused code is
# dropped at function granularity rather than whole-object-file granularity.
# This only takes full effect in the executables built by this file
# (numx_tests, numx_bench, etc.) -- consumers linking against the numx static
# library elsewhere need to pass the same flags at their own final link step
# to get the same benefit, since a static library archive doesn't control its
# consumer's link flags.
#
# Apple's linker (macOS default) does not understand GNU ld/lld's
# --gc-sections syntax and fails outright if you pass it; it uses -dead_strip
# instead. GNU ld, lld, and every embedded cross-toolchain tested (arm-none-
# eabi-gcc, xtensa-esp32s3-elf-gcc) accept --gc-sections normally.
if(MSVC)
  add_compile_options(/Gy)
  add_link_options(/OPT:REF /OPT:ICF)
elseif(APPLE)
  add_compile_options(-ffunction-sections -fdata-sections)
  add_link_options(-Wl,-dead_strip)
else()
  add_compile_options(-ffunction-sections -fdata-sections)
  add_link_options(-Wl,--gc-sections)
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
    src/ntt.c
)

add_library(numx STATIC ${NUMX_SOURCES})
add_library(numx::numx ALIAS numx)
target_include_directories(numx PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# ── Install & CMake package export ────────────────────────────────────
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(TARGETS numx
    EXPORT numxTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT numxTargets
    FILE numxTargets.cmake
    NAMESPACE numx::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/numx
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/numxConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/numxConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/numx
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/numxConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/numxConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/numxConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/numx
)

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
    tests/test_ntt.c
)

add_executable(numx_tests ${TEST_SOURCES})
target_include_directories(numx_tests PRIVATE include tests/unity)
target_link_libraries(numx_tests PRIVATE numx)

add_test(NAME numx_tests COMMAND numx_tests)

# ── Benchmarks ────────────────────────────────────────────────────────
# Add each new bench_<module>.c file here as it is implemented.
if(WIN32)
    set(BENCH_SOURCES benchmarks/bench_win.c)
else()
    set(BENCH_SOURCES
        benchmarks/bench_runner.c
        # Phase 1 benchmarks (add as implemented):
        # benchmarks/bench_linalg.c
        # Phase 2 benchmarks:
        # benchmarks/bench_fft.c
        # Phase 3 benchmarks:
        # benchmarks/bench_autodiff.c
        # benchmarks/bench_compressed_sensing.c
        benchmarks/bench_ntt.c
    )
endif()

add_executable(numx_bench ${BENCH_SOURCES})
target_include_directories(numx_bench PRIVATE include)
target_link_libraries(numx_bench PRIVATE numx)

# ── Examples ──────────────────────────────────────────────────────────
add_subdirectory(examples)

# ── Validation runner ─────────────────────────────────────────────────
add_executable(numx_val_runner validation/c/val_runner.c)
target_include_directories(numx_val_runner PRIVATE include)
target_link_libraries(numx_val_runner PRIVATE numx)
if(UNIX)
    target_link_libraries(numx_val_runner PRIVATE m)
endif()

add_executable(numx_val_bench_phase2 validation/c/val_bench_phase2.c)
target_include_directories(numx_val_bench_phase2 PRIVATE include)
target_link_libraries(numx_val_bench_phase2 PRIVATE numx)
if(UNIX)
    target_link_libraries(numx_val_bench_phase2 PRIVATE m)
endif()
