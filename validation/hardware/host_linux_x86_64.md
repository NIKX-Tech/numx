# Hardware Profile: Host (x86-64 Linux)

**Profile date:** 2026-05-25
**numx commit:** d81b386

## System

| Field       | Value                                      |
|-------------|--------------------------------------------|
| OS          | Ubuntu 22.04 LTS (kernel 6.8.0-106-generic) |
| CPU         | 13th Gen Intel Core i7-13700H              |
| Architecture| x86-64                                     |
| Compiler    | gcc (Ubuntu 11.4.0-1ubuntu1~22.04.3)       |
| C standard  | C99 (`-std=c99`)                           |
| Optimisation| `-O2`                                      |
| POSIX flag  | `-D_POSIX_C_SOURCE=199309L`                |
| Timer       | `clock_gettime(CLOCK_MONOTONIC)` — µs resolution |

## Notes

- Single-precision (`float`) — `NUMX_USE_DOUBLE` not defined.
- All default config limits from `numx_config.h` (no overrides).
- Benchmarks run in userspace; no CPU pinning or real-time priority.
  Results are indicative; expect ±10% variance between runs.
- ESP32 results must be collected separately on physical hardware.
