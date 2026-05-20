# Root Finding

**Module:** `numx/roots.h` | **Category:** Foundation | **Phase:** P1.13–P1.15

---

## What it solves

Finds a real scalar $x^*$ such that $f(x^*) = 0$ for a continuous function $f : \mathbb{R} \to \mathbb{R}$. Three methods are provided with increasing sophistication: bisection (safe, slow), Newton–Raphson (fast, needs derivative), and Brent–Dekker (fast, no derivative, provably convergent on a bracket).

---

## Mathematical formulations

### Bisection

Given a bracket $[a, b]$ with $f(a) \cdot f(b) < 0$ (opposite signs), the intermediate value theorem guarantees a root. Each iteration halves the interval:

$$m_k = \frac{a_k + b_k}{2}, \qquad \begin{cases} b_{k+1} = m_k & \text{if } f(a_k)\,f(m_k) < 0 \\ a_{k+1} = m_k & \text{otherwise} \end{cases}$$

After $k$ steps the bracket width is $(b - a)/2^k$. Convergence is linear with rate $\tfrac{1}{2}$.

### Newton–Raphson

Starting from an initial guess $x_0$, each iteration computes the zero of the tangent line at the current point:

$$x_{k+1} = x_k - \frac{f(x_k)}{f'(x_k)}$$

Convergence near a simple root is **quadratic**: $|e_{k+1}| \leq C |e_k|^2$. Diverges if $f'(x_k) \approx 0$ (returned as `NUMX_ERR_SINGULAR`) or if the initial guess is far from a root.

### Brent–Dekker

Combines bisection, secant, and inverse quadratic interpolation (IQI). It maintains a bracket $[a, b]$ (always converges like bisection) but uses IQI or secant steps when they fall inside the bracket and produce faster progress:

$$x_{\text{IQI}} = \frac{a \, f(b) f(c)}{(f(a)-f(b))(f(a)-f(c))} + \frac{b \, f(a) f(c)}{(f(b)-f(a))(f(b)-f(c))} + \frac{c \, f(a) f(b)}{(f(c)-f(a))(f(c)-f(b))}$$

Falls back to bisection whenever the interpolation step is rejected by Brent's conditions (step too large, outside bracket, or bracket too small). Superlinear convergence in practice, linear guaranteed.

---

## Convergence comparison

| Method | Order | Derivative needed | Bracket needed | Guaranteed |
|---|---|---|---|---|
| Bisection | 1 (linear) | No | Yes | Yes |
| Newton–Raphson | 2 (quadratic) | Yes | No | No |
| Brent–Dekker | superlinear | No | Yes | Yes |

---

## Error and stopping criteria

All three solvers stop when any of:
- $|f(x)| < \varepsilon_{\text{mach}}$ (essentially exact root), or
- bracket width $< \texttt{tol}$ (bisection, Brent), or
- step size $< \texttt{tol}$ (Newton).

Maximum iterations: `NUMX_MAX_ITER` (default 1000). Exceeding it returns `NUMX_ERR_NO_CONVERGE`.

---

## When to use

- **Bisection**: unknown function topology, need a guaranteed result, derivative unavailable.
- **Newton–Raphson**: derivative is cheap (analytic expression known), need fast convergence near root.
- **Brent–Dekker**: general-purpose; preferred when the function is smooth but the derivative is unavailable or expensive.

## When NOT to use

- Newton on functions with flat regions or multiple roots near the initial guess — oscillation or divergence can occur.
- Any method when $f$ has discontinuities inside the bracket.

---

## References

1. Brent, R. P. — *Algorithms for Minimization Without Derivatives*, Prentice-Hall, 1973. (Chapter 4)
2. Press, W. H. et al. — *Numerical Recipes in C*, 3rd ed., Cambridge University Press, 2007. (§9.4 Brent's method)

---

## Example

```c
#include "numx/roots.h"

/* f(x) = x^3 - x - 2  (root near x ≈ 1.5214) */
static numx_real_t f(numx_real_t x)  { return x*x*x - x - 2.0f; }
static numx_real_t df(numx_real_t x) { return 3.0f*x*x - 1.0f; }

numx_real_t root;

/* Brent: safe bracket, no derivative */
numx_root_brent(f, 1.0f, 2.0f, 1e-6f, &root);

/* Newton: faster if derivative is available */
numx_root_newton(f, df, 1.5f, 1e-6f, &root);
```
