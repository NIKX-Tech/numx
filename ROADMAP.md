# Roadmap

This is the live plan for numx. Every item here is tracked as a real
[GitHub milestone](https://github.com/NIKX-Tech/numx/milestones) with real
issues attached, not aspirational text. Closed milestones link to the actual
pull requests that shipped them.

---

## Shipped

### [v0.1.0 - Foundation release](https://github.com/NIKX-Tech/numx/milestone/1) - 2026-06-10

Initial public release. Pure C99 scaffolding (types, config, CMake, Unity,
CI, CodeQL) plus 13 modules: `linalg`, `stats`, `roots`, `integrate`,
`differentiate`, `interpolate`, `poly`, `ode`, `signal`, `fft`, `autodiff`,
`compressed_sensing`, `sketch`. 300 tests passing across 6 hardware targets.

### [v1.0.0 - NTT and full hardware validation](https://github.com/NIKX-Tech/numx/milestone/2) - 2026-07-05

First stable release. Added the 14th module, `ntt` (negacyclic Number
Theoretic Transform, CRYSTALS-Kyber/Dilithium parameters), validated across
10 platform and toolchain combinations. 329 tests passing everywhere, all
384 cryptographic constants independently re-derived and verified.

---

## In progress

### [v1.1.0 - Community feedback and Cholesky](https://github.com/NIKX-Tech/numx/milestone/3)

Everything learned from the v1.0.0 launch feedback, turned into shipped
fixes: Cholesky decomposition (contributed by Erfan Esmaeili), a branchless
Barrett reduction closing the NTT module's one known constant-time gap
(contributed by u/robchroma on r/C_Programming, verified exhaustively
against 22,164,483 inputs before merging), function/data-section garbage
collection, Kahan compensated summation across 8 hot loops, and cross-
validating the `ntt` module bit-for-bit against PQClean's official reference
implementation (closing a self-consistency-only test gap). Remaining:
multi-platform Cholesky validation, a post-Kahan benchmark refresh, and the
release itself.

---

## Next

### [Academic paper](https://github.com/NIKX-Tech/numx/milestone/9)

Promoted from backlog: an outside academic reviewer confirmed the algorithms
and cross-platform validation methodology are publication-grade, provided
every implementation is traceable to a citable scientific source (in the
docstring, not just the docs) and independently validated, not just
self-consistency-tested, the way the NTT/PQClean cross-validation above now
does. Covers outline, target venue, a citation audit of the crypto modules,
and an AI-assisted-development disclosure section.

### [v1.2.0 - Post-quantum crypto and rng](https://github.com/NIKX-Tech/numx/milestone/4)

CRYSTALS-Kyber KEM and CRYSTALS-Dilithium signatures on top of `numx_ntt_*`,
making numx a verified, allocation-free, pure C99 post-quantum stack for
embedded systems. Includes a new `rng` module (portable PRNG plus a
caller-supplied entropy hook) that also closes a known ESP32 test
portability issue. Architecture design and `rng` can proceed now; the Kyber
and Dilithium implementation issues themselves are gated on the citation
audit and reference-validation groundwork above landing first. Also gated on
an [open invitation for independent security
review](https://github.com/NIKX-Tech/numx/issues/68) of the `ntt` module.

### [v1.3.0 - Estimation and control](https://github.com/NIKX-Tech/numx/milestone/5)

Kalman filtering (KF and EKF), a PID controller, and quaternion-based IMU
sensor fusion (Madgwick/Mahony). Driven by real sensor-fusion needs from
production ESP32 deployments.

### [v1.4.0 - Optimization module](https://github.com/NIKX-Tech/numx/milestone/6)

On-device nonlinear least squares (Levenberg-Marquardt / Gauss-Newton) built
directly on numx's own `autodiff` module, so sensor calibration doesn't
require hand-derived Jacobians.

---

## Backlog

- [Fixed-point DSP (q15/q31)](https://github.com/NIKX-Tech/numx/milestone/7) - vector ops for FPU-less targets (Cortex-M0, AVR)
- [RISC-V hardware validation](https://github.com/NIKX-Tech/numx/milestone/8) - closing the gap between the platform list and what's actually been run on real silicon
- [v2.0.0 - API evolution](https://github.com/NIKX-Tech/numx/milestone/10) - breaking changes, applied together with a migration guide (first candidate: a context parameter on `numx_func1d_t` callbacks)

---

## Contributing

Looking for somewhere to start? A handful of self-contained functions are
tagged [`good first issue`](https://github.com/NIKX-Tech/numx/issues?q=is%3Aissue+is%3Aopen+label%3A%22good+first+issue%22),
each with the full spec: function contract, test levels, and docs
requirements from [`CONTRIBUTING.md`](CONTRIBUTING.md).

## How we work

Every change lands on `dev` first; `dev` merges to `main` only at release
time. No feature is considered done until it's validated on real hardware,
not just CI, see [`validation/`](validation/) for what that looks like in
practice.
