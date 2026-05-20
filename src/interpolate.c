/**
 * @file interpolate.c
 * @brief Interpolation implementation for numx.
 */

#include "numx/interpolate.h"

/* ── Private helpers ───────────────────────────────────────────────── */

static numx_real_t priv_abs(numx_real_t x)
{
    return x < (numx_real_t)0.0 ? -x : x;
}

/* Cosine via Taylor series on [0, pi/2], then reflection for [pi/2, pi].
 * 12 terms give < 1 ULP error for float32 on [0, pi]. */
static numx_real_t priv_cos(numx_real_t x)
{
    numx_real_t sign = (numx_real_t)1.0;
    numx_real_t x2, term, sum;
    int k;
    if (x > NUMX_PI * (numx_real_t)0.5)
    {
        x = NUMX_PI - x;
        sign = -(numx_real_t)1.0;
    }
    x2 = x * x;
    term = (numx_real_t)1.0;
    sum = term;
    for (k = 1; k <= 12; k++)
    {
        term *= -x2 / (numx_real_t)((2 * k - 1) * (2 * k));
        sum += term;
    }
    return sign * sum;
}

/* Binary search: return i such that xs[i] <= x < xs[i+1]. */
static numx_size_t priv_bsearch(const numx_real_t *xs, numx_size_t n, numx_real_t x)
{
    numx_size_t lo = 0, hi = n - 2, mid;
    while (lo < hi)
    {
        mid = (lo + hi + 1) / 2;
        if (xs[mid] <= x)
            lo = mid;
        else
            hi = mid - 1;
    }
    return lo;
}

/* ── Linear interpolation ──────────────────────────────────────────── */

numx_status_t numx_interp_linear(
    const numx_real_t *xs,
    const numx_real_t *ys,
    numx_size_t n,
    numx_real_t x,
    numx_real_t *result)
{
    numx_size_t i;
    numx_real_t t;

    if (!xs || !ys || !result)
        return NUMX_ERR_NULL_PTR;
    if (n < 2 || n > NUMX_MAX_INTERP_NODES)
        return NUMX_ERR_INVALID_ARG;

    if (x <= xs[0])
    {
        *result = ys[0];
        return NUMX_OK;
    }
    if (x >= xs[n - 1])
    {
        *result = ys[n - 1];
        return NUMX_OK;
    }

    i = priv_bsearch(xs, n, x);
    t = (x - xs[i]) / (xs[i + 1] - xs[i]);
    *result = ys[i] + t * (ys[i + 1] - ys[i]);
    return NUMX_OK;
}

/* ── Natural cubic spline ──────────────────────────────────────────── */

numx_status_t numx_interp_spline_precompute(
    const numx_real_t *xs,
    const numx_real_t *ys,
    numx_size_t n,
    numx_real_t *m)
{
    /* Thomas algorithm for the tridiagonal system arising from natural
     * cubic spline conditions: m[0] = m[n-1] = 0. */
    numx_real_t diag[NUMX_MAX_INTERP_NODES];
    numx_real_t rhs[NUMX_MAX_INTERP_NODES];
    numx_real_t hi_1, hi, w;
    numx_size_t i;

    if (!xs || !ys || !m)
        return NUMX_ERR_NULL_PTR;
    if (n < 2 || n > NUMX_MAX_INTERP_NODES)
        return NUMX_ERR_INVALID_ARG;

    /* Natural boundary conditions: m[0] = m[n-1] = 0. */
    m[0] = m[n - 1] = (numx_real_t)0.0;

    if (n == 2)
        return NUMX_OK; /* linear segment — no interior moments */

    /* Build the system for interior moments m[1..n-2]. */
    for (i = 1; i < n - 1; i++)
    {
        hi_1 = xs[i] - xs[i - 1];
        hi = xs[i + 1] - xs[i];
        diag[i] = (numx_real_t)2.0 * (hi_1 + hi);
        rhs[i] = (numx_real_t)6.0 * ((ys[i + 1] - ys[i]) / hi - (ys[i] - ys[i - 1]) / hi_1);
    }

    /* Forward sweep (Thomas). */
    for (i = 2; i < n - 1; i++)
    {
        hi_1 = xs[i - 1] - xs[i - 2]; /* h_{i-1} */
        w = (xs[i] - xs[i - 1]) / diag[i - 1];
        diag[i] -= w * (xs[i] - xs[i - 1]);
        rhs[i] -= w * rhs[i - 1];
        (void)hi_1;
    }

    /* Back substitution. */
    m[n - 2] = rhs[n - 2] / diag[n - 2];
    for (i = n - 3; i >= 1; i--)
    {
        numx_real_t h_i = xs[i + 1] - xs[i];
        m[i] = (rhs[i] - h_i * m[i + 1]) / diag[i];
        if (i == 0)
            break; /* guard for unsigned wrap */
    }
    return NUMX_OK;
}

numx_status_t numx_interp_spline_eval(
    const numx_real_t *xs,
    const numx_real_t *ys,
    const numx_real_t *m,
    numx_size_t n,
    numx_real_t x,
    numx_real_t *result)
{
    numx_size_t i;
    numx_real_t h, t, a0, a1, a2, a3;

    if (!xs || !ys || !m || !result)
        return NUMX_ERR_NULL_PTR;
    if (n < 2 || n > NUMX_MAX_INTERP_NODES)
        return NUMX_ERR_INVALID_ARG;

    if (x <= xs[0])
    {
        *result = ys[0];
        return NUMX_OK;
    }
    if (x >= xs[n - 1])
    {
        *result = ys[n - 1];
        return NUMX_OK;
    }

    i = priv_bsearch(xs, n, x);
    h = xs[i + 1] - xs[i];
    t = x - xs[i];

    /* Cubic polynomial in each interval using second derivatives m[i]. */
    a0 = ys[i];
    a1 = (ys[i + 1] - ys[i]) / h - h * ((numx_real_t)2.0 * m[i] + m[i + 1]) / (numx_real_t)6.0;
    a2 = m[i] * (numx_real_t)0.5;
    a3 = (m[i + 1] - m[i]) / ((numx_real_t)6.0 * h);

    *result = a0 + t * (a1 + t * (a2 + t * a3));
    return NUMX_OK;
}

numx_status_t numx_interp_spline_cubic(
    const numx_real_t *xs,
    const numx_real_t *ys,
    numx_size_t n,
    numx_real_t x,
    numx_real_t *result)
{
    numx_real_t m[NUMX_MAX_INTERP_NODES];
    numx_status_t s;

    if (!xs || !ys || !result)
        return NUMX_ERR_NULL_PTR;
    if (n < 2 || n > NUMX_MAX_INTERP_NODES)
        return NUMX_ERR_INVALID_ARG;

    s = numx_interp_spline_precompute(xs, ys, n, m);
    if (s != NUMX_OK)
        return s;
    return numx_interp_spline_eval(xs, ys, m, n, x, result);
}

/* ── Chebyshev interpolation (barycentric form) ────────────────────── */

numx_status_t numx_interp_chebyshev(
    numx_func1d_t f,
    numx_size_t n,
    numx_real_t a,
    numx_real_t b,
    numx_real_t x,
    numx_real_t *result)
{
    numx_real_t fvals[NUMX_MAX_INTERP_NODES];
    numx_real_t nodes[NUMX_MAX_INTERP_NODES];
    numx_real_t num, den, wi, dx;
    numx_size_t i;
    int sign;

    if (!f || !result)
        return NUMX_ERR_NULL_PTR;
    if (n < 2 || n > NUMX_MAX_INTERP_NODES)
        return NUMX_ERR_INVALID_ARG;
    if (b <= a)
        return NUMX_ERR_INVALID_ARG;

    /* Chebyshev nodes of the first kind mapped from [-1,1] to [a,b]. */
    for (i = 0; i < n; i++)
    {
        numx_real_t arg = NUMX_PI * (numx_real_t)(2 * i + 1) / (numx_real_t)(2 * n);
        numx_real_t tc = priv_cos(arg);
        nodes[i] = (numx_real_t)0.5 * ((a + b) + (b - a) * tc);
        fvals[i] = f(nodes[i]);
    }

    /* Barycentric weights: w_i = (-1)^i * sin(pi*(2i+1)/(2n)). */
    num = (numx_real_t)0.0;
    den = (numx_real_t)0.0;
    sign = 1;
    for (i = 0; i < n; i++)
    {
        dx = x - nodes[i];
        if (priv_abs(dx) < NUMX_EPSILON)
        {
            *result = fvals[i];
            return NUMX_OK;
        }
        numx_real_t arg = NUMX_PI * (numx_real_t)(2 * i + 1) / (numx_real_t)(2 * n);
        /* sin(arg) = cos(pi/2 - arg); reuse priv_cos with reflected argument. */
        numx_real_t sa = priv_cos(NUMX_PI * (numx_real_t)0.5 - arg);
        wi = (numx_real_t)sign * sa / dx;
        num += wi * fvals[i];
        den += wi;
        sign = -sign;
    }
    *result = num / den;
    return NUMX_OK;
}
