# Linear Algebra

**Module:** `numx/linalg.h` | **Category:** Foundation | **Phase:** P1.01–P1.08

---

## What it solves

Dense linear algebra on small, fixed-size matrices and vectors: dot products, norms, cross products, matrix multiply, transpose, determinant, and a direct linear solver via LU decomposition with partial pivoting. All storage is caller-supplied; no heap allocation occurs.

---

## Mathematical formulations

### Dot product

$$\mathbf{a} \cdot \mathbf{b} = \sum_{i=0}^{n-1} a_i \, b_i$$

### Vector norms

$$\|\mathbf{a}\|_1 = \sum_{i=0}^{n-1} |a_i|, \qquad \|\mathbf{a}\|_2 = \sqrt{\sum_{i=0}^{n-1} a_i^2}, \qquad \|\mathbf{a}\|_\infty = \max_i |a_i|$$

The $\ell_2$ norm uses an internal Newton–Raphson square root (no `<math.h>`).

### Cross product (ℝ³ only)

$$(\mathbf{a} \times \mathbf{b})_0 = a_1 b_2 - a_2 b_1, \quad (\mathbf{a} \times \mathbf{b})_1 = a_2 b_0 - a_0 b_2, \quad (\mathbf{a} \times \mathbf{b})_2 = a_0 b_1 - a_1 b_0$$

### LU decomposition — Doolittle with partial pivoting

Factorises $A \in \mathbb{R}^{n \times n}$ into $PA = LU$ where $P$ is a permutation matrix, $L$ is unit lower-triangular, and $U$ is upper-triangular. $L$ and $U$ are stored in the same array: the strict lower triangle holds $L$, the upper triangle (including diagonal) holds $U$.

At step $k$, the pivot row $p$ satisfies:

$$p = \arg\max_{i \geq k} |a_{ik}^{(k)}|$$

Pivot selection maximises numerical stability by reducing growth in the factors.

### Linear solve via LU

Given $PA = LU$, solving $Ax = b$ decomposes into two triangular systems:

$$L\mathbf{y} = P\mathbf{b} \quad (\text{forward substitution}), \qquad U\mathbf{x} = \mathbf{y} \quad (\text{backward substitution})$$

Each triangular solve is $O(n^2)$.

### Determinant

$$\det(A) = (-1)^s \prod_{i=0}^{n-1} u_{ii}$$

where $s$ is the number of transpositions in the permutation $P$, computed via cycle decomposition. For $n \leq 3$, direct closed-form formulas are used instead of LU.

---

## Complexity

| Function | Time | Stack (float32, max defaults) |
|---|---|---|
| `numx_vec_dot` / `numx_vec_norm` | $O(n)$ | negligible |
| `numx_vec_cross3` | $O(1)$ | negligible |
| `numx_mat_mul` | $O(r \cdot c \cdot k)$ | negligible |
| `numx_mat_transpose` | $O(r \cdot c)$ | negligible |
| `numx_mat_det` ($n > 3$) | $O(n^3)$ | ~4 KB (LU buffer 32×32×4 B) |
| `numx_lu_decompose` | $O(n^3)$ | ~4 KB |
| `numx_lu_solve` | $O(n^2)$ | ~128 B |

---

## Error and precision

For float32 with $n$ terms, accumulated dot-product error is approximately $n \varepsilon_{\text{mach}}$ where $\varepsilon_{\text{mach}} \approx 1.2 \times 10^{-7}$.

LU solve error grows with the condition number $\kappa(A)$. Rule of thumb: you lose $\log_{10}(\kappa)$ digits. Check $\det A \neq 0$ before calling `numx_lu_solve`.

The internal `priv_sqrt` (Newton–Raphson, 64 iterations) achieves $\leq 2$ ULP error for all normal float32 inputs.

---

## When to use

- `numx_vec_dot` / `numx_vec_norm`: sensor fusion, filter state magnitude checks, quaternion normalisation.
- `numx_lu_decompose` + `numx_lu_solve`: Kalman filter update step, least-squares on small systems ($n \leq 16$).
- `numx_mat_mul`: rotation composition, covariance propagation.
- `numx_mat_det`: pre-check matrix conditioning.

## When NOT to use

- $n > 16$ on Cortex-M0 without FPU — reduce `NUMX_MAX_MAT_ROWS` in `numx_config.h`.
- Near-singular systems — the solver returns `NUMX_ERR_SINGULAR` but residual may still be large.

---

## References

1. Golub, G. H. & Van Loan, C. F. — *Matrix Computations*, 4th ed., Johns Hopkins University Press, 2013.
2. Trefethen, L. N. & Bau, D. — *Numerical Linear Algebra*, SIAM, 1997.

---

## Example

```c
#include "numx/linalg.h"

/* Solve 3×3 system A·x = b */
numx_real_t A[] = { 2.0f,  1.0f, -1.0f,
                   -3.0f, -1.0f,  2.0f,
                   -2.0f,  1.0f,  2.0f };
numx_real_t b[]    = { 8.0f, -11.0f, -3.0f };
numx_real_t LU[9], x[3];
numx_idx_t  pivot[3];

numx_lu_decompose(A, 3, LU, pivot);
numx_lu_solve(LU, pivot, 3, b, x);
/* x ≈ [2, 3, -1] */
```
