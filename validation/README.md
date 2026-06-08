# numx Validation — Phase 1

**numx commit:** d81b386 (x86 / ESP32-S3) / 0248577 (Apple Silicon)
**Validators:** Amir Ab Khoshk (x86, ESP32-S3, Windows), Erfan Jazeb Nikoo (Apple Silicon)
**Validation start:** 2026-05-25
**Status:** x86 complete ✅ | Apple Silicon complete ✅ | ESP32-S3 complete ✅ | Windows complete ✅

---

## Directory layout

```
validation/
├── README.md                  ← this file
├── hardware/
│   ├── host_linux_x86_64.md   ← x86 host profile
│   ├── mac_mini_m4_pro.md     ← Apple Silicon profile (macOS 26.2 / arm64)
│   ├── windows_msvc.md        ← Windows x64 profile (MSVC 14.51 / VS 2026 Build Tools)
│   └── esp32_devkit_v1.md     ← ESP32-S3 profile (details pending; see note in file)
├── results/
│   ├── linalg/
│   │   ├── vec_dot.md
│   │   ├── vec_norm.md
│   │   ├── vec_cross3.md
│   │   ├── mat_mul.md
│   │   ├── mat_transpose.md
│   │   ├── mat_det.md
│   │   └── lu_solve.md
│   ├── stats/
│   │   └── stats.md           (mean, variance, median, percentile)
│   ├── roots/
│   │   └── roots.md           (bisect, newton, brent)
│   ├── integrate/
│   │   └── integrate.md       (trap, simpson, gauss)
│   ├── differentiate/
│   │   └── differentiate.md   (forward, central, richardson)
│   ├── interpolate/
│   │   └── interpolate.md     (linear, spline_*, chebyshev)
│   ├── poly/
│   │   └── poly.md            (poly_eval, poly_roots)
│   └── ode/
│       └── ode.md             (rk4, rk45)
└── python/
    └── reference_phase1.py    ← numpy reference script
```

---

## Phase 1 function inventory (30 functions)

| ID    | Function                          | Module       | Tests | x86 bench | x86 prec | ARM64 | ESP32 |
|-------|-----------------------------------|--------------|-------|-----------|----------|-------|-------|
| P1.01 | numx_vec_dot                      | linalg       | ✅    | ✅        | ✅       | ✅    | ✅    |
| P1.02 | numx_vec_norm (L1/L2/Linf)        | linalg       | ✅    | ✅        | ✅       | ✅    | ✅    |
| P1.03 | numx_vec_cross3                   | linalg       | ✅    | ✅        | ✅       | ✅    | ✅    |
| P1.04 | numx_mat_mul                      | linalg       | ✅    | ✅        | ✅       | ✅    | ✅    |
| P1.05 | numx_mat_transpose                | linalg       | ✅    | ✅        | ✅       | ✅    | ✅    |
| P1.06 | numx_mat_transpose_sq             | linalg       | ✅    | ✅        | ✅       | ✅    | ✅    |
| P1.07 | numx_mat_det                      | linalg       | ✅    | ✅        | ✅       | ✅    | ✅    |
| P1.08 | numx_lu_decompose                 | linalg       | ✅    | ✅        | ✅       | ✅    | ✅    |
| P1.09 | numx_lu_solve                     | linalg       | ✅    | ✅        | ✅       | ✅    | ✅    |
| P1.10 | numx_stats_mean                   | stats        | ✅    | ✅        | ✅       | ✅    | ✅    |
| P1.11 | numx_stats_variance               | stats        | ✅    | ✅        | ✅       | ✅    | ✅    |
| P1.12 | numx_stats_median                 | stats        | ✅    | ✅        | ✅       | ✅    | ✅    |
| P1.13 | numx_stats_percentile             | stats        | ✅    | ✅        | ✅       | ✅    | ✅    |
| P1.14 | numx_root_bisect                  | roots        | ✅    | ✅        | ✅       | ✅    | ✅    |
| P1.15 | numx_root_newton                  | roots        | ✅    | ✅        | ✅       | ✅    | ✅    |
| P1.16 | numx_root_brent                   | roots        | ✅    | ✅        | ✅       | ✅    | ✅    |
| P1.17 | numx_integrate_trap               | integrate    | ✅    | ✅        | ✅       | ✅    | ✅    |
| P1.18 | numx_integrate_simpson            | integrate    | ✅    | ✅        | ✅       | ✅    | ✅    |
| P1.19 | numx_integrate_gauss              | integrate    | ✅    | ✅        | ✅       | ✅    | ✅    |
| P1.20 | numx_diff_forward                 | differentiate| ✅    | ✅        | ⚠️ FLAG  | ✅    | ✅    |
| P1.21 | numx_diff_central                 | differentiate| ✅    | ✅        | ⚠️ FLAG  | ✅    | ✅    |
| P1.22 | numx_diff_richardson              | differentiate| ✅    | ✅        | ⚠️ FLAG  | ✅    | ✅    |
| P1.23 | numx_interp_linear                | interpolate  | ✅    | ✅        | ✅       | ✅    | ✅    |
| P1.24 | numx_interp_spline_precompute     | interpolate  | ✅    | ✅        | ⚠️ NOTE  | ✅    | ✅    |
| P1.25 | numx_interp_spline_eval           | interpolate  | ✅    | ✅        | ⚠️ NOTE  | ✅    | ✅    |
| P1.26 | numx_interp_chebyshev             | interpolate  | ✅    | ✅        | ✅       | ✅    | ✅    |
| P1.27 | numx_poly_eval                    | poly         | ✅    | ✅        | ✅       | ✅    | ✅    |
| P1.28 | numx_poly_roots                   | poly         | ✅    | ✅        | ✅       | ✅    | ✅    |
| P1.29 | numx_ode_rk4                      | ode          | ✅    | ✅        | ✅       | ✅    | ✅    |
| P1.30 | numx_ode_rk45                     | ode          | ✅    | ✅        | ✅       | ✅    | ✅    |

---

## Unity test summary

| Platform                           | Tests | Failures | Date       |
|------------------------------------|-------|----------|------------|
| x86-64 (Ubuntu 22.04 / gcc)        | 300   | 0        | 2026-05-25 |
| ARM64 (macOS 26.2 / Apple clang)   | 300   | 0        | 2026-05-29 |
| Windows x64 (float32 / MSVC 14.51) | 295   | 0        | 2026-06-05 |
| Windows x64 (float64 / MSVC 14.51) | 294   | 0        | 2026-06-06 |
| ESP32-S3 (ESP-IDF 5.5.2 / LX7)    | 550   | 2        | 2026-05-29 |

> **float64 rows** used the `NUMX_USE_DOUBLE` build flag. Some Unity assertions
> retain float32-level tolerances; affected tests still pass. See individual
> result files for per-module detail.
>
> **ESP32-S3 2 failures** are in `numx_sketch_rsvd` (FLAG S-01): `rand()` seed portability
> across libc implementations causes a degenerate random projection in one rank-2 test case.
> The algorithm is correct — rank-1 and seed=0 cases pass with 0.00 error. See
> `validation/results/sketch/sketch.md` for full analysis.

---

## Flags requiring review

### ⚠️ F-01 — diff_forward / diff_central / diff_richardson: precision > 1e-5 vs reference

- **Symptom:** All three differentiation functions show errors of 1e-2 to 1e-3 at h=1e-4.
- **Root cause:** Float32 catastrophic cancellation. With ε≈1.2e-7, computing f(x+h)−f(x) near h=1e-4 loses ~3 significant digits.
- **Is it a code bug?** No — the implementations are correct. The limitation is float32 precision.
- **Maximum achievable accuracy** with float32: ~1e-4 (forward, optimal h≈3.5e-4).
- **Recommendation:** Expose this in the API doc. Add `NUMX_USE_DOUBLE` guidance. Flag for discussion with Erfan.

### ⚠️ F-02 — interp_spline_cubic: 1.79e-02 error on x² nodes

- **Symptom:** Natural cubic spline on y=x² nodes gives 2.2321 at x=1.5 instead of 2.25.
- **Root cause:** Natural spline BC forces m[0]=m[n-1]=0. True curvature of x² at endpoints is 2, not 0. Deviation is O(h²) and expected.
- **Is it a code bug?** No — scipy CubicSpline with bc_type='natural' gives identical output.
- **Recommendation:** Document that natural spline does not interpolate polynomial curvature at endpoints.

### ⚠️ F-03 — stats_median performance: 6,625 ns/call at n=128

- **Symptom:** Median is 178× slower than mean for same n.
- **Root cause:** Copy-to-stack + quickselect every call. By design (no in-place modification).
- **ESP32 estimate:** ~25–80 µs/call — may be too slow for RT loops. Flag for review.

### ⚠️ F-04 — integrate_trap n=100: error 2.50e-05 vs 1e-5 threshold

- **Symptom:** Trapezoidal rule at n=100 has inherent O(h²)=2.5e-5 truncation error.
- **Root cause:** Method accuracy, not implementation error. numx output exactly matches theoretical trapezoidal sum.
- **Not a bug.** Use simpson (n=100 gives 1.19e-07) or gauss for precision.

---

## ESP32-S3 — Phase 1 complete ✅

243/243 Unity tests passed on ESP32-S3 (ESP-IDF v5.5.2 / Xtensa LX7 / float32). Per-function timing and precision results are in each module's result file under `validation/results/`.

**Remaining / future items:**

1. Verify stack headroom for `numx_mat_det` at n>4 under real workloads (flagged in hardware profile).
2. Differentiation precision (F-01) is a float32 limitation — document in API; no fix needed on ESP32.
3. Phase 2 validation on ESP32-S3 (autodiff, fft, signal, sketch, compressed_sensing) — pending.
4. Update `validation/hardware/esp32_devkit_v1.md` with per-function timing table once Phase 2 is done.
