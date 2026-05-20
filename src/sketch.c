/**
 * @file sketch.c
 * @brief Randomized SVD — Halko-Martinsson-Tropp algorithm.
 *
 * Stage A: random Gaussian sketch + Modified Gram-Schmidt range finder.
 * Stage B: Gram-matrix Jacobi EVD on the projected matrix.
 */

#include "numx/sketch.h"
#include <stdint.h>

/* ── Private math helpers ──────────────────────────────────────────── */

static numx_real_t priv_sqrt_sk(numx_real_t x)
{
    numx_real_t r;
    int k;
    if (x <= (numx_real_t)0.0)
        return (numx_real_t)0.0;
    r = x;
    for (k = 0; k < 64; k++)
        r = (numx_real_t)0.5 * (r + x / r);
    return r;
}

/* cos for x in [0, π] via Taylor series with π/2 reflection. */
static numx_real_t priv_cos_core_sk(numx_real_t x)
{
    numx_real_t sign, x2, term, sum;
    int k;
    sign = (numx_real_t)1.0;
    if (x > NUMX_PI * (numx_real_t)0.5)
    {
        x    = NUMX_PI - x;
        sign = -(numx_real_t)1.0;
    }
    x2   = x * x;
    term = (numx_real_t)1.0;
    sum  = term;
    for (k = 1; k <= 12; k++)
    {
        term *= -x2 / (numx_real_t)((2 * k - 1) * (2 * k));
        sum  += term;
    }
    return sign * sum;
}

/* Full-range cosine (any real x). */
static numx_real_t priv_cos_sk(numx_real_t x)
{
    numx_real_t two_pi;
    two_pi = (numx_real_t)2.0 * NUMX_PI;
    if (x < (numx_real_t)0.0)
        x = -x;
    while (x > two_pi)
        x -= two_pi;
    if (x > NUMX_PI)
        x = two_pi - x;
    return priv_cos_core_sk(x);
}

/* Natural log via atanh series with range reduction to [1, 2). */
static numx_real_t priv_log_sk(numx_real_t x)
{
    static const numx_real_t LN2 = (numx_real_t)0.6931471805599453;
    numx_real_t u, u2, term, result;
    int n, k;
    if (x <= (numx_real_t)0.0)
        return (numx_real_t)0.0;
    n = 0;
    while (x >= (numx_real_t)2.0) { x *= (numx_real_t)0.5; n++; }
    while (x < (numx_real_t)1.0)  { x *= (numx_real_t)2.0; n--; }
    u      = (x - (numx_real_t)1.0) / (x + (numx_real_t)1.0);
    u2     = u * u;
    term   = u;
    result = term;
    for (k = 1; k <= 20; k++)
    {
        term   *= u2;
        result += term / (numx_real_t)(2 * k + 1);
    }
    return (numx_real_t)2.0 * result + (numx_real_t)n * LN2;
}

/* ── PRNG and Gaussian sampler ─────────────────────────────────────── */

/* Xorshift32 — requires nonzero state. */
static uint32_t priv_xorshift32(uint32_t *state)
{
    uint32_t x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

/* Box-Muller: one standard Gaussian sample per call (uses cos variant). */
static numx_real_t priv_randn_sk(uint32_t *state)
{
    numx_real_t u1, u2;
    /* Clamp u1 away from 0 so log is defined. */
    u1 = (numx_real_t)priv_xorshift32(state) * (numx_real_t)(1.0 / 4294967296.0);
    if (u1 < NUMX_EPSILON)
        u1 = NUMX_EPSILON;
    u2 = (numx_real_t)priv_xorshift32(state) * (numx_real_t)(1.0 / 4294967296.0);
    return priv_sqrt_sk(-(numx_real_t)2.0 * priv_log_sk(u1))
           * priv_cos_sk((numx_real_t)2.0 * NUMX_PI * u2);
}

/* ── Jacobi EVD for a symmetric k×k matrix (stride NUMX_MAX_SKETCH_RANK) ─ */
/* On exit: G has eigenvalues on the diagonal; V holds eigenvectors (columns). */
static void priv_jacobi_evd(numx_real_t *G, numx_real_t *V, numx_size_t k)
{
    numx_size_t p, q, r, sweep;
    numx_real_t theta, t, c, s, tau, gpr, gqr, vpr, vqr, gpq;

    /* Initialise V = I. */
    for (p = 0; p < k; p++)
        for (q = 0; q < k; q++)
            V[p * NUMX_MAX_SKETCH_RANK + q] =
                (p == q) ? (numx_real_t)1.0 : (numx_real_t)0.0;

    for (sweep = 0; sweep < 20; sweep++)
    {
        for (p = 0; p + 1 < k; p++)
        {
            for (q = p + 1; q < k; q++)
            {
                gpq = G[p * NUMX_MAX_SKETCH_RANK + q];
                if (gpq == (numx_real_t)0.0)
                    continue;

                theta = (G[q * NUMX_MAX_SKETCH_RANK + q]
                         - G[p * NUMX_MAX_SKETCH_RANK + p])
                      / ((numx_real_t)2.0 * gpq);
                /* t = sign(θ) / (|θ| + sqrt(1+θ²)) — the small-angle root */
                if (theta >= (numx_real_t)0.0)
                    t = (numx_real_t)1.0
                      / (theta + priv_sqrt_sk((numx_real_t)1.0 + theta * theta));
                else
                    t = -(numx_real_t)1.0
                      / (-theta + priv_sqrt_sk((numx_real_t)1.0 + theta * theta));

                c   = (numx_real_t)1.0
                    / priv_sqrt_sk((numx_real_t)1.0 + t * t);
                s   = c * t;
                tau = s / ((numx_real_t)1.0 + c);

                /* Zero the (p,q) and (q,p) entries. */
                G[p * NUMX_MAX_SKETCH_RANK + p] -= t * gpq;
                G[q * NUMX_MAX_SKETCH_RANK + q] += t * gpq;
                G[p * NUMX_MAX_SKETCH_RANK + q]  = (numx_real_t)0.0;
                G[q * NUMX_MAX_SKETCH_RANK + p]  = (numx_real_t)0.0;

                /* Update the remaining rows/columns. */
                for (r = 0; r < k; r++)
                {
                    if (r == p || r == q)
                        continue;
                    gpr = G[p * NUMX_MAX_SKETCH_RANK + r];
                    gqr = G[q * NUMX_MAX_SKETCH_RANK + r];
                    G[p * NUMX_MAX_SKETCH_RANK + r] = gpr - s * (gqr + tau * gpr);
                    G[r * NUMX_MAX_SKETCH_RANK + p] = G[p * NUMX_MAX_SKETCH_RANK + r];
                    G[q * NUMX_MAX_SKETCH_RANK + r] = gqr + s * (gpr - tau * gqr);
                    G[r * NUMX_MAX_SKETCH_RANK + q] = G[q * NUMX_MAX_SKETCH_RANK + r];
                }

                /* Accumulate eigenvectors. */
                for (r = 0; r < k; r++)
                {
                    vpr = V[r * NUMX_MAX_SKETCH_RANK + p];
                    vqr = V[r * NUMX_MAX_SKETCH_RANK + q];
                    V[r * NUMX_MAX_SKETCH_RANK + p] = vpr - s * (vqr + tau * vpr);
                    V[r * NUMX_MAX_SKETCH_RANK + q] = vqr + s * (vpr - tau * vqr);
                }
            }
        }
    }
}

/* ── Randomized SVD ────────────────────────────────────────────────── */

numx_status_t numx_sketch_rsvd(
    const numx_real_t *A,
    numx_size_t m, numx_size_t n,
    numx_size_t rank, numx_size_t oversample,
    uint32_t seed,
    numx_real_t *U,
    numx_real_t *S,
    numx_real_t *Vt)
{
    numx_real_t Omega[NUMX_MAX_SKETCH_N    * NUMX_MAX_SKETCH_RANK]; /* n  × k_ext */
    numx_real_t Y    [NUMX_MAX_SKETCH_M    * NUMX_MAX_SKETCH_RANK]; /* m  × k_ext (→Q) */
    numx_real_t B    [NUMX_MAX_SKETCH_RANK * NUMX_MAX_SKETCH_N];    /* k_ext × n  */
    numx_real_t G    [NUMX_MAX_SKETCH_RANK * NUMX_MAX_SKETCH_RANK]; /* k_ext × k_ext */
    numx_real_t Ehat [NUMX_MAX_SKETCH_RANK * NUMX_MAX_SKETCH_RANK]; /* eigenvectors */
    numx_real_t sigma[NUMX_MAX_SKETCH_RANK];
    numx_size_t ord  [NUMX_MAX_SKETCH_RANK];
    numx_size_t k_ext, min_mn, i, j, r, p, q;
    numx_real_t dot, norm, val, sigma_inv, tmp_val;
    numx_size_t tmp_idx;
    uint32_t    rng;

    if (!A || !U || !S || !Vt)
        return NUMX_ERR_NULL_PTR;
    if (m == 0 || n == 0 || rank == 0)
        return NUMX_ERR_INVALID_ARG;
    if (m > NUMX_MAX_SKETCH_M || n > NUMX_MAX_SKETCH_N)
        return NUMX_ERR_INVALID_ARG;
    if (rank + oversample > NUMX_MAX_SKETCH_RANK)
        return NUMX_ERR_INVALID_ARG;

    /* Cap k_ext to min(m, n) so the range finder is never over-sized. */
    min_mn = (m < n) ? m : n;
    k_ext  = rank + oversample;
    if (k_ext > min_mn)
        k_ext = min_mn;
    if (rank > k_ext)
        rank = k_ext;

    rng = (seed == 0u) ? 1u : seed;

    /* ── Stage A: random range finder ─────────────────────────────── */

    /* 1. Draw Ω ∈ ℝ^{n × k_ext} with i.i.d. N(0,1) entries. */
    for (j = 0; j < n; j++)
        for (i = 0; i < k_ext; i++)
            Omega[j * NUMX_MAX_SKETCH_RANK + i] = priv_randn_sk(&rng);

    /* 2. Y = A · Ω  (m × k_ext). */
    for (r = 0; r < m; r++)
        for (i = 0; i < k_ext; i++)
            Y[r * NUMX_MAX_SKETCH_RANK + i] = (numx_real_t)0.0;

    for (r = 0; r < m; r++)
        for (j = 0; j < n; j++)
        {
            val = A[r * n + j];
            for (i = 0; i < k_ext; i++)
                Y[r * NUMX_MAX_SKETCH_RANK + i] +=
                    val * Omega[j * NUMX_MAX_SKETCH_RANK + i];
        }

    /* 3. Orthogonalise columns of Y via Modified Gram-Schmidt → Q. */
    for (i = 0; i < k_ext; i++)
    {
        /* Project out previous orthonormal columns. */
        for (p = 0; p < i; p++)
        {
            dot = (numx_real_t)0.0;
            for (r = 0; r < m; r++)
                dot += Y[r * NUMX_MAX_SKETCH_RANK + p]
                     * Y[r * NUMX_MAX_SKETCH_RANK + i];
            for (r = 0; r < m; r++)
                Y[r * NUMX_MAX_SKETCH_RANK + i] -=
                    dot * Y[r * NUMX_MAX_SKETCH_RANK + p];
        }
        /* Normalise or zero out (linearly dependent column). */
        norm = (numx_real_t)0.0;
        for (r = 0; r < m; r++)
        {
            val = Y[r * NUMX_MAX_SKETCH_RANK + i];
            norm += val * val;
        }
        norm = priv_sqrt_sk(norm);
        if (norm < NUMX_EPSILON)
            for (r = 0; r < m; r++)
                Y[r * NUMX_MAX_SKETCH_RANK + i] = (numx_real_t)0.0;
        else
            for (r = 0; r < m; r++)
                Y[r * NUMX_MAX_SKETCH_RANK + i] /= norm;
    }
    /* Y now holds Q (m × k_ext). */

    /* ── Stage B: SVD of the projected matrix ──────────────────────── */

    /* 4. B = Q⊤ · A  (k_ext × n). */
    for (i = 0; i < k_ext; i++)
        for (j = 0; j < n; j++)
        {
            dot = (numx_real_t)0.0;
            for (r = 0; r < m; r++)
                dot += Y[r * NUMX_MAX_SKETCH_RANK + i] * A[r * n + j];
            B[i * NUMX_MAX_SKETCH_N + j] = dot;
        }

    /* 5. G = B · B⊤  (k_ext × k_ext, symmetric). */
    for (p = 0; p < k_ext; p++)
        for (q = 0; q < k_ext; q++)
        {
            dot = (numx_real_t)0.0;
            for (j = 0; j < n; j++)
                dot += B[p * NUMX_MAX_SKETCH_N + j]
                     * B[q * NUMX_MAX_SKETCH_N + j];
            G[p * NUMX_MAX_SKETCH_RANK + q] = dot;
        }

    /* 6. Jacobi eigendecomposition: G → eigenvalues on diagonal, Ehat = eigenvectors. */
    priv_jacobi_evd(G, Ehat, k_ext);

    /* 7. Singular values σ_i = sqrt(λ_i); sort descending. */
    for (i = 0; i < k_ext; i++)
    {
        val      = G[i * NUMX_MAX_SKETCH_RANK + i];
        sigma[i] = (val > (numx_real_t)0.0) ? priv_sqrt_sk(val) : (numx_real_t)0.0;
        ord[i]   = i;
    }
    for (i = 1; i < k_ext; i++) /* insertion sort descending */
    {
        tmp_val = sigma[i];
        tmp_idx = ord[i];
        j = i;
        while (j > 0 && sigma[j - 1] < tmp_val)
        {
            sigma[j] = sigma[j - 1];
            ord[j]   = ord[j - 1];
            j--;
        }
        sigma[j] = tmp_val;
        ord[j]   = tmp_idx;
    }

    /* 8. Fill S. */
    for (i = 0; i < rank; i++)
        S[i] = sigma[i];

    /* 9. U = Q · Ê  (m × rank), taking columns in sorted order. */
    for (r = 0; r < m; r++)
        for (i = 0; i < rank; i++)
        {
            dot = (numx_real_t)0.0;
            for (p = 0; p < k_ext; p++)
                dot += Y[r * NUMX_MAX_SKETCH_RANK + p]
                     * Ehat[p * NUMX_MAX_SKETCH_RANK + ord[i]];
            U[r * rank + i] = dot;
        }

    /* 10. Vt = diag(1/σ) · Ê⊤ · B  (rank × n). */
    for (i = 0; i < rank; i++)
    {
        sigma_inv = (sigma[i] > NUMX_EPSILON)
                  ? (numx_real_t)1.0 / sigma[i]
                  : (numx_real_t)0.0;
        for (j = 0; j < n; j++)
        {
            dot = (numx_real_t)0.0;
            for (p = 0; p < k_ext; p++)
                dot += Ehat[p * NUMX_MAX_SKETCH_RANK + ord[i]]
                     * B[p * NUMX_MAX_SKETCH_N + j];
            Vt[i * n + j] = sigma_inv * dot;
        }
    }

    return NUMX_OK;
}
