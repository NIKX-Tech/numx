# Ordinary Differential Equations

**Module:** `numx/ode.h` | **Category:** Foundation | **Phase:** P1.27–P1.28

---

## What it solves

Numerically integrates an initial-value problem (IVP) of the form:

$$\frac{d\mathbf{y}}{dt} = \mathbf{f}(t,\, \mathbf{y}), \qquad \mathbf{y}(t_0) = \mathbf{y}_0, \qquad \mathbf{y} \in \mathbb{R}^n$$

Two Runge–Kutta methods are provided: a classic fixed-step RK4 and an adaptive-step RKF45 (Fehlberg's embedded pair) that automatically adjusts step size to meet a user-specified tolerance.

---

## Mathematical formulations

### Classic Runge–Kutta, order 4 (RK4)

The four-stage fixed-step method advances from $t_k$ to $t_{k+1} = t_k + h$:

$$k_1 = \mathbf{f}(t_k,\, \mathbf{y}_k)$$
$$k_2 = \mathbf{f}\!\left(t_k + \tfrac{h}{2},\, \mathbf{y}_k + \tfrac{h}{2}k_1\right)$$
$$k_3 = \mathbf{f}\!\left(t_k + \tfrac{h}{2},\, \mathbf{y}_k + \tfrac{h}{2}k_2\right)$$
$$k_4 = \mathbf{f}(t_k + h,\, \mathbf{y}_k + h\,k_3)$$

$$\mathbf{y}_{k+1} = \mathbf{y}_k + \frac{h}{6}\left(k_1 + 2k_2 + 2k_3 + k_4\right)$$

Local truncation error: $O(h^5)$. Global error: $O(h^4)$. Four function evaluations per step.

### Runge–Kutta–Fehlberg, orders 4/5 (RKF45)

Uses six stages to produce two solutions of different orders simultaneously. The **4th-order solution** is used to advance the state; the **difference** between the 4th and 5th-order solutions gives a local error estimate with no extra function evaluations beyond the six stages.

The six stage slopes use Fehlberg's coefficients:

| Stage | Time | Weight vector |
|---|---|---|
| $k_1$ | $t$ | — |
| $k_2$ | $t + h/4$ | $1/4$ |
| $k_3$ | $t + 3h/8$ | $3/32,\ 9/32$ |
| $k_4$ | $t + 12h/13$ | $1932/2197,\ {-7200/2197},\ 7296/2197$ |
| $k_5$ | $t + h$ | $439/216,\ -8,\ 3680/513,\ {-845/4104}$ |
| $k_6$ | $t + h/2$ | $-8/27,\ 2,\ {-3544/2565},\ 1859/4104,\ -11/40$ |

**Error estimate** ($\ell_\infty$ norm over components):

$$e_k = h \left\|{\textstyle\frac{1}{360}k_1 - \frac{128}{4275}k_3 - \frac{2197}{75240}k_4 + \frac{1}{50}k_5 + \frac{2}{55}k_6}\right\|_\infty$$

**Step-size control** — if the step is accepted ($e_k \leq \texttt{tol}$), the next step size is:

$$h_{\text{new}} = h \cdot 0.9 \cdot \left(\frac{\texttt{tol}}{e_k}\right)^{1/5}, \qquad h_{\text{new}} \in [0.1h,\; 10h]$$

The factor $0.9$ is a safety margin; the exponent $1/5$ comes from the 4th-order global error rate $O(h^4) \to O(h^5)$ per step.

---

## Complexity and stack

| Function | Evaluations per step | Stack (float32, $n$ = `NUMX_MAX_ODE_DIM` = 16) |
|---|---|---|
| `numx_ode_rk4` | 4 | ~256 B ($k_1$–$k_4$, $y_{\text{tmp}}$) |
| `numx_ode_rk45` | 6 | ~384 B ($k_1$–$k_6$, $y_{\text{tmp}}$, $y_4$) |

Both solvers return only the **final state** $\mathbf{y}(t_1)$; intermediate points are not stored.

---

## Error and stability

- **RK4**: globally $O(h^4)$. Stable for non-stiff systems when $h \leq h_{\max}$, which depends on the eigenvalues of the Jacobian $\partial\mathbf{f}/\partial\mathbf{y}$.
- **RKF45**: adaptive step ensures the local error stays near `tol`. Not suitable for stiff systems (step size collapses to maintain stability). Returns `NUMX_ERR_NO_CONVERGE` if `NUMX_MAX_ITER` steps are exhausted without reaching $t_1$.
- For a harmonic oscillator $\mathbf{y}' = [y_1, -y_0]$, RK4 dissipates energy slightly over long integration. RKF45 matches the tolerance.

---

## When to use

- **RK4**: real-time control loops where a fixed time step is required (e.g. motor FOC, PID outer loop); step $h$ is chosen offline based on system bandwidth.
- **RKF45**: simulation, batch processing, or offline parameter estimation where accuracy matters more than fixed timing.

## When NOT to use

- Stiff systems (e.g. fast electrical dynamics coupled to slow mechanical dynamics) — use an implicit solver instead.
- Systems with $n > $ `NUMX_MAX_ODE_DIM` — increase the config limit (affects stack).
- RK4 for very long integration times without error monitoring — error accumulates as $O((t_1 - t_0) h^4)$.

---

## References

1. Fehlberg, E. — "Low-order classical Runge–Kutta formulas with step size control", *NASA Technical Report* TR R-315, 1969.
2. Hairer, E., Nørsett, S. P. & Wanner, G. — *Solving Ordinary Differential Equations I: Nonstiff Problems*, 2nd ed., Springer, 1993.
3. Press, W. H. et al. — *Numerical Recipes in C*, 3rd ed., §17.2 (RKF45), Cambridge, 2007.

---

## Example

```c
#include "numx/ode.h"

/* Harmonic oscillator: [y0, y1]' = [y1, -y0]
 * Exact: y0(t) = cos(t), y1(t) = -sin(t) */
static numx_status_t harmonic(numx_real_t t, const numx_real_t *y,
                               numx_size_t n, numx_real_t *dydt)
{
    (void)t; (void)n;
    dydt[0] =  y[1];
    dydt[1] = -y[0];
    return NUMX_OK;
}

numx_real_t y[] = { 1.0f, 0.0f };  /* y(0) = [1, 0] */

/* Fixed-step RK4: 1000 steps to t=1 */
numx_ode_rk4(harmonic, 0.0f, y, 2, 1e-3f, 1000, y);
/* y ≈ [cos(1), -sin(1)] ≈ [0.5403, -0.8415] */

/* Adaptive RKF45 to t=10, tolerance 1e-5 */
numx_real_t y2[] = { 1.0f, 0.0f };
numx_ode_rk45(harmonic, 0.0f, 10.0f, y2, 2, 1e-5f, y2);
```
