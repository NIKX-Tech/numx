# Number Theoretic Transform

**Module:** `numx/ntt.h` | **Category:** Post-Quantum Cryptography | **Phase:** P3.04

---

## What it solves

Multiplies polynomials in the ring Z_3329[x]/(x^256 + 1) in O(n log n) time using
a Number Theoretic Transform (NTT). This ring is the algebraic core of CRYSTALS-Kyber
and CRYSTALS-Dilithium, the NIST post-quantum cryptography standards. Naive
multiplication in this ring costs O(n^2) = O(65536) operations; the NTT reduces this
to O(n log n) = O(2048) multiplications.

All arithmetic is exact (no floating-point), constant-time over the data path, and
allocation-free. The implementation targets embedded systems including ARM Cortex-M,
RISC-V, and ESP32.

---

## Mathematical formulations

### The ring

All operations are performed over the quotient polynomial ring:

$$R_q = \mathbb{Z}_q[x]/(x^n + 1), \quad q = 3329,\; n = 256$$

Coefficients live in Z_3329 = {0, 1, ..., 3328}. The ring relation is x^256 = -1,
so high-degree terms wrap back with a sign change.

$q - 1 = 3328 = 2^8 \cdot 13$, which means the multiplicative group Z_3329^* has
order 3328 and supports a primitive 256th root of unity.

### Primitive root and twiddle factors

$\omega = 17$ is a primitive 256th root of unity mod 3329:

$$\omega^{256} \equiv 1 \pmod{3329}, \qquad \omega^{128} \equiv -1 \pmod{3329}$$

This satisfies the negacyclic condition needed for the ring x^256 + 1.

The twiddle factors are $\zeta_k = \omega^{\text{brv}_7(k)} \bmod q$ where $\text{brv}_7$
is the 7-bit bit-reversal permutation. Storing them in bit-reversed order allows the
butterfly loops to access them sequentially (cache-friendly, no index arithmetic).

### Ring factorisation (CRT decomposition)

The Chinese Remainder Theorem factorises $R_q$ into 128 degree-2 rings:

$$R_q \;\cong\; \prod_{k=0}^{127} \mathbb{Z}_q[x]/(x^2 - \zeta^{2k+1})$$

This factorisation exists because $\omega^{128} = -1$ implies:

$$x^{256} + 1 = \prod_{k=0}^{127} (x^2 - \omega^{2k+1})$$

Each factor $\mathbb{Z}_q[x]/(x^2 - c_k)$ is a degree-2 ring with modulus $c_k = \omega^{2k+1} \bmod q$.

### Forward NTT (Cooley-Tukey)

The forward NTT maps $f \in R_q$ to its 128 CRT residues in seven Cooley-Tukey
butterfly stages. Each butterfly with twiddle factor $\zeta$:

$$f[j]       \leftarrow f[j] + \zeta \cdot f[j + \ell] \pmod{q}$$
$$f[j + \ell] \leftarrow f[j] - \zeta \cdot f[j + \ell] \pmod{q}$$

where $\ell$ is the current half-group length (128, 64, 32, 16, 8, 4, 2 across stages).
The output pairs $(f[2i], f[2i+1])$ represent $f \bmod (x^2 - c_i)$ for $i = 0, \ldots, 127$.

### Inverse NTT (Gentleman-Sande)

The inverse NTT uses Gentleman-Sande (decimation-in-frequency) butterflies, processing
stages in reverse order ($\ell = 2, 4, \ldots, 128$). Each inverse butterfly:

$$t           \leftarrow f[j]$$
$$f[j]        \leftarrow t + f[j + \ell] \pmod{q}$$
$$f[j + \ell] \leftarrow \zeta^{-1} \cdot (t - f[j + \ell]) \pmod{q}$$

After all stages, every coefficient is multiplied by the normalization constant
$n^{-1} = 128^{-1} \equiv 3303 \pmod{3329}$, recovering the original polynomial.

**Key invariant:** within each inverse stage, butterfly groups must use the same twiddle
indices as the corresponding forward stage (FORWARD order within each stage, not
reversed), because the group structure in the Gentleman-Sande form only undoes the
forward Cooley-Tukey correctly when paired this way.

### Polynomial addition and subtraction

Addition and subtraction of polynomials (or their NTT representations) are coefficient-wise operations mod $q$:

$$\text{poly\_add}: \quad h[k] = (a[k] + b[k]) \bmod q$$

$$\text{poly\_sub}: \quad h[k] = (a[k] - b[k] + q) \bmod q$$

Because the NTT is a linear map over $\mathbb{Z}_q$, these operations are valid in both the polynomial domain and the NTT domain without any conversion. The $+q$ term in subtraction prevents negative intermediate values before the modular reduction.

Both functions apply Barrett reduction to keep outputs in $[0, q-1]$.

### Pointwise multiplication (basemul)

Once both polynomials are in NTT domain, their product is computed in O(n) time via
128 independent degree-2 ring multiplications. For each pair $i$ with twiddle $c_i$:

$$(a_0 + a_1 x)(b_0 + b_1 x) \bmod (x^2 - c_i) = (a_0 b_0 + c_i a_1 b_1) + (a_0 b_1 + a_1 b_0)\, x$$

### Barrett reduction

All modular multiplications use Barrett reduction to avoid the hardware division
instruction, which is slow or absent on embedded processors:

$$\text{barrett}(a) = a - \left\lfloor \frac{a \cdot v}{2^{26}} \right\rfloor \cdot q, \qquad v = \left\lceil \frac{2^{26}}{3329} \right\rceil = 20159$$

For inputs in $[0,\, 2q^2] \approx [0,\, 22\text{M}]$, a single conditional subtract
brings the result into $[0, q-1]$. The 32-bit intermediate product fits in a 64-bit
integer (available as `uint64_t` in C99).

---

## Complexity

| Function | Time | Stack (bytes) |
|---|---|---|
| `numx_ntt_forward` | $O(n \log n)$ = 2048 multiplications | in-place |
| `numx_ntt_inverse` | $O(n \log n)$ + normalization | in-place |
| `numx_ntt_pointwise_mul` | $O(n)$ = 128 degree-2 ring muls | in-place |
| `numx_ntt_polymul` | $O(n \log n)$ | 512 bytes (two temp polynomials) |
| `numx_ntt_poly_add` | $O(n)$ | in-place |
| `numx_ntt_poly_sub` | $O(n)$ | in-place |
| `numx_ntt_reduce` | $O(n)$ | in-place |

Fixed input size: n = 256 (NUMX_NTT_N). No dynamic allocation.

---

## Data format

All functions operate on arrays of `numx_q15_t` (int16_t) of length `NUMX_NTT_N = 256`.

**Polynomial domain:** `f[k]` is the coefficient of $x^k$; coefficients in $[0, q-1]$.

**NTT domain:** `f[2i]` and `f[2i+1]` are the constant and linear coefficients of
$f \bmod (x^2 - c_i)$ for $i = 0, \ldots, 127$. Coefficients in $[0, q-1]$.

---

## When to use

- **`numx_ntt_forward` + `numx_ntt_pointwise_mul` + `numx_ntt_inverse`**: multiply
  one polynomial against many others (e.g., Kyber key encapsulation, matrix-vector
  products in the ring). Forward NTT a fixed operand once; reuse the NTT form.
- **`numx_ntt_polymul`**: one-shot polynomial multiplication; handles the full
  forward-pointwise-inverse pipeline with stack-only temporaries.
- **`numx_ntt_poly_add` / `numx_ntt_poly_sub`**: add or subtract two polynomials
  (or two NTT-domain arrays) coefficient-wise mod q. Both are valid in either domain
  because the NTT is a linear map. Use these to accumulate results in Kyber/Dilithium
  matrix-vector products before calling `numx_ntt_inverse` once at the end.
- **`numx_ntt_reduce`**: after external coefficient additions (e.g., adding two
  polynomials manually), reduce all coefficients back to $[0, q-1]$ before NTT.

## When NOT to use

- Polynomials not over $\mathbb{Z}_{3329}$ — the twiddle tables are fixed to q = 3329
  and n = 256. Other moduli or ring dimensions require a separate NTT implementation.
- Single-pair convolution where n << 256 — the transform always processes all 256
  coefficients regardless of actual polynomial degree.
- Floating-point polynomial convolution — use `numx_fft_f32` + pointwise complex
  multiply + `numx_ifft_f32` instead.

---

## References

1. Avanzi, R. et al. — *CRYSTALS-Kyber Algorithm Specifications and Supporting
   Documentation*, v3.02, NIST PQC Round 3 Submission, 2021.
   Available at: https://pq-crystals.org/kyber/data/kyber-specification-round3-20210804.pdf
2. Longa, P. & Naehrig, M. — "Speeding up the Number Theoretic Transform for Faster
   Ideal Lattice-Based Cryptography", *CANS 2016*, LNCS 10052, pp. 124-139, 2016.
3. Gentleman, W. M. & Sande, G. — "Fast Fourier Transforms: For Fun and Profit",
   *AFIPS Fall Joint Computing Conf.*, Vol. 29, pp. 563-578, 1966.
4. Barrett, P. — "Implementing the Rivest Shamir and Adleman Public Key Encryption
   Algorithm on a Standard Digital Signal Processor", *CRYPTO 1986*, LNCS 263, 1987.

---

## Example

```c
#include "numx/ntt.h"

/* Polynomial multiplication: f * g in Z_3329[x]/(x^256+1). */
numx_q15_t f[256] = {0};   /* f(x) = 1 + 2x */
numx_q15_t g[256] = {0};   /* g(x) = 3 + x  */
numx_q15_t h[256];         /* h = f * g      */

f[0] = 1; f[1] = 2;
g[0] = 3; g[1] = 1;

numx_ntt_polymul(f, g, h);
/* h[0]=3, h[1]=7, h[2]=2, rest=0:
 * (1+2x)(3+x) = 3 + 7x + 2x^2  */

/* One-polynomial-against-many pattern (e.g. Kyber matrix-vector): */
numx_q15_t a[256], b[256], c[256], out[256];
/* ... fill a[], b[], c[] ... */

numx_ntt_forward(a);   /* forward NTT of fixed operand — done once */
numx_ntt_forward(b);
numx_ntt_pointwise_mul(a, b, out);
numx_ntt_inverse(out); /* result in polynomial domain */

numx_ntt_forward(c);   /* reuse NTT(a) for second product */
numx_ntt_pointwise_mul(a, c, out);
numx_ntt_inverse(out);

/* poly_add / poly_sub — works in polynomial domain or NTT domain */
numx_q15_t p[256] = {0}, q_poly[256] = {0}, sum[256], diff[256];
p[0] = 1; p[1] = 2;       /* p(x) = 1 + 2x */
q_poly[0] = 3; q_poly[1] = 1; /* q(x) = 3 + x  */

numx_ntt_poly_add(p, q_poly, sum);
/* sum[0] = 4, sum[1] = 3 */

numx_ntt_poly_sub(p, q_poly, diff);
/* diff[0] = 3325 (= -2 mod 3329), diff[1] = 1 */

/* Kyber matrix-vector accumulation pattern: sum of products in NTT domain */
numx_q15_t acc[256] = {0};
numx_q15_t tmp[256];
/* for each row element: NTT(a_i) * NTT(s_i), accumulate */
numx_ntt_pointwise_mul(a, b, tmp);
numx_ntt_poly_add(acc, tmp, acc);   /* acc += a*b in NTT domain */
numx_ntt_inverse(acc);              /* single INTT at the end */
```
