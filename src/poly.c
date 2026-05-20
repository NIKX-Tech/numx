/**
 * @file poly.c
 * @brief Polynomial evaluation and root-finding for numx.
 */

#include "numx/poly.h"

static numx_real_t priv_abs(numx_real_t x)
{
    return x < (numx_real_t)0.0 ? -x : x;
}

numx_status_t numx_poly_eval(
    const numx_real_t *coeffs,
    numx_size_t degree,
    numx_real_t x,
    numx_real_t *result)
{
    numx_real_t acc;
    numx_size_t i;

    if (!coeffs || !result)
        return NUMX_ERR_NULL_PTR;
    if (degree > NUMX_MAX_POLY_DEGREE)
        return NUMX_ERR_INVALID_ARG;

    /* Horner's method: p = (...((c0*x + c1)*x + c2)*x + ...) + c_n */
    acc = coeffs[0];
    for (i = 1; i <= degree; i++)
        acc = acc * x + coeffs[i];

    *result = acc;
    return NUMX_OK;
}

numx_status_t numx_poly_roots(
    const numx_real_t *coeffs,
    numx_size_t degree,
    numx_real_t *roots,
    numx_size_t *nroots,
    numx_real_t tol)
{
    /* Working copy of coefficients for deflation. */
    numx_real_t work[NUMX_MAX_POLY_DEGREE + 1];
    numx_real_t starts[] = {0.0f, 1.0f, -1.0f, 2.0f, -2.0f,
                            3.0f, -3.0f, 5.0f, -5.0f, 10.0f, -10.0f};
    numx_size_t nstarts = 11;
    numx_size_t deg, found, s, i;
    numx_real_t x, fx, dfx, step, tmp;
    int iter;

    if (!coeffs || !roots || !nroots)
        return NUMX_ERR_NULL_PTR;
    if (degree == 0 || degree > NUMX_MAX_POLY_DEGREE)
        return NUMX_ERR_INVALID_ARG;
    if (tol <= (numx_real_t)0.0)
        return NUMX_ERR_INVALID_ARG;

    for (i = 0; i <= degree; i++)
        work[i] = coeffs[i];

    found = 0;
    deg = degree;

    while (deg >= 1)
    {
        numx_real_t root = (numx_real_t)0.0;
        int ok = 0;

        for (s = 0; s < nstarts && !ok; s++)
        {
            x = starts[s];
            for (iter = 0; iter < NUMX_MAX_ITER; iter++)
            {
                /* Evaluate polynomial and derivative via Horner. */
                fx = work[0];
                dfx = (numx_real_t)0.0;
                for (i = 1; i <= deg; i++)
                {
                    dfx = dfx * x + fx;
                    fx = fx * x + work[i];
                }
                if (priv_abs(fx) < tol)
                {
                    root = x;
                    ok = 1;
                    break;
                }
                if (priv_abs(dfx) < NUMX_EPSILON)
                    break;
                step = fx / dfx;
                x -= step;
                if (priv_abs(step) < tol)
                {
                    root = x;
                    ok = 1;
                    break;
                }
            }
        }

        if (!ok)
            break; /* no real root found at this degree — likely complex pair */

        roots[found++] = root;

        /* Synthetic division: deflate work[] by (x - root). */
        tmp = work[0];
        for (i = 1; i <= deg; i++)
        {
            numx_real_t next = work[i] + tmp * root;
            work[i - 1] = tmp;
            tmp = next;
        }
        deg--;
    }

    *nroots = found;
    return NUMX_OK;
}
