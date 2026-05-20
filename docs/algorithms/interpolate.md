# Interpolation

**Module:** `numx/interpolate.h` | **Category:** Foundation | **Phase:** P1.22–P1.24

---

## What it solves

Constructs a smooth function that passes through a given set of data points $(x_i, y_i)$, then evaluates it at any query $x$. Three methods are provided: piecewise-linear (fast, $C^0$), natural cubic spline ($C^2$, optimal smoothness), and Chebyshev barycentric interpolation (near-minimax accuracy for function approximation).

---

## Mathematical formulations

### Piecewise linear interpolation

For $x \in [x_i, x_{i+1}]$, the interpolant is:

$$p(x) = y_i + \frac{y_{i+1} - y_i}{x_{i+1} - x_i}(x - x_i)$$

Reproduction order: **exact for degree $\leq 1$**. Error: $O(h^2 f'')$ where $h = \max_i(x_{i+1}-x_i)$.

### Natural cubic spline

Constructs a piecewise cubic $S(x)$ on each interval $[x_i, x_{i+1}]$ that is:
- **Interpolating:** $S(x_i) = y_i$ for all $i$
- **$C^2$ continuous:** first and second derivatives match at every interior knot
- **Natural boundary conditions:** $S''(x_0) = S''(x_{n-1}) = 0$

On each interval, $S$ is expressed using second derivatives $m_i = S''(x_i)$ (the **moments**):

$$S(x) = y_i + a_1(x - x_i) + \frac{m_i}{2}(x-x_i)^2 + \frac{m_{i+1}-m_i}{6h_i}(x-x_i)^3$$

where $h_i = x_{i+1} - x_i$ and $a_1 = \dfrac{y_{i+1}-y_i}{h_i} - \dfrac{h_i(2m_i + m_{i+1})}{6}$.

The moments $m_1, \ldots, m_{n-2}$ satisfy the symmetric tridiagonal system:

$$h_{i-1}m_{i-1} + 2(h_{i-1}+h_i)m_i + h_i m_{i+1} = 6\left(\frac{y_{i+1}-y_i}{h_i} - \frac{y_i - y_{i-1}}{h_{i-1}}\right)$$

Solved by the **Thomas algorithm** (forward elimination + back substitution) in $O(n)$ time.

**Two-step API:** `numx_interp_spline_precompute` solves for the moments once; `numx_interp_spline_eval` evaluates cheaply for multiple query points. The one-shot wrapper `numx_interp_spline_cubic` combines both.

### Chebyshev barycentric interpolation

Samples $f$ at $n$ Chebyshev nodes of the first kind mapped from $[-1,1]$ to $[a,b]$:

$$x_i = \frac{a+b}{2} + \frac{b-a}{2}\cos\!\left(\frac{\pi(2i+1)}{2n}\right), \quad i = 0,\ldots,n-1$$

The barycentric form of the interpolating polynomial avoids the Runge phenomenon and is numerically stable:

$$p(x) = \frac{\displaystyle\sum_{i=0}^{n-1} \frac{w_i}{x - x_i} f(x_i)}{\displaystyle\sum_{i=0}^{n-1} \frac{w_i}{x - x_i}}, \qquad w_i = (-1)^i \sin\!\left(\frac{\pi(2i+1)}{2n}\right)$$

**Exact for polynomials of degree $< n$**. The maximum error for analytic $f$ decays geometrically in $n$.

The implementation evaluates $\sin$ via `priv_cos(π/2 - \text{arg})` — no `<math.h>` dependency.

---

## Complexity

| Function | Time | Stack (float32, max defaults) |
|---|---|---|
| `numx_interp_linear` | $O(\log n)$ (binary search) | negligible |
| `numx_interp_spline_precompute` | $O(n)$ (Thomas) | ~2 KB (diag + rhs buffers) |
| `numx_interp_spline_eval` | $O(\log n)$ | negligible |
| `numx_interp_chebyshev` | $O(n)$ | ~2 KB (nodes + fvals buffers) |

---

## When to use

- **Linear**: lookup tables from calibration data; $C^0$ is sufficient; minimal computation.
- **Cubic spline**: smooth reconstruction of sensor trajectories, waveform shaping; precompute once, evaluate many times.
- **Chebyshev**: approximating a known analytic function $f$ on a fixed interval (e.g. replacing `sinf` on a constrained MCU with a degree-8 approximation).

## When NOT to use

- Cubic spline on non-monotone $x$ data — requires sorted knots.
- Chebyshev outside $[a, b]$ — extrapolation is undefined; the function clips to boundary values.
- Spline on $n > $ `NUMX_MAX_INTERP_NODES` — increase the config limit (affects stack).

---

## References

1. de Boor, C. — *A Practical Guide to Splines*, Revised ed., Springer, 2001.
2. Berrut, J.-P. & Trefethen, L. N. — "Barycentric Lagrange interpolation", *SIAM Review* 46(3):501–517, 2004.
3. Higham, N. J. — "The numerical stability of barycentric Lagrange interpolation", *IMA Journal of Numerical Analysis* 24(4):547–556, 2004.

---

## Example

```c
#include "numx/interpolate.h"

/* Cubic spline through 5 calibration points — precompute once */
numx_real_t xs[] = {0.0f, 1.0f, 2.0f, 3.0f, 4.0f};
numx_real_t ys[] = {0.0f, 0.8f, 0.9f, 0.1f,-0.8f};
numx_real_t m[5];  /* moments — reuse across many queries */

numx_interp_spline_precompute(xs, ys, 5, m);

numx_real_t result;
numx_interp_spline_eval(xs, ys, m, 5, 1.5f, &result);  /* smooth evaluation */

/* Chebyshev approximation of a custom function on [0, π] */
static numx_real_t my_f(numx_real_t x) { return x * x - x; }
numx_interp_chebyshev(my_f, 8, 0.0f, 3.14159f, 1.57f, &result);
```
