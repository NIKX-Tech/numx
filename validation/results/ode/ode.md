# Validation: numx_ode (rk4 / rk45)

---

## x86-64 — Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-25 | **Commit:** d81b386

### Test cases

| Function | Problem | Expected | Computed | Pass |
|----------|---------|----------|----------|------|
| rk4 | decay y(1.0), e^-1 | 0.36787944 | 0.36787960 | ✅ |
| rk4 | harmonic x(1.0), cos(1) | 0.54030231 | 0.54030234 | ✅ |
| rk4 | harmonic energy=0.5 | 0.5 | 0.49999991 | ✅ |
| rk4 | RHS error propagates | correct | correct | ✅ |
| rk45 | decay y(1.0) tol=1e-4 | 0.36787944 | 0.36787945 | ✅ |
| rk45 | harmonic energy=0.5 | 0.5 | 0.50000006 | ✅ |
| rk4 | h≤0 / steps=0 / n=0 → ERR | -2 | -2 | ✅ |
| rk45 | t1≤t0 / tol≤0 → ERR | -2 | -2 | ✅ |

*All 18 Unity tests: PASS (test_ode.c:257–275)*

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| rk4 decay (n=1, 100 steps) | 10,000 | 17,353 µs | 1,735 ns |
| rk4 harmonic (n=2, 100 steps) | 10,000 | 17,742 µs | 1,774 ns |
| rk45 decay tol=1e-4 | 10,000 | 2,395 µs | 239 ns |
| rk45 harmonic tol=1e-4 | 10,000 | 4,892 µs | 489 ns |

### Precision vs reference (double)

| Function | exact (double) | numx (float32) | Error |
|----------|---------------|----------------|-------|
| rk4 y(1.0) decay | 0.367879441171 | 0.36787960 | 1.59e-07 |
| rk4 x(1.0) harmonic | 0.540302305868 | 0.54030234 | 2.62e-08 |
| rk4 v(1.0) harmonic | -0.841470984808 | -0.84147084 | 1.42e-07 |
| rk4 energy(1.0) | 0.5 | 0.49999991 | 8.94e-08 |
| rk45 y(1.0) decay | 0.367879441171 | 0.36787945 | 1.03e-08 |

*All errors within float32 epsilon (~1.2e-07). rk45 achieves better accuracy than rk4 via adaptive step control.*

---

## ARM64 — macOS 26.2 / Apple M4 Pro / Apple clang 21.0.0 / float32
**Validator:** Erfan Jazeb Nikoo | **Date:** 2026-05-29 | **Commit:** 37e581f

### Test cases

| Function | Problem | Expected | Computed | Error | Pass |
|----------|---------|----------|----------|-------|------|
| rk4 | decay y(1.0) → e^-1 | 0.36787945 | 0.36787960 | 1.49e-07 | ✅ |
| rk4 | harmonic x(1.0) → cos(1) | 0.54030234 | 0.54030234 | 0.00e+00 | ✅ |
| rk4 | harmonic v(1.0) → -sin(1) | -0.84147096 | -0.84147084 | 1.19e-07 | ✅ |
| rk4 | energy at t=1 | 0.5 | 0.4999999106 | 8.94e-08 | ✅ |
| rk45 | decay y(1.0) tol=1e-4 | 0.36787945 | 0.36787939 | 5.96e-08 | ✅ |
| rk45 | harmonic x(1.0) | 0.54030234 | 0.54030228 | 5.96e-08 | ✅ |
| rk45 | harmonic energy | 0.5 | 0.5000000596 | 5.96e-08 | ✅ |

*300 / 300 Unity tests PASS*

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| rk4 decay (n=1, 100 steps) | 10,000 | 26,575 µs | 2,657 ns |
| rk45 decay tol=1e-4 | 10,000 | 3,131 µs | 313 ns |

### Precision vs reference (double)

| Function | exact (double) | numx (float32) | Error |
|----------|---------------|----------------|-------|
| rk4 y(1.0) decay | 0.367879441171 | 0.36787960 | 1.49e-07 |
| rk4 x(1.0) harmonic | 0.540302305868 | 0.54030234 | 0.00e+00 |
| rk4 v(1.0) harmonic | -0.841470984808 | -0.84147084 | 1.19e-07 |
| rk4 energy | 0.5 | 0.4999999106 | 8.94e-08 |
| rk45 y(1.0) | 0.367879441171 | 0.36787939 | 5.96e-08 |

**Note:** rk4 per-call is 2,657 ns on ARM64 vs 1,735 ns on x86-64. The M4 Pro has a higher IPC but the i7-13700H operates at higher single-thread turbo frequency (~5.0 GHz) for tight sequential loops, explaining the inversion. rk45 shows no such inversion (313 ns vs 239 ns, similar ratio).

---

## ESP32-S3 — ESP-IDF v5.5.2 / Xtensa LX7 / xtensa-esp32s3-elf-gcc / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-29 | **Commit:** d81b386

### Test cases — rk4

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| rk4 growth rc | rc=0 | rc=0 | — | ✅ |
| rk4 growth y(1)=e | 2.7182818 | 2.7182817 | 1.67e-06 | ✅ |
| rk4 decay y(1)=1/e | 0.3678794 | 0.3678795 | 5.07e-07 | ✅ |
| rk4 growth y(2)=e² | 7.3890561 | 7.3890595 | 4.77e-07 | ✅ |
| rk4 harmonic energy conserved | ~0.5 | confirmed | — | ✅ |
| rk4 single step | 1.0100502 | 1.0100502 | 1.19e-07 | ✅ |
| rk4 rhs-error propagated | rc=-3 | rc=-3 | — | ✅ |
| rk4 null-f | rc=-1 | rc=-1 | — | ✅ |
| rk4 null-y0 | rc=-1 | rc=-1 | — | ✅ |
| rk4 null-yout | rc=-1 | rc=-1 | — | ✅ |
| rk4 h=0 | rc=-2 | rc=-2 | — | ✅ |
| rk4 h<0 | rc=-2 | rc=-2 | — | ✅ |
| rk4 steps=0 | rc=-2 | rc=-2 | — | ✅ |
| rk4 n=0 | rc=-2 | rc=-2 | — | ✅ |

### Test cases — rk45

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| rk45 growth rc | rc=0 | rc=0 | — | ✅ |
| rk45 growth y(1)=e | 2.7182818 | 2.7182815 | 1.43e-06 | ✅ |
| rk45 decay y(1)=1/e | 0.3678794 | 0.3678795 | 5.36e-07 | ✅ |
| rk45 harmonic energy conserved | ~0.5 | confirmed | — | ✅ |
| rk45 rhs-error propagated | rc=-3 | rc=-3 | — | ✅ |
| rk45 null-f | rc=-1 | rc=-1 | — | ✅ |
| rk45 null-y0 | rc=-1 | rc=-1 | — | ✅ |
| rk45 null-yout | rc=-1 | rc=-1 | — | ✅ |
| rk45 t1<=t0 | rc=-2 | rc=-2 | — | ✅ |
| rk45 t1=t0 | rc=-2 | rc=-2 | — | ✅ |
| rk45 tol=0 | rc=-2 | rc=-2 | — | ✅ |
| rk45 tol<0 | rc=-2 | rc=-2 | — | ✅ |
| rk45 n=0 | rc=-2 | rc=-2 | — | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| ode_rk4 decay n=1 100-steps | 1,000 | 173,201 µs | 173,201 ns |
| ode_rk4 harmonic n=2 100-steps | 1,000 | 274,568 µs | 274,568 ns |
| ode_rk45 decay n=1 tol=1e-4 | 1,000 | 39,634 µs | 39,634 ns |
| ode_rk45 harmonic n=2 tol=1e-4 | 1,000 | 63,362 µs | 63,362 ns |

### Precision vs reference

| Function | Problem | Exact | Computed | Error |
|----------|---------|-------|----------|-------|
| rk4 | y(1): y′=y, y0=1 | e=2.7182818 | 2.7182817 | 1.67e-06 |
| rk4 | y(1): y′=−y, y0=1 | 1/e=0.3678794 | 0.3678795 | 5.07e-07 |
| rk4 | y(2): y′=y, y0=1 | e²=7.3890561 | 7.3890595 | 4.77e-07 |
| rk4 | single step h=0.01 | e^0.01=1.0100502 | 1.0100502 | 1.19e-07 |
| rk45 | y(1): y′=y, y0=1 | e=2.7182818 | 2.7182815 | 1.43e-06 |
| rk45 | y(1): y′=−y, y0=1 | 1/e=0.3678794 | 0.3678795 | 5.36e-07 |

*Errors on growth problems (1.67e-06 / 1.43e-06) are ODE integration truncation error, not float32 rounding — inherent in finite step size. Single-step and decay errors are at float32 epsilon level (~1.2e-07).*

**RESULTS: 27 PASS / 0 FAIL / 27 TOTAL**

---

## Windows x64 — Windows 11 / MSVC 14.51 (VS 2026 Build Tools) / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-06-05 | **Commit:** 4c4c0f0

### Test cases

| Test | Result |
|------|--------|
| test_rk4_growth_at_t1 | ✅ |
| test_rk4_decay_at_t1 | ✅ |
| test_rk4_growth_at_t2 | ✅ |
| test_rk4_harmonic_energy_conserved | ✅ |
| test_rk4_single_step | ✅ |
| test_rk4_propagates_rhs_error | ✅ |
| test_rk4_null | ✅ |
| test_rk4_h_nonpositive | ✅ |
| test_rk4_steps_zero | ✅ |
| test_rk4_n_zero | ✅ |
| test_rk45_growth_at_t1 | ✅ |
| test_rk45_decay_at_t1 | ✅ |
| test_rk45_harmonic_energy_conserved | ✅ |
| test_rk45_propagates_rhs_error | ✅ |
| test_rk45_null | ✅ |
| test_rk45_t1_le_t0 | ✅ |
| test_rk45_tol_nonpositive | ✅ |
| test_rk45_n_zero | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| ode_rk4 decay n=1 100-steps | 1,000 | 1,745 µs | 1,745 ns |
| ode_rk4 harmonic n=2 100-steps | 1,000 | 2,141 µs | 2,141 ns |
| ode_rk45 decay n=1 tol=1e-4 | 1,000 | 339 µs | 339 ns |
| ode_rk45 harmonic n=2 tol=1e-4 | 1,000 | 416 µs | 416 ns |

**RESULTS: 18 PASS / 0 FAIL / 18 TOTAL**

---

## Windows x64 — Windows 11 / MSVC 14.51 (VS 2026 Build Tools) / float64
**Validator:** Amir Ab Khoshk | **Date:** 2026-06-06 | **Commit:** 1bba399

### Test cases

| Test | Result |
|------|--------|
| test_rk4_growth_at_t1 | ✅ |
| test_rk4_decay_at_t1 | ✅ |
| test_rk4_growth_at_t2 | ✅ |
| test_rk4_harmonic_energy_conserved | ✅ |
| test_rk4_single_step | ✅ |
| test_rk4_propagates_rhs_error | ✅ |
| test_rk4_null | ✅ |
| test_rk4_h_nonpositive | ✅ |
| test_rk4_steps_zero | ✅ |
| test_rk4_n_zero | ✅ |
| test_rk45_growth_at_t1 | ✅ |
| test_rk45_decay_at_t1 | ✅ |
| test_rk45_harmonic_energy_conserved | ✅ |
| test_rk45_propagates_rhs_error | ✅ |
| test_rk45_null | ✅ |
| test_rk45_t1_le_t0 | ✅ |
| test_rk45_tol_nonpositive | ✅ |
| test_rk45_n_zero | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| ode_rk4 decay n=1 100-steps | 1,000 | 1,813 µs | 1,813 ns |
| ode_rk4 harmonic n=2 100-steps | 1,000 | 1,827 µs | 1,827 ns |
| ode_rk45 decay n=1 tol=1e-4 | 1,000 | 347 µs | 347 ns |
| ode_rk45 harmonic n=2 tol=1e-4 | 1,000 | 450 µs | 450 ns |

**RESULTS: 18 PASS / 0 FAIL / 18 TOTAL**

---

## ARM64 — macOS 26.2 / Apple M1 Pro / Apple clang 17.0.0 / float32
**Validator:** Erfan Jazeb Nikoo | **Date:** 2026-06-08 | **Commit:** 1380ab1

### Test cases

| Function | Problem | Expected | Computed | Error | Pass |
|----------|---------|----------|----------|-------|------|
| rk4 | decay y(1.0) → e^-1 | 0.36787945 | 0.36787960 | 1.49e-07 | ✅ |
| rk4 | harmonic x(1.0) → cos(1) | 0.54030234 | 0.54030234 | 0.00e+00 | ✅ |
| rk4 | harmonic v(1.0) → -sin(1) | -0.84147096 | -0.84147084 | 1.19e-07 | ✅ |
| rk4 | energy at t=1 | 0.5 | 0.4999999106 | 8.94e-08 | ✅ |
| rk45 | decay y(1.0) tol=1e-4 | 0.36787945 | 0.36787939 | 5.96e-08 | ✅ |
| rk45 | harmonic x(1.0) | 0.54030234 | 0.54030228 | 5.96e-08 | ✅ |
| rk45 | harmonic energy | 0.5 | 0.5000000596 | 5.96e-08 | ✅ |

*300 / 300 Unity tests PASS*

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| rk4 decay (n=1, 100 steps) | 10,000 | 33,639 µs | 3,363 ns |
| rk45 decay tol=1e-4 | 10,000 | 4,703 µs | 470 ns |

### Precision vs reference (double)

| Function | exact (double) | numx (float32) | Error |
|----------|---------------|----------------|-------|
| rk4 y(1.0) decay | 0.367879441171 | 0.36787960 | 1.49e-07 |
| rk4 x(1.0) harmonic | 0.540302305868 | 0.54030234 | 0.00e+00 |
| rk4 v(1.0) harmonic | -0.841470984808 | -0.84147084 | 1.19e-07 |
| rk4 energy | 0.5 | 0.4999999106 | 8.94e-08 |
| rk45 y(1.0) | 0.367879441171 | 0.36787939 | 5.96e-08 |
