# Contributors

numx exists because of the people below, in addition to the core work by
[Erfan Jazeb Nikoo](https://github.com/erfanjazebnikoo), owner and architect
of the project.

| Contributor | Contribution |
|---|---|
| [Amir Ab Khoshk](https://github.com/abkhoshk) | Hardware validation lead. Ran the full test and benchmark suite across Linux x86/x86-64, Windows x86/x64, Raspberry Pi 4, and ESP32-S3, producing the platform validation results under `validation/`. |
| [Sepand Haghighi](https://github.com/sepandhaghighi) | Early collaborator on the project's original 2021 prototype, reviewing and merging the first integration module (`src/integral/`), later kept as historical reference under `archive/legacy_integral/` and used as the algorithmic basis for Phase 1's `numx_integrate_trap` and `numx_integrate_simpson`. |
| [Erfan Esmaeili, PhD](https://github.com/erfanili) | Contributed the Cholesky decomposition implementation (`numx_cholesky_decompose`) in the `linalg` module, including its unit test suite. |
| u/robchroma (r/C_Programming) | Identified that the NTT module's Barrett-reduction canonicalization could be made branchless without relying on implementation-defined behavior, and contributed the fix used in `priv_barrett` (`src/ntt.c`), resolving the module's one known constant-time gap. |

---

Want to contribute? See [`CONTRIBUTING.md`](CONTRIBUTING.md) for the project's
conventions, then open a PR against `dev`.
