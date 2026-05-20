# Polynomial Evaluation and Root Finding

**Module:** `numx/poly.h` | **Category:** Foundation | **Phase:** P1.25–P1.26

---

## What it solves

Evaluates a polynomial at a scalar point using Horner's scheme, and finds all real roots of a polynomial via iterative Newton–Raphson with successive synthetic deflation. Coefficients follow the **descending-order convention**: `coeffs[0]` is the leading term, `coeffs[degree]` is the constant.

$$p(x) = c_0 x^d + c_1 x^{d-1} + \cdots + c_{d-1} x + c_d$$

---

## Mathematical formulations

### Horner's scheme

Naïve evaluation of a degree-$d$ polynomial requires $d$ multiplications and $d$ additions using $O(d)$ temporary powers. Horner's scheme achieves the same with $d$ multiply-add pairs and no powers:

$$p(x) = c_0 x^d + c_1 x^{d-1} + \cdots + c_d = (\cdots((c_0\, x + c_1)\,x + c_2)\,x + \cdots)\,x + c_d$$

This is numerically optimal — it minimises the number of operations and has the smallest forward error among all evaluation schemes for a given coefficient set.

**Derivative via Horner (used internally for Newton):**

$$p'(x) = d\cdot c_0 x^{d-1} + (d-1)c_1 x^{d-2} + \cdots + c_{d-1}$$

Computed simultaneously with $p(x)$ in a single Horner pass:

$$b_0 = c_0, \quad b_i = b_{i-1}x + c_i \implies p(x) = b_d, \quad p'(x) = b_{d-1}^{(\text{inner})}$$

### Newton–Raphson root finding

Starting from initial guess $x_0$, iterate until $|p(x_k)| < \texttt{tol}$ or the step size $|x_{k+1} - x_k| < \texttt{tol}$:

$$x_{k+1} = x_k - \frac{p(x_k)}{p'(x_k)}$$

Convergence is **quadratic** near a simple root. Both $p$ and $p'$ are evaluated simultaneously via a single Horner pass — no extra cost over plain evaluation.

To increase the chance of finding a root, the solver tries 11 starting points: $\{0, 1, -1, 2, -2, 3, -3, 5, -5, 10, -10\}$.

### Synthetic deflation

After a root $r$ is found, the polynomial is reduced by one degree by polynomial long division, eliminating $(x - r)$ from the factor list:

$$p(x) = (x - r) \cdot q(x), \qquad \deg q = \deg p - 1$$

Coefficients of $q$ are computed by the synthetic division recurrence:

$$b_0 = c_0, \quad b_i = c_i + r \cdot b_{i-1} \quad (i = 1,\ldots, d-1)$$

The process repeats on $q(x)$ until the degree reaches zero or no new root is found (indicating a complex conjugate pair).

---

## Complexity

| Function | Time | Stack |
|---|---|---|
| `numx_poly_eval` | $O(d)$ | negligible |
| `numx_poly_roots` | $O(d^2 \cdot I)$ | $O(d)$ (work copy of coeffs) |

where $I$ = `NUMX_MAX_ITER` per root attempt, $d$ = degree. In practice $I \ll$ `NUMX_MAX_ITER` for well-conditioned roots.

---

## Numerical caveats

- **Deflation error accumulation**: each division introduces rounding error into the deflated polynomial. Roots of high-degree polynomials ($d > 10$) should be polished with one Newton step on the **original** polynomial after finding them.
- **Complex roots**: if all 11 starting points fail to converge, the routine stops and returns the real roots found so far. The caller can detect this by checking `nroots < degree`.
- **Repeated roots**: Newton converges linearly (not quadratically) to repeated roots.

---

## When to use

- **Horner eval**: signal processing (FIR filter evaluation as a polynomial), polynomial chaos expansions, characteristic polynomial evaluation.
- **Roots**: finding zeros of characteristic polynomials, control system pole analysis, sensor calibration curve inversion.

## When NOT to use

- High-degree polynomials ($d > 20$) without iterative polishing — deflation errors compound.
- Polynomials with very close roots — Newton may jump between them.

---

## References

1. Horner, W. G. — "A new method of solving numerical equations of all orders", *Philosophical Transactions of the Royal Society* 109:308–335, 1819.
2. Jenkins, M. A. & Traub, J. F. — "A three-stage variable-shift iteration for polynomial zeros", *SIAM Journal on Numerical Analysis* 7(4):545–566, 1970.
3. Press, W. H. et al. — *Numerical Recipes in C*, 3rd ed., §9.5–9.6, Cambridge, 2007.

---

## Example

```c
#include "numx/poly.h"

/* p(x) = x² - 5x + 6 = (x-2)(x-3) */
numx_real_t coeffs[] = { 1.0f, -5.0f, 6.0f };

numx_real_t val;
numx_poly_eval(coeffs, 2, 4.0f, &val);  /* val = 16 - 20 + 6 = 2 */

numx_real_t roots[2];
numx_size_t nroots;
numx_poly_roots(coeffs, 2, roots, &nroots, 1e-6f);
/* nroots = 2,  roots ≈ {2.0, 3.0} (order depends on Newton starting point) */
```
