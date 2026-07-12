# Validation prompt: numx_cholesky_decompose on Raspberry Pi 4 and ESP32-S3

> **Status: complete ✅ (2026-07-12).** Both platforms below have been validated — see
> the "ARMv7 (armhf)" and "ESP32-S3" sections in
> [`validation/results/linalg/cholesky_decompose.md`](../results/linalg/cholesky_decompose.md).
> Notably, the Pi turned out to be running a 32-bit armhf userland on a 64-bit kernel
> (not aarch64 as originally assumed — see that section's notes), and the ESP32-S3 run
> needed a new self-contained example at `examples/esp32_cholesky_test/` (same pattern
> as `examples/esp32_ntt_test/`) since the on-device harness has no cholesky coverage.
> This file is kept as a record of the workflow used; the instructions below still
> apply if this ever needs to be re-run (e.g. after a future implementation change).

Self-contained instructions for finishing the multi-platform validation sweep for
`numx_cholesky_decompose` (merged in [#51](https://github.com/NIKX-Tech/numx/pull/51),
contributed by Erfan Esmaeili). Windows x86/x64 and Linux x86/x86-64 (WSL2) are already
done — see [`validation/results/linalg/cholesky_decompose.md`](../results/linalg/cholesky_decompose.md).
This mirrors the workflow used for the NTT module's hardware validation (commits
`068b332`, `1a50e3c`, see [`validation/results/ntt/ntt.md`](../results/ntt/ntt.md) for
the reference output format).

Give this file to whoever has physical access to the two boards (or their SSH/serial
sessions) — Claude Code or a human — as the task prompt.

## What "done" looks like

Two new sections appended to `validation/results/linalg/cholesky_decompose.md`, one per
platform, in the same format as the existing ARM64/Windows/Linux sections in that file:
header line (`## <arch> — <OS/board> / <compiler> / <precision>`), a validator/date/commit
line, a short prose note on how the build was done, a **Test cases** table, a **Performance**
table, and a `**RESULTS: N PASS / 0 FAIL / N TOTAL**` line. Do not edit or reformat the
existing sections — only append.

Also update the `## Remaining validation` section at the bottom of that file once both
platforms are done (remove the pending note, or delete the section entirely if nothing
is left).

---

## Part 1 — Raspberry Pi 4 (aarch64 / gcc)

Reference hardware profile: [`validation/hardware/raspberry_pi.md`](../hardware/raspberry_pi.md)
(Raspbian GNU/Linux 13, gcc 14.2.0, Cortex-A72, float32 by default).

1. SSH into the Pi. Fetch and check out the latest commit on `main` (or whichever branch
   has the Cholesky work — confirm with `git log --oneline -1 -- src/linalg.c` that
   `numx_cholesky_decompose` is present before proceeding).
2. Build unmodified, Release config, from the root `CMakeLists.txt` — do not edit any
   existing CMake files:
   ```
   cmake -S . -B build_rpi -DCMAKE_BUILD_TYPE=Release
   cmake --build build_rpi -j4 --target numx_tests numx_val_runner
   ```
3. Run `./build_rpi/numx_tests`, confirm 0 failures, and pull out the six
   `test_cholesky_decompose_*` lines (`3x3_success`, `residual_reconstruction`,
   `non_spd_fail`, `null_A`, `null_L`, `invalid_dim`) plus the final `N Tests 0
   Failures 0 Ignored` summary line.
4. Run `./build_rpi/numx_val_runner`, grab the `numx_cholesky_decompose` block (six
   `L[i][j]` checks against the textbook 3×3 SPD matrix, plus the `BENCH cholesky_decompose
   3×3` timing line — this is where the Performance table numbers come from). Confirm no
   `FAIL` lines in that block.
5. If float64 validation is also wanted on this platform (optional — the existing sweep
   only ran RPi at float32, matching its hardware profile default), repeat step 2 with
   `-DCMAKE_C_FLAGS="-DNUMX_USE_DOUBLE -DUNITY_INCLUDE_DOUBLE"` into a separate build
   directory (e.g. `build_rpi_f64`), and repeat steps 3–4.
6. Write the results into `validation/results/linalg/cholesky_decompose.md`, matching the
   existing table format (see the ARM64/Windows/Linux sections already in that file as the
   template). Include validator name, date, and short commit hash. Note the compiler
   version (`gcc --version`) and kernel (`uname -r`) in the header, same style as the
   RPi section in `validation/results/ntt/ntt.md`.

## Part 2 — ESP32-S3 (Xtensa LX7 / ESP-IDF)

Reference hardware profile: [`validation/hardware/esp32_devkit_v1.md`](../hardware/esp32_devkit_v1.md).

The existing on-device test harness (`tests/esp32_tests/`) does **not** cover
`numx_cholesky_decompose` — same gap the NTT module had. Do not wire it into that harness
(it would require editing `CMakeLists.txt`, `tests.h`, and `esp32_tests.c`). Instead,
follow the pattern already established at `examples/esp32_ntt_test/` and create a new,
self-contained example project at `examples/esp32_cholesky_test/`:

```
examples/esp32_cholesky_test/
├── CMakeLists.txt              ← top-level ESP-IDF project file (copy+adapt from esp32_ntt_test)
├── main/
│   ├── CMakeLists.txt          ← component registration, links src/linalg.c directly
│   ├── main.c                  ← test + bench entry point (app_main)
│   └── sdkconfig.defaults
```

1. `main.c` should mirror the 6 Unity cases in `tests/test_linalg.c`
   (`test_cholesky_decompose_3x3_success`, `_residual_reconstruction`, `_non_spd_fail`,
   `_null_A`, `_null_L`, `_invalid_dim`) as plain `printf`-based checks (no Unity
   dependency needed, same approach as `examples/esp32_ntt_test/main/main.c`), plus a
   timing loop around `numx_cholesky_decompose` using `esp_timer_get_time()`.
2. Cap benchmark iterations well below what the NTT example used (that one had to drop
   from N=10,000 to N=1,000 to dodge the task watchdog) — start at N=1,000 for the 3×3
   case and reduce further if the watchdog fires. Cholesky is O(n³) but n=3 here, so a
   few thousand iterations should be safe; adjust based on actual runtime.
3. Build and flash over USB:
   ```
   idf.py build
   idf.py -p <COM_PORT> flash monitor
   ```
   Capture the full UART output (test PASS/FAIL lines + bench total/per-call numbers)
   before any watchdog reset. If a stack-overflow guard fires after output completes
   (as it did for the NTT run — a harness artifact from deep on-stack buffer usage, not
   a library bug), that's fine as long as all test/bench output printed first.
4. Write the results into `validation/results/linalg/cholesky_decompose.md`, same table
   format as the other sections. Note ESP-IDF version, Xtensa toolchain version, and
   whether float32 or int-only arithmetic was used (Cholesky is float, unlike NTT's
   int16 Barrett-reduction path — so `NUMX_USE_DOUBLE` is N/A here unless you also want
   an ESP32 float64 run, which is unusual for this MCU and can be skipped).

## Notes carried over from the NTT precedent

- Do not edit any existing files outside of what's explicitly required (new CMake/example
  files are fine; do not touch `tests/esp32_tests/*` or the root `CMakeLists.txt`).
- If the repo checkout on either board is stale, fetch/fast-forward to the commit that has
  `numx_cholesky_decompose` before validating, and note the actual commit hash used in the
  result file header (not the one in this prompt).
- Record precision as it actually ran (float32/float64), not assumed from the hardware
  profile default.
