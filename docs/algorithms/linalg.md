# Linear Algebra

**Module:** `numx/linalg.h`
**Category:** Foundation
**Implemented:** P1.01 – P1.08

---

## Functions

| Function | Complexity | Description |
|---|---|---|
| `numx_vec_dot` | O(n) | Dot product of two vectors |
| `numx_vec_norm` | O(n) | L1, L2, or L-inf norm |
| `numx_vec_cross3` | O(1) | 3-D cross product |
| `numx_mat_mul` | O(r·c·k) | Matrix multiplication |
| `numx_mat_transpose` | O(r·c) | Out-of-place transpose |
| `numx_mat_transpose_sq` | O(n²) | In-place square transpose |
| `numx_mat_det` | O(n³) | Matrix determinant |
| `numx_lu_decompose` | O(n³) | LU decomposition (partial pivoting) |
| `numx_lu_solve` | O(n²) | Solve A·x = b via LU |

---

## Mathematical background

### Dot product
```
a · b = Σᵢ aᵢ · bᵢ
```

### Vector norms
```
‖a‖₁   = Σᵢ |aᵢ|
‖a‖₂   = √(Σᵢ aᵢ²)       — Newton-Raphson sqrt, no <math.h>
‖a‖∞   = maxᵢ |aᵢ|
```

### Cross product (3-D only)
```
(a × b)₀ = a₁·b₂ − a₂·b₁
(a × b)₁ = a₂·b₀ − a₀·b₂
(a × b)₂ = a₀·b₁ − a₁·b₀
```

### LU decomposition (Doolittle, partial pivoting)
Factors A into P·A = L·U where P is a permutation matrix, L is unit
lower triangular, and U is upper triangular. The factorisation is stored
compactly: L occupies the strictly lower triangle, U the upper triangle
including the diagonal.

Partial pivoting selects the largest absolute value in each pivot column
to maximise numerical stability. The permutation is stored as an index
array `pivot[i]` = original row now occupying position i.

Solving A·x = b after factorisation:
```
Forward:   L · y = P · b   — O(n²)
Backward:  U · x = y       — O(n²)
```

### Determinant
- n = 1, 2: direct formula
- n = 3: Sarrus rule (cofactor expansion)
- n = 4: cofactor expansion along first row using `priv_det3`
- n > 4: product of LU diagonal, sign from permutation parity via cycle decomposition

---

## When to use

- `numx_vec_dot` / `numx_vec_norm`: sensor fusion, filter state magnitude checks
- `numx_lu_decompose` + `numx_lu_solve`: solve small linear systems, Kalman filter update step
- `numx_mat_mul`: rotation composition, covariance propagation
- `numx_mat_det`: check matrix conditioning before inversion

## When NOT to use

- n > 16 on Cortex-M0 without FPU — consider reducing NUMX_MAX_MAT_ROWS
- Near-singular systems — check that det ≠ 0 before calling `numx_lu_solve`
- For float32 and n ≥ 8, determinant may lose significance; use `NUMX_USE_DOUBLE`

---

## Precision notes

| Operation | float32 error bound | float64 error bound |
|---|---|---|
| vec_dot (n=256) | ~n·ε ≈ 3×10⁻⁵ | ~n·ε ≈ 5.7×10⁻¹⁴ |
| lu_solve (n=32) | grows with condition number | same |
| priv_sqrt | ≤ 2 ULP | ≤ 2 ULP |

---

## Stack usage

| Function | Stack (float32, defaults) |
|---|---|
| `numx_mat_det` (n > 4) | ~4 KB (LU buffer 32×32×4 B) |
| `numx_lu_solve` | ~128 B (y buffer 32×4 B) |

Reduce `NUMX_MAX_MAT_ROWS` in `numx_config.h` if stack is constrained.

---

## References

- Golub, G.H. & Van Loan, C.F. — *Matrix Computations*, 4th ed., Johns Hopkins, 2013.
- Trefethen, L.N. & Bau, D. — *Numerical Linear Algebra*, SIAM, 1997.

---

## Example

```c
#include "numx/linalg.h"

/* Solve the 3x3 system A*x = b */
numx_real_t A[] = { 2.0f, 1.0f,-1.0f,
                   -3.0f,-1.0f, 2.0f,
                   -2.0f, 1.0f, 2.0f};
numx_real_t b[] = { 8.0f,-11.0f,-3.0f};
numx_real_t LU[9], x[3];
numx_idx_t  pivot[3];

numx_status_t s = numx_lu_decompose(A, 3, LU, pivot);
if (s != NUMX_OK) { /* handle */ }

s = numx_lu_solve(LU, pivot, 3, b, x);
if (s != NUMX_OK) { /* handle */ }
/* x = [2, 3, -1] */
```
