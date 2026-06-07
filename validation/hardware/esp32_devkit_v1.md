# Hardware Profile: ESP32-S3 DevKit

> **Note:** The filename `esp32_devkit_v1.md` is a legacy name; the actual
> board being validated is an **ESP32-S3** (Xtensa LX7), not the classic
> ESP32 DevKit V1 (Xtensa LX6). Results collected here are for ESP32-S3.

> **Status: pending — hardware not yet flashed.**
> All benchmark values are TBD. This file will be updated with real
> measured results once `idf.py flash monitor` has been run on the
> physical device. The incomplete state is intentional, not an oversight.

**Profile date:** 2026-05-25
**numx commit:** d81b386
**Validator:** Amir Ab Khoshk

## System

| Field        | Value                                        |
|--------------|----------------------------------------------|
| Board        | ESP32-S3 DevKit (generic or official Espressif) |
| SoC          | ESP32-S3 (dual-core Xtensa LX7)              |
| CPU speed    | 240 MHz                                      |
| FPU          | ✅ Hardware single-precision FPU (LX7)       |
| Flash        | 8 MB (typical)                               |
| PSRAM        | varies by board                              |
| RAM (SRAM)   | 512 KB internal                              |
| IDF version  | ESP-IDF 5.5.4                                |
| Compiler     | xtensa-esp32s3-elf-gcc (bundled with IDF)    |
| C standard   | C99 (`-std=c99`)                             |
| Optimisation | `-O2`                                        |
| Timer        | `esp_timer_get_time()` — µs resolution       |
| Precision    | float32 (NUMX_USE_DOUBLE not defined)        |

## Notes

- ESP32-**S3** uses Xtensa **LX7** (not LX6 as original ESP32).
  LX7 has a faster FPU pipeline — single-precision float ops are faster
  than the original ESP32 for the same clock speed.
- Main task stack set to 16 KB (`CONFIG_ESP_MAIN_TASK_STACK_SIZE=16384`)
  to safely accommodate Unity + numx stack-allocated buffers
  (e.g. `numx_mat_det` allocates n*n floats on-stack for n>4).
- Benchmark N counts lower than x86: at 240 MHz the ESP32-S3 is ~6–12×
  slower per-operation than the i7-13700H at 240 MHz-equivalent throughput.
- `esp_timer_get_time()` returns int64_t microseconds since boot;
  resolution is 1 µs, so per-call times < 1 µs are measured over N
  iterations and divided.

## Actual results

*Filled after running `idf.py flash monitor` on the physical device.*

| Function            | N     | Total (µs) | Per call (ns) |
|---------------------|-------|------------|---------------|
| vec_dot n=64        | 10000 | *TBD*      | *TBD*         |
| vec_norm L2 n=64    | 10000 | *TBD*      | *TBD*         |
| …                   | …     | …          | …             |

