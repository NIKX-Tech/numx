# Numerical Integration

**Module:** `numx/integrate.h` | **Category:** Foundation | **Phase:** P1.16–P1.18

---

## What it solves

Approximates the definite integral $\int_a^b f(x)\,dx$ for a black-box function $f$. Three methods are provided: the composite trapezoid rule (simple, $O(h^2)$), composite Simpson's rule (exact for cubics, $O(h^4)$), and Gauss–Legendre quadrature (exact for high-degree polynomials, fixed evaluation points).

---

## Mathematical formulations

### Composite trapezoid rule

Divides $[a, b]$ into $n$ equal subintervals of width $h = (b-a)/n$ and approximates $f$ by a piecewise linear function:

$$\int_a^b f(x)\,dx \approx \frac{h}{2}\left[f(a) + 2\sum_{i=1}^{n-1}f(a + ih) + f(b)\right]$$

Global truncation error: $-\dfrac{(b-a)}{12}h^2 f''(\xi)$ for some $\xi \in (a,b)$. Error is $O(h^2)$.

### Composite Simpson's rule

Uses piecewise quadratic (parabolic) approximation over pairs of subintervals ($n$ must be even):

$$\int_a^b f(x)\,dx \approx \frac{h}{3}\left[f(a) + 4f(x_1) + 2f(x_2) + 4f(x_3) + \cdots + f(b)\right]$$

Global truncation error: $-\dfrac{(b-a)}{180}h^4 f^{(4)}(\xi)$. Error is $O(h^4)$. **Exact for polynomials of degree $\leq 3$.**

### Gauss–Legendre quadrature

Maps $[a, b]$ to the reference interval $[-1, 1]$ via $x = \frac{a+b}{2} + \frac{b-a}{2}\xi$ and applies a weighted sum at pre-computed nodes $\xi_i$:

$$\int_a^b f(x)\,dx \approx \frac{b-a}{2}\sum_{i=1}^{n} w_i\, f\!\left(\frac{a+b}{2} + \frac{b-a}{2}\xi_i\right)$$

The $n$-point rule is **exact for polynomials of degree $\leq 2n-1$**. The implementation provides $n \in \{2, 4, 8\}$:

| Points | Exact up to degree | Nodes stored as |
|---|---|---|
| 2 | 3 | `GL2_X`, `GL2_W` |
| 4 | 7 | `GL4_X`, `GL4_W` |
| 8 | 15 | `GL8_X`, `GL8_W` |

Nodes and weights are compile-time constants (no runtime computation).

---

## Error comparison

| Method | Error order | Evaluations | Notes |
|---|---|---|---|
| Trapezoid ($n$ subintervals) | $O(h^2)$ | $n+1$ | Robust, needs $f''$ bounded |
| Simpson ($n$ subintervals) | $O(h^4)$ | $n+1$ | $n$ must be even |
| Gauss-8 (single interval) | exact to deg 15 | 8 | Best for smooth $f$ on short interval |

---

## When to use

- **Trapezoid**: tabulated data or functions with limited smoothness; straightforward to increase $n$.
- **Simpson**: smooth $f$ over a moderately-sized interval; higher accuracy per evaluation than trapezoid.
- **Gauss-8**: smooth analytic $f$ on a single interval; minimum function evaluations for high accuracy.

## When NOT to use

- Gauss quadrature on functions with singularities or discontinuities inside $[a, b]$ — accuracy degrades sharply.
- Trapezoid/Simpson on oscillatory functions with period much smaller than $h$ — aliasing causes large error.
- `numx_integrate_simpson` with odd $n$ — returns `NUMX_ERR_INVALID_ARG`.

---

## References

1. Davis, P. J. & Rabinowitz, P. — *Methods of Numerical Integration*, 2nd ed., Academic Press, 1984.
2. Abramowitz, M. & Stegun, I. A. — *Handbook of Mathematical Functions*, §25.4 (Gauss–Legendre), Dover, 1972.

---

## Example

```c
#include "numx/integrate.h"

static numx_real_t f(numx_real_t x) { return x * x; }  /* ∫₀¹ x² dx = 1/3 */

numx_real_t result;

numx_integrate_trap   (f, 0.0f, 1.0f, 1000, &result);  /* ≈ 0.3333 (O(h²))  */
numx_integrate_simpson(f, 0.0f, 1.0f,    4, &result);  /* = 0.3333 (exact)  */
numx_integrate_gauss  (f, 0.0f, 1.0f,    4, &result);  /* = 0.3333 (exact)  */
```
