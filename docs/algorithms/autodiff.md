# Automatic Differentiation

**Module:** `numx/autodiff.h` | **Category:** Calculus | **Phase:** P3.01

---

## What it solves

Computes exact derivatives of programs written in C — not numerical approximations. Two independent modes are provided:

- **Forward-mode** (dual numbers): propagates one derivative alongside each primal computation. Cost: one forward pass, proportional to the number of inputs being differentiated simultaneously. Best when the function has few inputs and potentially many outputs.

- **Reverse-mode** (static Wengert tape): records the computation graph during the forward pass, then sweeps backward to accumulate all partial derivatives in a single pass. Cost: one forward pass + one backward pass, independent of the number of inputs. Best when the output is a single scalar (e.g. a loss function) and the number of inputs is large.

---

## Mathematical formulations

### Forward-mode: dual numbers

A **dual number** is an element of the ring $\mathbb{R}[\varepsilon]/\langle\varepsilon^2\rangle$:

$$a = a_r + a_d\varepsilon, \quad \varepsilon^2 = 0$$

Arithmetic extends naturally via the nilpotency of $\varepsilon$:

| Operation | Result |
|---|---|
| $a + b$ | $(a_r + b_r) + (a_d + b_d)\varepsilon$ |
| $a - b$ | $(a_r - b_r) + (a_d - b_d)\varepsilon$ |
| $a \cdot b$ | $a_r b_r + (a_r b_d + a_d b_r)\varepsilon$ |
| $a / b$ | $\dfrac{a_r}{b_r} + \dfrac{a_d b_r - a_r b_d}{b_r^2}\varepsilon$ |
| $-a$ | $-a_r - a_d\varepsilon$ |

Transcendental functions follow the chain rule automatically:

| Function | Value | Derivative part |
|---|---|---|
| $\sin(a)$ | $\sin(a_r)$ | $\cos(a_r)\,a_d$ |
| $\cos(a)$ | $\cos(a_r)$ | $-\sin(a_r)\,a_d$ |
| $\exp(a)$ | $e^{a_r}$ | $e^{a_r}\,a_d$ |
| $\ln(a)$ | $\ln(a_r)$ | $a_d / a_r$ |
| $\sqrt{a}$ | $\sqrt{a_r}$ | $a_d / (2\sqrt{a_r})$ |

To compute $f'(x)$ at a point $x_0$: set the input dual number to $(x_0, 1)$ (via `numx_dual_var`) and all constants to $(c, 0)$ (via `numx_dual_const`). After evaluating $f$ over dual arithmetic, the `.du` field of the result is exactly $f'(x_0)$.

**Jacobian-vector products** (JVPs) follow the same mechanism: set `.du = v_i` for each input $x_i$ to compute $\sum_i v_i\,\partial f/\partial x_i$.

### Reverse-mode: Wengert tape

A **Wengert tape** (also called a trace) records every elementary operation applied during the forward pass as a node:

$$\text{node}_i = (op_i,\; \text{arg}_0,\; \text{arg}_1,\; v_i)$$

where $v_i$ is the primal value computed at step $i$.

#### Forward pass

Variables are recorded in order of creation. Each call to `numx_ad_add`, `numx_ad_mul`, etc. appends a node and computes its primal value from its parents' primals.

#### Backward pass

Define the **adjoint** of node $i$ as:

$$\bar{v}_i = \frac{\partial L}{\partial v_i}$$

where $L = v_{\text{out}}$ is the scalar output. The adjoint is initialised as $\bar{v}_{\text{out}} = 1$ and zero for all other nodes. The backward sweep processes nodes from $i = \text{out}$ down to $i = 0$, applying the local chain rule at each step:

| Operation | Propagation |
|---|---|
| $v_i = v_a + v_b$ | $\bar{v}_a \mathrel{+}= \bar{v}_i$,  $\bar{v}_b \mathrel{+}= \bar{v}_i$ |
| $v_i = v_a - v_b$ | $\bar{v}_a \mathrel{+}= \bar{v}_i$,  $\bar{v}_b \mathrel{-}= \bar{v}_i$ |
| $v_i = v_a \cdot v_b$ | $\bar{v}_a \mathrel{+}= \bar{v}_i\, v_b$,  $\bar{v}_b \mathrel{+}= \bar{v}_i\, v_a$ |
| $v_i = v_a / v_b$ | $\bar{v}_a \mathrel{+}= \bar{v}_i / v_b$,  $\bar{v}_b \mathrel{-}= \bar{v}_i\, v_a / v_b^2$ |
| $v_i = -v_a$ | $\bar{v}_a \mathrel{-}= \bar{v}_i$ |
| $v_i = \sin(v_a)$ | $\bar{v}_a \mathrel{+}= \bar{v}_i\, \cos(v_a)$ |
| $v_i = \cos(v_a)$ | $\bar{v}_a \mathrel{-}= \bar{v}_i\, \sin(v_a)$ |
| $v_i = \exp(v_a)$ | $\bar{v}_a \mathrel{+}= \bar{v}_i\, v_i$ |
| $v_i = \ln(v_a)$ | $\bar{v}_a \mathrel{+}= \bar{v}_i / v_a$ |
| $v_i = \sqrt{v_a}$ | $\bar{v}_a \mathrel{+}= \bar{v}_i / (2\, v_i)$ |

After the sweep, the adjoint $\bar{v}_k$ of each leaf node $k$ is exactly $\partial L / \partial v_k$ — the partial derivative of the output with respect to that input.

### Private math primitives

All transcendentals are computed without `<math.h>`:

| Primitive | Method |
|---|---|
| $\cos(x)$, any $x$ | Period-reduce to $[0, \pi]$, then 12-term Taylor series |
| $\sin(x)$, any $x$ | $\sin(x) = \cos(\pi/2 - x)$ after sign / period handling |
| $\sqrt{x}$ | Newton–Raphson, 64 iterations |
| $\exp(x)$ | Range-reduce via $x/128$, 20-term Taylor, then 7 squarings ($2^7 = 128$) |
| $\ln(x)$ | Range-reduce to $[1,2)$; $\ln x = 2\,\mathrm{atanh}\!\left(\tfrac{x-1}{x+1}\right)$ with 20 terms, then add $n\ln 2$ |

---

## Complexity

| Mode | Forward | Backward | Memory |
|---|---|---|---|
| Forward-mode | $O(n)$ where $n$ = operations | none | $O(1)$ (struct-on-stack) |
| Reverse-mode | $O(n)$ | $O(n)$ | $O(n)$ tape nodes (static) |

Maximum tape length: `NUMX_MAX_AD_TAPE_LEN` (default 4096 nodes).

---

## When to use

- **Forward-mode**: single-variable derivatives or directional derivatives; composable with any C expression; zero allocation; ideal for control loops where derivatives are needed online.
- **Reverse-mode**: gradient of a scalar loss over many parameters (e.g. small neural network, parameter estimation, optimisation); gradient is exact and computed in a single backward pass regardless of input dimension.

## When NOT to use

- Tape overflow — `numx_ad_add` etc. return `NUMX_ERR_BUFFER_SMALL` when the tape is full; increase `NUMX_MAX_AD_TAPE_LEN` at compile time.
- Division by zero in forward-mode — `numx_dual_div` silently returns `{0, 0}`; check inputs before composing.
- `numx_ad_log` / `numx_ad_sqrt` with non-positive / negative arguments — return `NUMX_ERR_INVALID_ARG`; guard inputs.
- Very deep computation graphs without re-initialising the tape — call `numx_ad_init` to reset before each independent computation.

---

## References

1. Griewank, A. & Walther, A. — *Evaluating Derivatives: Principles and Techniques of Algorithmic Differentiation*, 2nd ed., SIAM, 2008.
2. Wengert, R. E. — "A simple automatic derivative evaluation program", *Communications of the ACM* 7(8):463–464, 1964.
3. Baydin, A. G. et al. — "Automatic Differentiation in Machine Learning: a Survey", *JMLR* 18(153):1–43, 2018.

---

## Example

```c
#include "numx/autodiff.h"

/* ── Forward-mode: d/dx (x^2 + sin(x)) at x = 1 ── */
numx_dual_t x  = numx_dual_var(1.0f);
numx_dual_t x2 = numx_dual_mul(x, x);       /* {1, 2*x=2}        */
numx_dual_t sx = numx_dual_sin(x);           /* {sin(1), cos(1)}  */
numx_dual_t r  = numx_dual_add(x2, sx);      /* {1+sin(1), 2+cos(1)} */
/* r.re  = 1 + sin(1) ≈ 1.8415  (function value)  */
/* r.du  = 2 + cos(1) ≈ 2.5403  (exact derivative) */

/* ── Reverse-mode: gradient of z = x^2 + y^2 at (3, 4) ── */
numx_ad_tape_t tape;
numx_size_t xi, yi, x2i, y2i, zi;

numx_ad_init(&tape);
numx_ad_var(&tape, 3.0f, &xi);           /* x = 3 */
numx_ad_var(&tape, 4.0f, &yi);           /* y = 4 */
numx_ad_mul(&tape, xi, xi, &x2i);        /* x^2    */
numx_ad_mul(&tape, yi, yi, &y2i);        /* y^2    */
numx_ad_add(&tape, x2i, y2i, &zi);       /* z = x^2 + y^2 */

numx_ad_backward(&tape, zi);

/* numx_ad_grad(&tape, xi) == 2*3 == 6  (dz/dx) */
/* numx_ad_grad(&tape, yi) == 2*4 == 8  (dz/dy) */
```
