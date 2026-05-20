# Randomized Sketch (Randomized SVD)

**Module:** `numx/sketch.h` | **Category:** Linear Algebra | **Phase:** P3.03

---

## What it solves

Computes a rank-$r$ approximation to an $m \times n$ matrix $A$:

$$A \approx U\,\Sigma\,V^\top$$

where $U \in \mathbb{R}^{m \times r}$, $\Sigma = \mathrm{diag}(\sigma_1, \ldots, \sigma_r)$ with $\sigma_1 \geq \cdots \geq \sigma_r \geq 0$, and $V^\top \in \mathbb{R}^{r \times n}$.

The algorithm (Halko, Martinsson, Tropp 2011) requires only two passes over $A$ and runs in $O(mn(r + p) + (r+p)^2 n)$ time, where $p$ is the oversampling parameter. For $r \ll \min(m, n)$ this is far cheaper than the exact $O(mn^2)$ SVD.

---

## Mathematical formulations

### Stage A — Randomized range finder

Draw a random Gaussian test matrix $\Omega \in \mathbb{R}^{n \times (r+p)}$ with i.i.d. $\mathcal{N}(0, 1)$ entries and form the sketch:

$$Y = A\,\Omega \in \mathbb{R}^{m \times (r+p)}$$

Orthonormalise the columns of $Y$ via Modified Gram-Schmidt to obtain $Q \in \mathbb{R}^{m \times (r+p)}$ with $Q^\top Q = I$:

$$Y = Q R$$

The columns of $Q$ span a near-optimal approximation of the range of $A$ (with failure probability $\leq \delta$ that decreases exponentially with $p$).

### Stage B — Projected SVD

Project $A$ onto the sketch basis:

$$B = Q^\top A \in \mathbb{R}^{(r+p) \times n}$$

Compute the Gram matrix $G = B B^\top \in \mathbb{R}^{(r+p) \times (r+p)}$ and find its eigendecomposition via Jacobi iteration:

$$G = \hat{U}\,\Lambda\,\hat{U}^\top, \qquad \Lambda = \mathrm{diag}(\lambda_1, \ldots, \lambda_{r+p})$$

Eigenvalues satisfy $\lambda_i = \sigma_i^2(B) \approx \sigma_i^2(A)$. The singular triplets of $A$ are:

$$\sigma_i = \sqrt{\lambda_i}, \qquad U_i = Q\hat{u}_i, \qquad V_i = \frac{B^\top\hat{u}_i}{\sigma_i}$$

Output: first $r$ triplets sorted by $\sigma$ descending.

### Jacobi eigendecomposition

For a symmetric $k \times k$ matrix $G$ ($k = r + p \leq 16$ by default), classical Jacobi sweeps are applied. Each rotation zeroes the $(p, q)$ and $(q, p)$ off-diagonal entries via:

$$\theta = \frac{G_{qq} - G_{pp}}{2\,G_{pq}}, \quad t = \frac{\operatorname{sgn}(\theta)}{|\theta| + \sqrt{1 + \theta^2}}, \quad c = \frac{1}{\sqrt{1+t^2}}, \quad s = ct$$

The update uses the numerically stable $\tau = s/(1+c)$ form:

$$G_{pr} \leftarrow G_{pr} - s(G_{qr} + \tau G_{pr}), \qquad G_{qr} \leftarrow G_{qr} + s(G_{pr} - \tau G_{qr})$$

Convergence: 20 sweeps is sufficient for all practical $k \leq 16$.

### Random number generation

The test matrix $\Omega$ is generated with a seeded xorshift32 PRNG and Box-Muller transform:

$$z = \sqrt{-2\ln u_1}\;\cos(2\pi u_2), \qquad u_1, u_2 \sim U(0, 1)$$

A fixed seed gives deterministic, reproducible behaviour across calls.

---

## Approximation quality

For an $m \times n$ matrix $A$ with singular values $\sigma_1 \geq \sigma_2 \geq \cdots$, the expected error of the rank-$r$ approximation with oversampling $p \geq 2$ satisfies:

$$\mathbb{E}\!\left[\|A - U\Sigma V^\top\|_F\right] \leq \left(1 + \frac{r}{p-1}\right)^{1/2}\!\left(\sum_{j > r} \sigma_j^2\right)^{1/2}$$

The bound matches the optimal Eckart-Young rank-$r$ approximation up to a small constant that decreases rapidly with oversampling $p$.

---

## Complexity

| Operation | Cost |
|---|---|
| Draw Ω and form Y = AΩ | $O(mn(r+p))$ |
| MGS orthogonalisation | $O(m(r+p)^2)$ |
| Project B = Q⊤A | $O(mn(r+p))$ |
| Gram matrix G = BB⊤ | $O(n(r+p)^2)$ |
| Jacobi EVD of G | $O((r+p)^3)$ per sweep × 20 sweeps |
| Recover U, Vt | $O(m(r+p)r + n(r+p)r)$ |
| **Total** | $O(mn(r+p))$ dominant |

Stack usage: ~14 KB with default config (M=N=64, RANK=16). Reduce `NUMX_MAX_SKETCH_M`, `NUMX_MAX_SKETCH_N`, `NUMX_MAX_SKETCH_RANK` for tighter targets.

---

## When to use

- Low-rank approximation of large matrices (covariance, kernel, adjacency) where $r \ll \min(m, n)$.
- PCA / dimensionality reduction on streaming or batch sensor data.
- Preconditioning or data compression pipelines on MCUs with enough SRAM for the static buffers.
- When the exact SVD cost $O(mn^2)$ is prohibitive and a controlled approximation is acceptable.

## When NOT to use

- When $r + p > \text{NUMX\_MAX\_SKETCH\_RANK}$ — returns `NUMX_ERR_INVALID_ARG`; increase the limit.
- When $m > \text{NUMX\_MAX\_SKETCH\_M}$ or $n > \text{NUMX\_MAX\_SKETCH\_N}` — same error; increase limits.
- When the matrix has no clear low-rank structure — the approximation error will be large regardless of $r$.
- When exact singular values are required — use the exact SVD (not in numx Phase 3).

---

## References

1. Halko, N., Martinsson, P.-G. & Tropp, J. A. — "Finding structure with randomness: Probabilistic algorithms for constructing approximate matrix decompositions", *SIAM Review* 53(2):217–288, 2011.
2. Martinsson, P.-G. & Tropp, J. A. — "Randomized numerical linear algebra: Foundations and algorithms", *Acta Numerica* 29:403–572, 2020.
3. Golub, G. H. & Van Loan, C. F. — *Matrix Computations*, 4th ed., §8.5, Johns Hopkins University Press, 2013.

---

## Example

```c
#include "numx/sketch.h"

/* Rank-2 approximation of a 4×4 matrix with seed 42 */
numx_real_t A[16] = {
    3.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 2.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f
};
numx_real_t U[8];  /* 4 × 2, row-major */
numx_real_t S[2];  /* 2 singular values */
numx_real_t Vt[8]; /* 2 × 4, row-major */

numx_sketch_rsvd(A, 4, 4, 2, 2, 42u, U, S, Vt);
/* S ≈ [3.0, 2.0] (descending) */
/* A ≈ U * diag(S) * Vt  (exact for rank-2 A with k_ext=4=min(m,n)) */
```
