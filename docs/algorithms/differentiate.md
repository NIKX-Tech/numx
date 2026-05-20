# Numerical Differentiation

**Module:** `numx/differentiate.h` | **Category:** Foundation | **Phase:** P1.19–P1.21

---

## What it solves

Approximates the first derivative $f'(x)$ of a black-box scalar function using finite difference formulas. Three methods are provided, trading truncation accuracy against function evaluation count and the need for derivative sign knowledge.

---

## Mathematical formulations

### Forward difference

Derives from the first-order Taylor expansion $f(x+h) = f(x) + h f'(x) + \tfrac{h^2}{2}f''(\xi)$:

$$f'(x) \approx \frac{f(x+h) - f(x)}{h}$$

Truncation error: $\dfrac{h}{2}f''(\xi)$ for some $\xi \in (x, x+h)$. **Order $O(h)$.**

### Central difference

Uses symmetric Taylor expansions to cancel the $O(h)$ term:

$$f'(x) \approx \frac{f(x+h) - f(x-h)}{2h}$$

Truncation error: $\dfrac{h^2}{6}f'''(\xi)$. **Order $O(h^2)$.** Two function evaluations; more accurate than forward difference at the same $h$.

### Richardson extrapolation

Applies the central difference at step $h$ and $h/2$, then combines to cancel the $O(h^2)$ truncation term:

$$D_h = \frac{f(x+h)-f(x-h)}{2h}, \qquad D_{h/2} = \frac{f(x+h/2)-f(x-h/2)}{h}$$

$$f'(x) \approx \frac{4\,D_{h/2} - D_h}{3}$$

This is a second-order Richardson extrapolation. Truncation error: $O(h^4)$. Four function evaluations. Most accurate of the three methods when truncation error dominates.

---

## Step-size selection and the accuracy cliff

All finite difference methods face a tension between truncation error (decreases with $h$) and round-off error (increases as $h \to 0$ due to catastrophic cancellation):

$$\text{total error} \approx \underbrace{\frac{h^p}{c} |f^{(p+1)}|}_{\text{truncation}} + \underbrace{\frac{2\varepsilon_{\text{mach}}}{h} |f|}_{\text{round-off}}$$

Optimal $h$ minimises the sum. For float32 ($\varepsilon_{\text{mach}} \approx 1.2 \times 10^{-7}$):

| Method | Optimal $h$ | Achievable error |
|---|---|---|
| Forward | $\sim \varepsilon^{1/2} \approx 3 \times 10^{-4}$ | $\sim 10^{-4}$ |
| Central | $\sim \varepsilon^{1/3} \approx 2.5 \times 10^{-3}$ | $\sim 10^{-5}$ |
| Richardson | $\sim \varepsilon^{1/5} \approx 0.04$ | $\sim 10^{-6}$ |

For a large $x$ value (e.g. $x = 100$) the cancellation in $f(x+h) - f(x)$ is severe — prefer Richardson or reduce $x$ to a reference frame near the origin.

---

## When to use

- **Forward**: simplest implementation, only one extra evaluation. Use when the sign of the derivative is unknown and no bracket is available.
- **Central**: default choice. Better accuracy than forward for the same $h$, no extra complexity.
- **Richardson**: when high accuracy is required and the function is smooth. Use a larger $h$ than central; Richardson self-corrects for the $O(h^2)$ term.

## When NOT to use

- Any method on non-differentiable or discontinuous functions — results are meaningless near kinks.
- Forward/central with very small $h$ on float32 — round-off dominates below optimal $h$.

---

## References

1. Richardson, L. F. — "The approximate arithmetical solution by finite differences of physical problems", *Philosophical Transactions of the Royal Society A* 210:307–357, 1911.
2. Higham, N. J. — *Accuracy and Stability of Numerical Algorithms*, 2nd ed., SIAM, 2002. (§8.1)

---

## Example

```c
#include "numx/differentiate.h"

/* f(x) = x³,  f'(x) = 3x²,  f'(2) = 12 */
static numx_real_t f(numx_real_t x) { return x * x * x; }

numx_real_t d;
numx_diff_forward   (f, 2.0f, 1e-3f, &d);  /* d ≈ 12.006  (O(h))   */
numx_diff_central   (f, 2.0f, 1e-3f, &d);  /* d ≈ 12.000  (O(h²))  */
numx_diff_richardson(f, 2.0f, 0.1f,  &d);  /* d ≈ 12.000  (O(h⁴))  */
```
