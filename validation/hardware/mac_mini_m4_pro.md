# Hardware Profile: Mac mini (Apple M4 Pro)

**Profile date:** 2026-05-29
**numx commit:** 0248577
**Validator:** Erfan Jazeb Nikoo

## System

| Field        | Value                                              |
|--------------|----------------------------------------------------|
| Machine      | Mac mini (Model: Mac16,11)                         |
| Chip         | Apple M4 Pro                                       |
| Architecture | arm64 (AArch64)                                    |
| CPU cores    | 12 (8 performance + 4 efficiency)                  |
| Memory       | 24 GB                                              |
| OS           | macOS 26.2 (Darwin 25.2.0)                         |
| Compiler     | Apple clang 21.0.0 (clang-2100.1.1.101)            |
| C standard   | C99 (`-std=c99`)                                   |
| Optimisation | `-O2`                                              |
| Precision    | float32 (`NUMX_USE_DOUBLE` not defined)            |
| CMake        | 3.31.4                                             |

## Notes

- Apple M4 Pro uses AArch64 with hardware single-precision FPU.
  This is a different ISA from the x86-64 host validation — confirms
  the library is portable across architectures.
- Apple clang (LLVM-based) vs gcc used for x86 validation — different
  optimizer; any compiler-specific UB would surface here.
- ARM64 is the closest available host ISA to embedded ARM Cortex-M
  targets; results here are more predictive of Cortex-M behaviour
  than x86-64 results.
- No CPU pinning or real-time priority. Results are indicative; expect
  ±10% variance between runs.
- Benchmarks pending — `bench_runner` is not yet implemented.
  Per-function timing will be added once `numx_bench_<module>()`
  entries are wired up.

## Test results

**300 / 300 tests PASS. 0 failures.**

```
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
./build/numx_tests
```

## Benchmark results

*Pending — `bench_runner` has no registered modules yet.*

| Function | N | Total (µs) | Per call (ns) |
|----------|---|------------|---------------|
| —        | — | *TBD*      | *TBD*         |
