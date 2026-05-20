/**
 * @file integrate.c
 * @brief Numerical integration implementation for numx.
 */

#include "numx/integrate.h"

/* ── Gauss-Legendre nodes and weights on [-1, 1] ──────────────────── */

static const numx_real_t GL2_X[2] = {
    -5.77350269189625764509e-1f,
    5.77350269189625764509e-1f};
static const numx_real_t GL2_W[2] = {1.0f, 1.0f};

static const numx_real_t GL4_X[4] = {
    -8.61136311594952744974e-1f, -3.39981043584856264803e-1f,
    3.39981043584856264803e-1f, 8.61136311594952744974e-1f};
static const numx_real_t GL4_W[4] = {
    3.47854845137453857374e-1f, 6.52145154862546142626e-1f,
    6.52145154862546142626e-1f, 3.47854845137453857374e-1f};

static const numx_real_t GL8_X[8] = {
    -9.60289856497536231684e-1f, -7.96666477413626739592e-1f,
    -5.25532409916328985818e-1f, -1.83434642495649804940e-1f,
    1.83434642495649804940e-1f, 5.25532409916328985818e-1f,
    7.96666477413626739592e-1f, 9.60289856497536231684e-1f};
static const numx_real_t GL8_W[8] = {
    1.01228536290376259154e-1f, 2.22381034453374470545e-1f,
    3.13706645877887287338e-1f, 3.62683783378361982965e-1f,
    3.62683783378361982965e-1f, 3.13706645877887287338e-1f,
    2.22381034453374470545e-1f, 1.01228536290376259154e-1f};

/* ── Implementations ───────────────────────────────────────────────── */

numx_status_t numx_integrate_trap(
    numx_func1d_t f,
    numx_real_t a,
    numx_real_t b,
    numx_size_t n,
    numx_real_t *result)
{
    numx_real_t h, sum, x;
    numx_size_t i;

    if (!f || !result)
        return NUMX_ERR_NULL_PTR;
    if (n == 0 || b <= a)
        return NUMX_ERR_INVALID_ARG;

    h = (b - a) / (numx_real_t)n;
    sum = f(a) + f(b);
    for (i = 1; i < n; i++)
    {
        x = a + (numx_real_t)i * h;
        sum += (numx_real_t)2.0 * f(x);
    }
    *result = sum * h * (numx_real_t)0.5;
    return NUMX_OK;
}

numx_status_t numx_integrate_simpson(
    numx_func1d_t f,
    numx_real_t a,
    numx_real_t b,
    numx_size_t n,
    numx_real_t *result)
{
    numx_real_t h, sum, x;
    numx_size_t i;

    if (!f || !result)
        return NUMX_ERR_NULL_PTR;
    if (n < 2 || (n & 1u) || b <= a)
        return NUMX_ERR_INVALID_ARG;

    h = (b - a) / (numx_real_t)n;
    sum = f(a) + f(b);
    for (i = 1; i < n; i++)
    {
        x = a + (numx_real_t)i * h;
        sum += (i & 1u) ? (numx_real_t)4.0 * f(x)
                        : (numx_real_t)2.0 * f(x);
    }
    *result = sum * h / (numx_real_t)3.0;
    return NUMX_OK;
}

numx_status_t numx_integrate_gauss(
    numx_func1d_t f,
    numx_real_t a,
    numx_real_t b,
    numx_size_t npts,
    numx_real_t *result)
{
    const numx_real_t *gx, *gw;
    numx_real_t mid, half, sum;
    numx_size_t i;

    if (!f || !result)
        return NUMX_ERR_NULL_PTR;
    if (b <= a)
        return NUMX_ERR_INVALID_ARG;

    switch (npts)
    {
    case 2:
        gx = GL2_X;
        gw = GL2_W;
        break;
    case 4:
        gx = GL4_X;
        gw = GL4_W;
        break;
    case 8:
        gx = GL8_X;
        gw = GL8_W;
        break;
    default:
        return NUMX_ERR_INVALID_ARG;
    }

    mid = (a + b) * (numx_real_t)0.5;
    half = (b - a) * (numx_real_t)0.5;
    sum = (numx_real_t)0.0;
    for (i = 0; i < npts; i++)
        sum += gw[i] * f(mid + half * gx[i]);

    *result = half * sum;
    return NUMX_OK;
}
