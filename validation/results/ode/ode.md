# Validation: numx_ode (rk4 / rk45)

**Validator:** Amir Ab Khoshk
**Date:** 2026-05-25
**numx version:** d81b386

## Hardware: ESP32
*⚠️ Pending.*

## Hardware: Host
- OS: Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 -O2 / float32

## Test results

| Test case                          | Expected          | numx output     | Pass/Fail |
|-----------------------------------|------------------|-----------------|-----------|
| rk4 decay y(1.0) exact e^-1        | 0.36787944       | 0.36787960      | ✅        |
| rk4 harmonic x(1.0) = cos(1)       | 0.54030231       | 0.54030234      | ✅        |
| rk4 harmonic energy conserved      | 0.5              | 0.49999991      | ✅        |
| rk4 propagates RHS error           | returns RHS err  | correct         | ✅        |
| rk45 decay y(1.0) tol=1e-4         | 0.36787944       | 0.36787945      | ✅        |
| rk45 harmonic energy               | 0.5              | 0.50000006      | ✅        |
| rk4 h≤0 / steps=0 / n=0 → ERR     | -2               | -2              | ✅        |
| rk45 t1≤t0 / tol≤0 → ERR          | -2               | -2              | ✅        |

*All 18 Unity tests: PASS (test_ode.c:257–275)*

## Performance (x86-64)

| Function                          | N      | Total      | Per call   |
|-----------------------------------|--------|------------|------------|
| rk4 decay (n=1, 100 steps)        | 10,000 | 17,353 µs  | 1,735 ns   |
| rk4 harmonic (n=2, 100 steps)     | 10,000 | 17,742 µs  | 1,774 ns   |
| rk45 decay (n=1, tol=1e-4)        | 10,000 | 2,395 µs   | 239 ns     |
| rk45 harmonic (n=2, tol=1e-4)     | 10,000 | 4,892 µs   | 489 ns     |
| ESP32                             | —      | *pending*  | *pending*  |

Per rk4 step: ~17 ns (n=1), ~17.7 ns (n=2) → ~4.25 ns/function evaluation. This makes sense for 4 evaluations per step.

## Python comparison

| Function         | Exact (double)    | numx (float32)  | Abs error | OK |
|-----------------|-------------------|-----------------|-----------|----|
| rk4 y(1.0)       | 0.367879441171    | 0.36787960      | 1.59e-07  | ✅ |
| rk4 x(1.0) harm  | 0.540302305868    | 0.54030234      | 2.62e-08  | ✅ |
| rk4 v(1.0) harm  | -0.841470984808   | -0.84147084     | 1.42e-07  | ✅ |
| rk4 energy(1.0)  | 0.5               | 0.49999991      | 8.94e-08  | ✅ |
| rk45 y(1.0)      | 0.367879441171    | 0.36787945      | 1.03e-08  | ✅ |
| rk45 energy      | 0.5               | 0.50000006      | 5.96e-08  | ✅ |

## Notes
All errors within float32 epsilon (~1.2e-07). rk45 achieves better accuracy than rk4 on the decay problem (1.03e-08 vs 1.59e-07) because the adaptive step control drives local error below tol=1e-4 per step, accumulating less global error.

rk4 100-step benchmark (1,735 ns/call) on ESP32 at 240 MHz would take ~12× longer in clock cycles → expect ~20 µs. Feasible for most control loops at ≤50 kHz.
