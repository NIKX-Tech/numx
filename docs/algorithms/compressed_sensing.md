# Compressed Sensing

**Module:** `numx/compressed_sensing.h` | **Category:** Sparse Recovery | **Phase:** P3.02

---

## What it solves

Recovers a sparse signal $x \in \mathbb{R}^n$ from a small number of linear measurements $y = Ax$ where $A \in \mathbb{R}^{m \times n}$, $m \ll n$, and $x$ has at most $k$ nonzero entries.

Two algorithms are provided:

- **OMP** (Orthogonal Matching Pursuit): greedy exact recovery, guaranteed when the sensing matrix satisfies the Restricted Isometry Property (RIP) with $\delta_{2k} < \sqrt{2} - 1$.
- **ISTA** (Iterative Shrinkage-Thresholding Algorithm): convex relaxation via $\ell_1$ minimisation, converges to the global optimum of the LASSO.

A **spectral norm** utility (`numx_cs_spectral_norm`) estimates $\sigma_{\max}(A)$ via power iteration; the ISTA step size must satisfy $\text{step} < 1/\sigma_{\max}^2$.

---

## Mathematical formulations

### Compressed sensing problem

Given measurements $y = Ax + \eta$ (where $\eta$ is optional noise) and prior knowledge that $\|x\|_0 \leq k$, find $\hat{x}$ such that $A\hat{x} \approx y$.

### OMP — Orthogonal Matching Pursuit

Greedy iterative algorithm. Maintains a residual $r$ and a support set $\mathcal{S}$:

**Initialise:** $r \leftarrow y$, $\mathcal{S} \leftarrow \emptyset$

**For** $s = 1, \ldots, k$:

1. **Correlation step** — find the column of $A$ most correlated with the residual:
$$j^* = \arg\max_{j \notin \mathcal{S}} \bigl|\langle A_j,\, r\rangle\bigr|$$

2. **Support update:** $\mathcal{S} \leftarrow \mathcal{S} \cup \{j^*\}$

3. **Least-squares projection** — solve the normal equations over the active support:
$$\hat{x}_{\mathcal{S}} = \bigl(A_{\mathcal{S}}^\top A_{\mathcal{S}}\bigr)^{-1} A_{\mathcal{S}}^\top y$$

   Implemented via incremental update of the Gram matrix $G_{ij} = A_i^\top A_j$ ($|\mathcal{S}| \times |\mathcal{S}|$ system, solved by LU with partial pivoting).

4. **Residual update:**
$$r \leftarrow y - A_{\mathcal{S}}\,\hat{x}_{\mathcal{S}}$$

**Output:** $\hat{x}$ with $\hat{x}_{\mathcal{S}} = $ solution coefficients, $\hat{x}_j = 0$ for $j \notin \mathcal{S}$.

### ISTA — Iterative Shrinkage-Thresholding Algorithm

Solves the LASSO:

$$\min_{x \in \mathbb{R}^n} \frac{1}{2}\|Ax - y\|_2^2 + \lambda\|x\|_1$$

Each iteration applies a proximal gradient step:

$$x^{(t+1)} = \mathcal{S}_{\lambda\alpha}\!\left(x^{(t)} - \alpha A^\top(Ax^{(t)} - y)\right)$$

where $\alpha$ is the step size and $\mathcal{S}_\tau$ is the **soft-threshold operator**:

$$\mathcal{S}_\tau(v)_j = \begin{cases}
v_j - \tau & \text{if } v_j > \tau \\
v_j + \tau & \text{if } v_j < -\tau \\
0 & \text{otherwise}
\end{cases}$$

Convergence is guaranteed when $\alpha \leq 1/\sigma_{\max}^2(A)$.

### Spectral norm via power iteration

Estimates $\sigma_{\max}(A)$ (largest singular value) by iterating on $A^\top A$:

$$v^{(t+1)} = \frac{A^\top A\, v^{(t)}}{\|A^\top A\, v^{(t)}\|_2}, \qquad v^{(0)} = \frac{1}{\sqrt{n}}\mathbf{1}$$

After convergence: $\sigma_{\max}(A) \approx \|A v^{(\infty)}\|_2$.

---

## Complexity

| Function | Time | Stack (default config) |
|---|---|---|
| `numx_cs_omp` | $O(k \cdot m \cdot n + k^2 \cdot m + k^3)$ | ~9 KB |
| `numx_cs_ista` | $O(\text{max\_iter} \cdot m \cdot n)$ | ~2 KB |
| `numx_cs_spectral_norm` | $O(\text{max\_iter} \cdot m \cdot n)$ | ~3 KB |

Stack scales as $O(\text{NUMX\_MAX\_CS\_SPARSITY}^2 + \text{NUMX\_MAX\_CS\_MEASUREMENTS})$ for OMP. Reduce the config limits to shrink memory for constrained targets.

---

## When to use

- **OMP**: exact recovery in a noise-free or low-noise setting; $k$ is known; the sensing matrix has low coherence (e.g. random Gaussian, ±1 Rademacher). Typically faster than ISTA for small $k$.
- **ISTA**: unknown sparsity; moderate noise; soft recovery via $\ell_1$ regularisation preferred. Works well when the problem is already set up as a convex LASSO.
- **spectral_norm**: compute once per matrix $A$ to determine a valid ISTA step size.

## When NOT to use

- `numx_cs_omp` when $k > \text{NUMX\_MAX\_CS\_SPARSITY}$ — returns `NUMX_ERR_INVALID_ARG`; increase the limit or use ISTA.
- `numx_cs_omp` when the sensing matrix is highly coherent — greedy selection may pick correlated atoms and fail to converge.
- `numx_cs_ista` with `step ≥ 1/σ²` — the iteration diverges; call `numx_cs_spectral_norm` first.
- Either algorithm when $m > \text{NUMX\_MAX\_CS\_MEASUREMENTS}$ or $n > \text{NUMX\_MAX\_CS\_SIGNAL\_DIM}` — returns `NUMX_ERR_INVALID_ARG`.

---

## References

1. Tropp, J. A. & Gilbert, A. C. — "Signal recovery from random measurements via Orthogonal Matching Pursuit", *IEEE Trans. Inf. Theory* 53(12):4655–4666, 2007.
2. Beck, A. & Teboulle, M. — "A fast iterative shrinkage-thresholding algorithm for linear inverse problems", *SIAM J. Imaging Sci.* 2(1):183–202, 2009.
3. Candès, E. J. & Wakin, M. B. — "An introduction to compressive sampling", *IEEE Signal Process. Mag.* 25(2):21–30, 2008.

---

## Example

```c
#include "numx/compressed_sensing.h"

/* ── OMP: recover a 2-sparse signal from 4 measurements ── */
/* A = I_4, y = [0, 1, 0, 1], k = 2 → x = [0, 1, 0, 1] */
numx_real_t A[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
numx_real_t y[4]  = {0.0f, 1.0f, 0.0f, 1.0f};
numx_real_t x[4];

numx_cs_omp(A, y, 4, 4, 2, x);
/* x = [0, 1, 0, 1] */

/* ── ISTA: LASSO recovery with regularisation ── */
numx_real_t sigma, step;
numx_cs_spectral_norm(A, 4, 4, 32, &sigma);  /* sigma ≈ 1.0 */
step = 0.9f / (sigma * sigma);               /* safe step size */

numx_real_t x_ista[4];
numx_cs_ista(A, y, 4, 4, 0.05f, step, 200, x_ista);
/* x_ista ≈ [0, 0.95, 0, 0.95] (soft-shrunk by lambda*step) */
```
