# Hardware Profile: Windows x64 (MSVC)

**Profile date:** 2026-06-05 (float32) / 2026-06-06 (float64)
**numx commit:** 4c4c0f0 (float32 run) / 1bba399 (float64 run)

## System

| Field        | Value                                              |
|--------------|----------------------------------------------------|
| Board/host   | Windows 11 desktop                                 |
| OS           | Windows 11 Pro (build 10.0.26200)                  |
| CPU          | — (not recorded)                                   |
| Architecture | x86-64                                             |
| Compiler     | MSVC 14.51 (VS 2026 Build Tools)                   |
| C standard   | C99 (`/std:c11`)                                   |
| Optimisation | `/O2`                                              |
| Timer        | `QueryPerformanceCounter`                          |
| Precision    | float32 (first run) and float64 / `NUMX_USE_DOUBLE` (second run) |

## Notes

- Two validation runs: float32 (default, no `NUMX_USE_DOUBLE`, commit 4c4c0f0,
  2026-06-05) and float64 (`NUMX_USE_DOUBLE` defined, commit 1bba399, 2026-06-06).
- Some Unity assertions use float32-level tolerances even in the float64 build.
  Affected tests still pass because errors are well within float32 tolerance.
  This is a test harness configuration issue, not a library bug. See individual
  result files for per-module detail.
- Benchmarks run in a Windows user-mode process; no CPU affinity or real-time
  priority. Results are indicative; expect ±10% variance between runs.
