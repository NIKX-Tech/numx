cmake_minimum_required(VERSION 3.15)
project(numx_tests_x86 C)

# Host-native float32 tests — NOT for ESP32.
# numx_real_t = float (NUMX_USE_DOUBLE is NOT set).
# To build 32-bit on a 64-bit host pass -DNUMX_FORCE_32BIT=ON; this requires
# multilib support (gcc-multilib / clang -m32 / MSVC Win32 generator).

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(NUMX_INC  ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
set(NUMX_SRC  ${CMAKE_CURRENT_SOURCE_DIR}/../../src)
set(UNITY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../unity)

option(NUMX_FORCE_32BIT "Compile as 32-bit x86 (requires multilib)" OFF)
if(NUMX_FORCE_32BIT)
    if(MSVC)
        message(STATUS "NUMX_FORCE_32BIT: select the Win32 CMake generator instead.")
    else()
        add_compile_options(-m32)
        add_link_options(-m32)
    endif()
endif()

if(MSVC)
    add_compile_options(/W3 /wd4100)
else()
    add_compile_options(-Wall -Wextra -Wno-unused-parameter)
endif()

# numx library — float32 (no NUMX_USE_DOUBLE)
file(GLOB NUMX_SRCS ${NUMX_SRC}/*.c)
add_library(numx_f32 STATIC ${NUMX_SRCS})
target_include_directories(numx_f32 PUBLIC ${NUMX_INC})

enable_testing()

# ── Per-module executables ────────────────────────────────────────────────────
# Each generates a tiny main() stub so the module can be run in isolation.
# Build one:   cmake --build . --target numx_test_x86_linalg
# Run one:     ./numx_test_x86_linalg          (Linux/macOS)
#              numx_test_x86_linalg.exe         (Windows)
# Via CTest:   ctest -R linalg

function(add_numx_module_test MODULE)
    set(STUB "${CMAKE_CURRENT_BINARY_DIR}/runner_${MODULE}.c")
    file(WRITE ${STUB}
        "#include \"unity.h\"\n"
        "void numx_test_${MODULE}(void);\n"
        "void setUp(void){}\n"
        "void tearDown(void){}\n"
        "int main(void){UNITY_BEGIN();numx_test_${MODULE}();return UNITY_END();}\n"
    )
    add_executable(numx_test_x86_${MODULE}
        ${UNITY_DIR}/unity.c
        test_${MODULE}.c
        ${STUB}
    )
    target_include_directories(numx_test_x86_${MODULE} PRIVATE ${NUMX_INC} ${UNITY_DIR})
    if(UNIX)
        target_link_libraries(numx_test_x86_${MODULE} numx_f32 m)
    else()
        target_link_libraries(numx_test_x86_${MODULE} numx_f32)
    endif()
    add_test(NAME numx_x86_${MODULE} COMMAND numx_test_x86_${MODULE})
endfunction()

set(NUMX_MODULES
    linalg stats roots integrate differentiate
    interpolate poly ode signal fft
    autodiff compressed_sensing sketch
)
foreach(MOD ${NUMX_MODULES})
    add_numx_module_test(${MOD})
endforeach()

# ── Full suite (all modules in one run) ──────────────────────────────────────
add_executable(numx_test_x86
    ${UNITY_DIR}/unity.c
    test_runner.c
    test_linalg.c
    test_stats.c
    test_roots.c
    test_integrate.c
    test_differentiate.c
    test_interpolate.c
    test_poly.c
    test_ode.c
    test_signal.c
    test_fft.c
    test_autodiff.c
    test_compressed_sensing.c
    test_sketch.c
)
target_include_directories(numx_test_x86 PRIVATE ${NUMX_INC} ${UNITY_DIR})
if(UNIX)
    target_link_libraries(numx_test_x86 numx_f32 m)
else()
    target_link_libraries(numx_test_x86 numx_f32)
endif()
add_test(NAME numx_x86 COMMAND numx_test_x86)

# ── Windows performance benchmark (float32) ──────────────────────────────────
add_executable(numx_bench_x86
    ${CMAKE_CURRENT_SOURCE_DIR}/../../benchmarks/bench_win.c
)
target_include_directories(numx_bench_x86 PRIVATE ${NUMX_INC})
if(UNIX)
    target_link_libraries(numx_bench_x86 numx_f32 m)
else()
    target_link_libraries(numx_bench_x86 numx_f32)
endif()
