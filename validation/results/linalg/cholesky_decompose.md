# Validation: numx_cholesky_decompose

Covers: `numx_cholesky_decompose`

---

## ARM64 — macOS 26.5.1 / Apple M4 Pro / Apple clang 21.0.0 / float32
**Validator:** Erfan Jazeb Nikoo | **Date:** 2026-07-09 | **Commit:** 73a7610

> Implementation contributed by Erfan Esmaeili ([#51](https://github.com/NIKX-Tech/numx/pull/51)).
> Verified with AddressSanitizer and UndefinedBehaviorSanitizer (335/335 tests, zero
> issues) and compiled clean under `-std=c99 -pedantic-errors -Wall -Wextra -Werror`
> in both float32 and float64. The sections below complete the full multi-platform
> sweep: Windows x86/x64, Linux x86 (32-bit)/x86-64 (WSL2), Raspberry Pi 4 (armhf),
> and ESP32-S3 (Xtensa LX7).

### Test cases

| Function | Input / scenario | Expected | Computed | Error | Pass |
|----------|-----------------|----------|----------|-------|------|
| cholesky_decompose | 3×3 textbook SPD, L[0][0] | 2.0 | 2.0000000 | 0.00e+00 | ✅ |
| cholesky_decompose | 3×3 textbook SPD, L[1][0] | 6.0 | 6.0000000 | 0.00e+00 | ✅ |
| cholesky_decompose | 3×3 textbook SPD, L[1][1] | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| cholesky_decompose | 3×3 textbook SPD, L[2][0] | -8.0 | -8.0000000 | 0.00e+00 | ✅ |
| cholesky_decompose | 3×3 textbook SPD, L[2][1] | 5.0 | 5.0000000 | 0.00e+00 | ✅ |
| cholesky_decompose | 3×3 textbook SPD, L[2][2] | 3.0 | 3.0000000 | 0.00e+00 | ✅ |
| cholesky_decompose | L·L^T reconstruction vs original A | A | A | 0.00e+00 | ✅ |
| cholesky_decompose | non-SPD input (zero diagonal pivot) | rc=NUMX_ERR_SINGULAR | rc=NUMX_ERR_SINGULAR | — | ✅ |
| null-ptr guards | A, L | rc=NUMX_ERR_NULL_PTR | rc=NUMX_ERR_NULL_PTR | — | ✅ |
| invalid-arg guard | n=0 | rc=NUMX_ERR_INVALID_ARG | rc=NUMX_ERR_INVALID_ARG | — | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| cholesky_decompose 3×3 | 100,000 | 2,195 µs | 21 ns |

**RESULTS: 6 PASS / 0 FAIL / 6 TOTAL** (Unity suite, `tests/test_linalg.c`); validation-runner checks above match.

---

## Windows x86 - Windows 11 / MSVC 19.51.36248.0 (VS 2026 Build Tools) / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-07-12 | **Commit:** 4dc881f

> Built with the Win32 CMake generator platform (`cmake -S . -B build_x86 -A Win32`),
> producing a 32-bit Intel i386 binary, confirmed via `file build_x86/Release/numx_tests.exe`.
> Uses `numx_real_t = float` (no `NUMX_USE_DOUBLE`). No existing CMake files edited.

### Test cases

| Test | Result |
|------|--------|
| test_cholesky_decompose_3x3_success | ✅ |
| test_cholesky_decompose_residual_reconstruction | ✅ |
| test_cholesky_decompose_non_spd_fail | ✅ |
| test_cholesky_decompose_null_A | ✅ |
| test_cholesky_decompose_null_L | ✅ |
| test_cholesky_decompose_invalid_dim | ✅ |

*337 / 337 Unity tests PASS*

### Validation-runner checks

| Function | Input / scenario | Expected | Computed | Error | Pass |
|----------|-----------------|----------|----------|-------|------|
| cholesky_decompose | 3×3 textbook SPD, L[0][0] | 2.0 | 2.00000000 | 0.00e+00 | ✅ |
| cholesky_decompose | 3×3 textbook SPD, L[1][0] | 6.0 | 6.00000000 | 0.00e+00 | ✅ |
| cholesky_decompose | 3×3 textbook SPD, L[1][1] | 1.0 | 1.00000000 | 0.00e+00 | ✅ |
| cholesky_decompose | 3×3 textbook SPD, L[2][0] | -8.0 | -8.00000000 | 0.00e+00 | ✅ |
| cholesky_decompose | 3×3 textbook SPD, L[2][1] | 5.0 | 5.00000000 | 0.00e+00 | ✅ |
| cholesky_decompose | 3×3 textbook SPD, L[2][2] | 3.0 | 3.00000000 | 0.00e+00 | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| cholesky_decompose 3×3 | 100,000 | 3,041 µs | 30 ns |

**RESULTS: 6 PASS / 0 FAIL / 6 TOTAL** (Unity suite); validation-runner checks above match.

---

## Windows x64 - Windows 11 / MSVC 19.51.36248.0 (VS 2026 Build Tools) / float64
**Validator:** Amir Ab Khoshk | **Date:** 2026-07-12 | **Commit:** 4dc881f

> Built from the root CMakeLists.txt with `CMAKE_C_FLAGS="-DNUMX_USE_DOUBLE -DUNITY_INCLUDE_DOUBLE"`
> passed on the command line, so `numx_real_t = double` throughout without editing any
> existing CMake files. Confirmed a 64-bit x86-64 binary (PE32+) via
> `file build_x64_f64/Release/numx_tests.exe`.

### Test cases

| Test | Result |
|------|--------|
| test_cholesky_decompose_3x3_success | ✅ |
| test_cholesky_decompose_residual_reconstruction | ✅ |
| test_cholesky_decompose_non_spd_fail | ✅ |
| test_cholesky_decompose_null_A | ✅ |
| test_cholesky_decompose_null_L | ✅ |
| test_cholesky_decompose_invalid_dim | ✅ |

*337 / 337 Unity tests PASS*

### Validation-runner checks

| Function | Input / scenario | Expected | Computed | Error | Pass |
|----------|-----------------|----------|----------|-------|------|
| cholesky_decompose | 3×3 textbook SPD, L[0][0] | 2.0 | 2.00000000 | 0.00e+00 | ✅ |
| cholesky_decompose | 3×3 textbook SPD, L[1][0] | 6.0 | 6.00000000 | 0.00e+00 | ✅ |
| cholesky_decompose | 3×3 textbook SPD, L[1][1] | 1.0 | 1.00000000 | 0.00e+00 | ✅ |
| cholesky_decompose | 3×3 textbook SPD, L[2][0] | -8.0 | -8.00000000 | 0.00e+00 | ✅ |
| cholesky_decompose | 3×3 textbook SPD, L[2][1] | 5.0 | 5.00000000 | 0.00e+00 | ✅ |
| cholesky_decompose | 3×3 textbook SPD, L[2][2] | 3.0 | 3.00000000 | 0.00e+00 | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| cholesky_decompose 3×3 | 100,000 | 2,794 µs | 27 ns |

**RESULTS: 6 PASS / 0 FAIL / 6 TOTAL** (Unity suite); validation-runner checks above match.

---

## Linux x86 - Ubuntu 24.04.4 LTS (WSL2, kernel 6.18.33.2-microsoft-standard-WSL2) / gcc 13.3.0 (-m32, gcc-multilib) / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-07-12 | **Commit:** 4dc881f

> Configured with `CMAKE_C_FLAGS=-m32 CMAKE_EXE_LINKER_FLAGS=-m32` against the root
> CMakeLists.txt, no existing files edited. Confirmed a 32-bit ELF binary via
> `file build_wsl_x86/numx_tests` (`ELF 32-bit LSB pie executable, Intel 80386`).
> Required `gcc-multilib`/`g++-multilib` to be installed in WSL first (installed by
> the user with sudo, not by this session).

### Test cases

| Test | Result |
|------|--------|
| test_cholesky_decompose_3x3_success | ✅ |
| test_cholesky_decompose_residual_reconstruction | ✅ |
| test_cholesky_decompose_non_spd_fail | ✅ |
| test_cholesky_decompose_null_A | ✅ |
| test_cholesky_decompose_null_L | ✅ |
| test_cholesky_decompose_invalid_dim | ✅ |

*337 / 337 Unity tests PASS*

### Validation-runner checks

| Function | Input / scenario | Expected | Computed | Error | Pass |
|----------|-----------------|----------|----------|-------|------|
| cholesky_decompose | 3×3 textbook SPD, L[0][0] | 2.0 | 2.00000000 | 0.00e+00 | ✅ |
| cholesky_decompose | 3×3 textbook SPD, L[1][0] | 6.0 | 6.00000000 | 0.00e+00 | ✅ |
| cholesky_decompose | 3×3 textbook SPD, L[1][1] | 1.0 | 1.00000000 | 0.00e+00 | ✅ |
| cholesky_decompose | 3×3 textbook SPD, L[2][0] | -8.0 | -8.00000000 | 0.00e+00 | ✅ |
| cholesky_decompose | 3×3 textbook SPD, L[2][1] | 5.0 | 5.00000000 | 0.00e+00 | ✅ |
| cholesky_decompose | 3×3 textbook SPD, L[2][2] | 3.0 | 3.00000000 | 0.00e+00 | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| cholesky_decompose 3×3 | 100,000 | 4,129 µs | 41 ns |

**RESULTS: 6 PASS / 0 FAIL / 6 TOTAL** (Unity suite); validation-runner checks above match.

---

## Linux x86-64 - Ubuntu 24.04.4 LTS (WSL2, kernel 6.18.33.2-microsoft-standard-WSL2) / gcc 13.3.0 / float64
**Validator:** Amir Ab Khoshk | **Date:** 2026-07-12 | **Commit:** 4dc881f

> Built from the root CMakeLists.txt with `CMAKE_C_FLAGS="-DNUMX_USE_DOUBLE -DUNITY_INCLUDE_DOUBLE"`
> passed on the command line, no existing CMake files edited. `numx_tests`, `numx_val_runner`,
> and `numx_bench` all build cleanly under float64 at this commit.

### Test cases

| Test | Result |
|------|--------|
| test_cholesky_decompose_3x3_success | ✅ |
| test_cholesky_decompose_residual_reconstruction | ✅ |
| test_cholesky_decompose_non_spd_fail | ✅ |
| test_cholesky_decompose_null_A | ✅ |
| test_cholesky_decompose_null_L | ✅ |
| test_cholesky_decompose_invalid_dim | ✅ |

*337 / 337 Unity tests PASS*

### Validation-runner checks

| Function | Input / scenario | Expected | Computed | Error | Pass |
|----------|-----------------|----------|----------|-------|------|
| cholesky_decompose | 3×3 textbook SPD, L[0][0] | 2.0 | 2.00000000 | 0.00e+00 | ✅ |
| cholesky_decompose | 3×3 textbook SPD, L[1][0] | 6.0 | 6.00000000 | 0.00e+00 | ✅ |
| cholesky_decompose | 3×3 textbook SPD, L[1][1] | 1.0 | 1.00000000 | 0.00e+00 | ✅ |
| cholesky_decompose | 3×3 textbook SPD, L[2][0] | -8.0 | -8.00000000 | 0.00e+00 | ✅ |
| cholesky_decompose | 3×3 textbook SPD, L[2][1] | 5.0 | 5.00000000 | 0.00e+00 | ✅ |
| cholesky_decompose | 3×3 textbook SPD, L[2][2] | 3.0 | 3.00000000 | 0.00e+00 | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| cholesky_decompose 3×3 | 100,000 | 4,652 µs | 46 ns |

**RESULTS: 6 PASS / 0 FAIL / 6 TOTAL** (Unity suite); validation-runner checks above match.

---

## ARMv7 (armhf) — Raspbian GNU/Linux 13 (trixie) / Raspberry Pi 4 Model B / gcc 14.2.0 / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-07-12 | **Commit:** 4dc881f

> Validated over SSH (`sellol@raspberrypi.local`). Kernel is 64-bit
> (`6.18.33+rpt-rpi-v8 aarch64`) but the OS userland/toolchain is 32-bit armhf
> (`dpkg --print-architecture` → `armhf`), so the actual build output is ARMv7/EABI5,
> not aarch64 — confirmed via `file build/numx_tests`
> (`ELF 32-bit LSB executable, ARM, EABI5`). The working tree at commit 4dc881f was
> copied to the Pi via `git archive` + SFTP into a fresh directory
> (`~/numx_cholesky_validation`, separate from the existing `~/numx` clone, which was
> behind on a different fork/branch) since the existing clone didn't yet have
> `numx_cholesky_decompose`. Built from the root CMakeLists.txt, unmodified, Release
> config.

### Test cases

| Test | Result |
|------|--------|
| test_cholesky_decompose_3x3_success | ✅ |
| test_cholesky_decompose_residual_reconstruction | ✅ |
| test_cholesky_decompose_non_spd_fail | ✅ |
| test_cholesky_decompose_null_A | ✅ |
| test_cholesky_decompose_null_L | ✅ |
| test_cholesky_decompose_invalid_dim | ✅ |

*337 / 337 Unity tests PASS*

### Validation-runner checks

| Function | Input / scenario | Expected | Computed | Error | Pass |
|----------|-----------------|----------|----------|-------|------|
| cholesky_decompose | 3×3 textbook SPD, L[0][0] | 2.0 | 2.00000000 | 0.00e+00 | ✅ |
| cholesky_decompose | 3×3 textbook SPD, L[1][0] | 6.0 | 6.00000000 | 0.00e+00 | ✅ |
| cholesky_decompose | 3×3 textbook SPD, L[1][1] | 1.0 | 1.00000000 | 0.00e+00 | ✅ |
| cholesky_decompose | 3×3 textbook SPD, L[2][0] | -8.0 | -8.00000000 | 0.00e+00 | ✅ |
| cholesky_decompose | 3×3 textbook SPD, L[2][1] | 5.0 | 5.00000000 | 0.00e+00 | ✅ |
| cholesky_decompose | 3×3 textbook SPD, L[2][2] | 3.0 | 3.00000000 | 0.00e+00 | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| cholesky_decompose 3×3 | 100,000 | 17,304 µs | 173 ns |

**RESULTS: 6 PASS / 0 FAIL / 6 TOTAL** (Unity suite); validation-runner checks above match.

---

## ESP32-S3 - ESP-IDF v5.5.2 / Xtensa LX7 / xtensa-esp-elf-gcc 14.2.0 (esp-14.2.0) / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-07-12 | **Commit:** 4dc881f

> The existing on-device test harness (`tests/esp32_tests/`) has no `cholesky_decompose`
> coverage, same gap the NTT module had. Rather than editing that harness (would require
> touching `CMakeLists.txt`, `tests.h`, `esp32_tests.c`), a new self-contained example
> project was created at `examples/esp32_cholesky_test/` (`CMakeLists.txt`,
> `main/CMakeLists.txt`, `main/main.c`, `main/sdkconfig.defaults` — all new files, nothing
> existing modified), mirroring the pattern already established at
> `examples/esp32_ntt_test/`. It links `src/linalg.c` directly (self-contained, no other
> numx source dependencies) and mirrors the 6 test cases in `tests/test_linalg.c`,
> expanded into 23 individual value/status assertions (0 dependency on Unity). Built with
> `idf.py set-target esp32s3` / `idf.py build`, flashed to a real ESP32-S3 DevKit over USB
> (COM5, VID 303A) via `idf.py -p COM5 flash`, output captured over UART at 115200 baud.
> Unlike the NTT run, no stack overflow / watchdog issue was observed — cholesky's 3×3
> buffers are far smaller than NTT's 256-element `numx_q15_t` arrays, so the default
> `CONFIG_ESP_MAIN_TASK_STACK_SIZE=8192` was sufficient and all 100,000 benchmark
> iterations completed normally.

### Test cases

| Test | Result |
|------|--------|
| test_cholesky_decompose_3x3_success | ✅ |
| test_cholesky_decompose_residual_reconstruction | ✅ |
| test_cholesky_decompose_non_spd_fail | ✅ |
| test_cholesky_decompose_null_A | ✅ |
| test_cholesky_decompose_null_L | ✅ |
| test_cholesky_decompose_invalid_dim | ✅ |

*23 / 23 assertions PASS (6 test cases, expanded per-element/per-status-code)*

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| cholesky_decompose 3×3 | 100,000 | 698,140 µs | 6,981 ns |

**RESULTS: 23 PASS / 0 FAIL / 23 TOTAL**
