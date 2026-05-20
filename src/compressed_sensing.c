/**
 * @file compressed_sensing.c
 * @brief OMP and ISTA sparse recovery for numx.
 */

#include "numx/compressed_sensing.h"

/* ── Private helpers ───────────────────────────────────────────────── */

/* Newton-Raphson square root. */
static numx_real_t priv_sqrt_cs(numx_real_t x)
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

/* In-place LU solver for an ss×ss system stored with stride
 * NUMX_MAX_CS_SPARSITY.  On entry: LU holds the matrix, b the RHS.
 * On success: b is overwritten with the solution.
 * Uses Gaussian elimination with partial pivoting. */
static numx_status_t priv_lu_solve(
    numx_real_t *LU,
    numx_real_t *b,
    numx_size_t  ss)
{
    numx_size_t i, j, k, max_row;
    numx_real_t max_val, val, factor;

    /* Forward elimination with partial pivoting. */
    for (k = 0; k < ss; k++)
    {
        /* Find the largest-magnitude entry in column k at or below row k. */
        max_val = LU[k * NUMX_MAX_CS_SPARSITY + k];
        if (max_val < (numx_real_t)0.0)
            max_val = -max_val;
        max_row = k;
        for (i = k + 1; i < ss; i++)
        {
            val = LU[i * NUMX_MAX_CS_SPARSITY + k];
            if (val < (numx_real_t)0.0)
                val = -val;
            if (val > max_val)
            {
                max_val = val;
                max_row = i;
            }
        }
        if (max_val < NUMX_EPSILON)
            return NUMX_ERR_SINGULAR;

        /* Swap rows k ↔ max_row. */
        if (max_row != k)
        {
            for (j = 0; j < ss; j++)
            {
                val = LU[k * NUMX_MAX_CS_SPARSITY + j];
                LU[k * NUMX_MAX_CS_SPARSITY + j]       = LU[max_row * NUMX_MAX_CS_SPARSITY + j];
                LU[max_row * NUMX_MAX_CS_SPARSITY + j] = val;
            }
            val      = b[k];
            b[k]     = b[max_row];
            b[max_row] = val;
        }

        /* Eliminate entries below the pivot. */
        for (i = k + 1; i < ss; i++)
        {
            factor = LU[i * NUMX_MAX_CS_SPARSITY + k]
                   / LU[k * NUMX_MAX_CS_SPARSITY + k];
            for (j = k; j < ss; j++)
                LU[i * NUMX_MAX_CS_SPARSITY + j] -=
                    factor * LU[k * NUMX_MAX_CS_SPARSITY + j];
            b[i] -= factor * b[k];
        }
    }

    /* Back-substitution (column-oriented). */
    for (k = ss; k-- > 0; )
    {
        b[k] /= LU[k * NUMX_MAX_CS_SPARSITY + k];
        for (j = 0; j < k; j++)
            b[j] -= LU[j * NUMX_MAX_CS_SPARSITY + k] * b[k];
    }
    return NUMX_OK;
}

/* ── OMP ───────────────────────────────────────────────────────────── */

numx_status_t numx_cs_omp(
    const numx_real_t *A,
    const numx_real_t *y,
    numx_size_t m, numx_size_t n, numx_size_t k,
    numx_real_t *x)
{
    numx_size_t support[NUMX_MAX_CS_SPARSITY];
    int         selected[NUMX_MAX_CS_SIGNAL_DIM];
    numx_real_t G[NUMX_MAX_CS_SPARSITY * NUMX_MAX_CS_SPARSITY];
    numx_real_t LU_ws[NUMX_MAX_CS_SPARSITY * NUMX_MAX_CS_SPARSITY];
    numx_real_t b[NUMX_MAX_CS_SPARSITY];
    numx_real_t coeffs[NUMX_MAX_CS_SPARSITY];
    numx_real_t residual[NUMX_MAX_CS_MEASUREMENTS];
    numx_size_t s, i, j, row, ss;
    numx_real_t dot, best_corr, corr;
    numx_size_t j_best;
    numx_status_t stat;

    if (!A || !y || !x)
        return NUMX_ERR_NULL_PTR;
    if (m == 0 || n == 0 || k == 0)
        return NUMX_ERR_INVALID_ARG;
    if (m > NUMX_MAX_CS_MEASUREMENTS || n > NUMX_MAX_CS_SIGNAL_DIM)
        return NUMX_ERR_INVALID_ARG;
    if (k > n || k > m || k > NUMX_MAX_CS_SPARSITY)
        return NUMX_ERR_INVALID_ARG;

    /* Initialise residual = y, zero output and selection flags. */
    for (i = 0; i < m; i++)
        residual[i] = y[i];
    for (i = 0; i < n; i++)
    {
        x[i]        = (numx_real_t)0.0;
        selected[i] = 0;
    }

    for (s = 0; s < k; s++)
    {
        /* Find the unselected column most correlated with the residual. */
        j_best    = n; /* sentinel: n is out-of-range */
        best_corr = (numx_real_t)0.0;
        for (j = 0; j < n; j++)
        {
            if (selected[j])
                continue;
            dot = (numx_real_t)0.0;
            for (row = 0; row < m; row++)
                dot += A[row * n + j] * residual[row];
            corr = dot < (numx_real_t)0.0 ? -dot : dot;
            if (corr > best_corr)
            {
                best_corr = corr;
                j_best    = j;
            }
        }
        if (j_best == n || best_corr < NUMX_EPSILON)
            break; /* residual orthogonal to all remaining atoms */

        support[s]       = j_best;
        selected[j_best] = 1;
        ss               = s + 1;

        /* Extend the Gram matrix by one row and column for atom s. */
        for (i = 0; i < ss; i++)
        {
            dot = (numx_real_t)0.0;
            for (row = 0; row < m; row++)
                dot += A[row * n + j_best] * A[row * n + support[i]];
            G[s * NUMX_MAX_CS_SPARSITY + i] = dot;
            G[i * NUMX_MAX_CS_SPARSITY + s] = dot;
        }

        /* b[s] = A[:,j_best]^T y */
        dot = (numx_real_t)0.0;
        for (row = 0; row < m; row++)
            dot += A[row * n + j_best] * y[row];
        b[s] = dot;

        /* Copy G and b into the workspace; solve G * coeffs = b. */
        for (i = 0; i < ss; i++)
        {
            for (j = 0; j < ss; j++)
                LU_ws[i * NUMX_MAX_CS_SPARSITY + j] =
                    G[i * NUMX_MAX_CS_SPARSITY + j];
            coeffs[i] = b[i];
        }
        stat = priv_lu_solve(LU_ws, coeffs, ss);
        if (stat != NUMX_OK)
            return stat;

        /* Update residual: r = y − A_support * coeffs. */
        for (row = 0; row < m; row++)
        {
            residual[row] = y[row];
            for (i = 0; i < ss; i++)
                residual[row] -= A[row * n + support[i]] * coeffs[i];
        }
    }

    /* Write solution: only the selected support entries are non-zero. */
    for (i = 0; i < s; i++)
        x[support[i]] = coeffs[i];
    return NUMX_OK;
}

/* ── ISTA ──────────────────────────────────────────────────────────── */

numx_status_t numx_cs_ista(
    const numx_real_t *A,
    const numx_real_t *y,
    numx_size_t m, numx_size_t n,
    numx_real_t lambda, numx_real_t step,
    numx_size_t max_iter,
    numx_real_t *x)
{
    numx_real_t Ax[NUMX_MAX_CS_MEASUREMENTS];
    numx_real_t AtR[NUMX_MAX_CS_SIGNAL_DIM];
    numx_size_t t, i, j;
    numx_real_t threshold, v;

    if (!A || !y || !x)
        return NUMX_ERR_NULL_PTR;
    if (m == 0 || n == 0)
        return NUMX_ERR_INVALID_ARG;
    if (m > NUMX_MAX_CS_MEASUREMENTS || n > NUMX_MAX_CS_SIGNAL_DIM)
        return NUMX_ERR_INVALID_ARG;
    if (step <= (numx_real_t)0.0 || lambda < (numx_real_t)0.0)
        return NUMX_ERR_INVALID_ARG;

    /* Initialise x = 0. */
    for (j = 0; j < n; j++)
        x[j] = (numx_real_t)0.0;

    threshold = lambda * step;

    for (t = 0; t < max_iter; t++)
    {
        /* Compute r = A*x − y in Ax[]. */
        for (i = 0; i < m; i++)
        {
            Ax[i] = (numx_real_t)0.0;
            for (j = 0; j < n; j++)
                Ax[i] += A[i * n + j] * x[j];
            Ax[i] -= y[i];
        }

        /* Compute gradient = A^T * r. */
        for (j = 0; j < n; j++)
        {
            AtR[j] = (numx_real_t)0.0;
            for (i = 0; i < m; i++)
                AtR[j] += A[i * n + j] * Ax[i];
        }

        /* Gradient step then soft-threshold. */
        for (j = 0; j < n; j++)
        {
            v = x[j] - step * AtR[j];
            if (v > threshold)
                x[j] = v - threshold;
            else if (v < -threshold)
                x[j] = v + threshold;
            else
                x[j] = (numx_real_t)0.0;
        }
    }
    return NUMX_OK;
}

/* ── Spectral norm via power iteration ─────────────────────────────── */

numx_status_t numx_cs_spectral_norm(
    const numx_real_t *A,
    numx_size_t m, numx_size_t n,
    numx_size_t max_iter,
    numx_real_t *result)
{
    numx_real_t v[NUMX_MAX_CS_SIGNAL_DIM];
    numx_real_t w[NUMX_MAX_CS_MEASUREMENTS];
    numx_real_t u[NUMX_MAX_CS_SIGNAL_DIM];
    numx_size_t i, j, t;
    numx_real_t norm, wi;

    if (!A || !result)
        return NUMX_ERR_NULL_PTR;
    if (m == 0 || n == 0)
        return NUMX_ERR_INVALID_ARG;
    if (m > NUMX_MAX_CS_MEASUREMENTS || n > NUMX_MAX_CS_SIGNAL_DIM)
        return NUMX_ERR_INVALID_ARG;

    /* Initialise v = [1/sqrt(n), ..., 1/sqrt(n)] (uniform unit vector). */
    norm = priv_sqrt_cs((numx_real_t)1.0 / (numx_real_t)n);
    for (j = 0; j < n; j++)
        v[j] = norm;

    /* Power iteration on A^T A. */
    for (t = 0; t < max_iter; t++)
    {
        /* w = A*v */
        for (i = 0; i < m; i++)
        {
            w[i] = (numx_real_t)0.0;
            for (j = 0; j < n; j++)
                w[i] += A[i * n + j] * v[j];
        }
        /* u = A^T * w */
        for (j = 0; j < n; j++)
        {
            u[j] = (numx_real_t)0.0;
            for (i = 0; i < m; i++)
                u[j] += A[i * n + j] * w[i];
        }
        /* norm = ||u||; if zero the matrix is zero */
        norm = (numx_real_t)0.0;
        for (j = 0; j < n; j++)
            norm += u[j] * u[j];
        norm = priv_sqrt_cs(norm);
        if (norm < NUMX_EPSILON)
            break;
        /* v = u / norm */
        for (j = 0; j < n; j++)
            v[j] = u[j] / norm;
    }

    /* sigma_max = ||A * v_converged||. */
    norm = (numx_real_t)0.0;
    for (i = 0; i < m; i++)
    {
        wi = (numx_real_t)0.0;
        for (j = 0; j < n; j++)
            wi += A[i * n + j] * v[j];
        norm += wi * wi;
    }
    *result = priv_sqrt_cs(norm);
    return NUMX_OK;
}
