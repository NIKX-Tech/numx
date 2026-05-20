/**
 * @file roots.c
 * @brief Root-finding implementation for numx.
 */

#include "numx/roots.h"

static numx_real_t priv_abs(numx_real_t x)
{
    return x < (numx_real_t)0.0 ? -x : x;
}

numx_status_t numx_root_bisect(
    numx_func1d_t f,
    numx_real_t a,
    numx_real_t b,
    numx_real_t tol,
    numx_real_t *root)
{
    numx_real_t fa, fb, mid, fmid;
    int i;

    if (!f || !root)
        return NUMX_ERR_NULL_PTR;
    if (tol <= (numx_real_t)0.0)
        return NUMX_ERR_INVALID_ARG;

    fa = f(a);
    fb = f(b);
    if (fa * fb > (numx_real_t)0.0)
        return NUMX_ERR_INVALID_ARG;
    if (fa == (numx_real_t)0.0)
    {
        *root = a;
        return NUMX_OK;
    }
    if (fb == (numx_real_t)0.0)
    {
        *root = b;
        return NUMX_OK;
    }

    for (i = 0; i < NUMX_MAX_ITER; i++)
    {
        mid = (a + b) * (numx_real_t)0.5;
        fmid = f(mid);
        if (priv_abs(fmid) < NUMX_EPSILON || (b - a) * (numx_real_t)0.5 < tol)
        {
            *root = mid;
            return NUMX_OK;
        }
        if (fa * fmid < (numx_real_t)0.0)
        {
            b = mid;
            fb = fmid;
        }
        else
        {
            a = mid;
            fa = fmid;
        }
    }
    *root = (a + b) * (numx_real_t)0.5;
    return NUMX_ERR_NO_CONVERGE;
}

numx_status_t numx_root_newton(
    numx_func1d_t f,
    numx_func1d_t df,
    numx_real_t x0,
    numx_real_t tol,
    numx_real_t *root)
{
    numx_real_t x, fx, dfx, step;
    int i;

    if (!f || !df || !root)
        return NUMX_ERR_NULL_PTR;
    if (tol <= (numx_real_t)0.0)
        return NUMX_ERR_INVALID_ARG;

    x = x0;
    for (i = 0; i < NUMX_MAX_ITER; i++)
    {
        fx = f(x);
        if (priv_abs(fx) < tol)
        {
            *root = x;
            return NUMX_OK;
        }
        dfx = df(x);
        if (priv_abs(dfx) < NUMX_EPSILON)
            return NUMX_ERR_SINGULAR;
        step = fx / dfx;
        x -= step;
        if (priv_abs(step) < tol)
        {
            *root = x;
            return NUMX_OK;
        }
    }
    return NUMX_ERR_NO_CONVERGE;
}

numx_status_t numx_root_brent(
    numx_func1d_t f,
    numx_real_t a,
    numx_real_t b,
    numx_real_t tol,
    numx_real_t *root)
{
    numx_real_t fa, fb, fc, c, d, s, fs;
    numx_real_t tmp, p, q, qlo, qhi;
    int mflag, i;

    if (!f || !root)
        return NUMX_ERR_NULL_PTR;
    if (tol <= (numx_real_t)0.0)
        return NUMX_ERR_INVALID_ARG;

    fa = f(a);
    fb = f(b);
    if (fa * fb > (numx_real_t)0.0)
        return NUMX_ERR_INVALID_ARG;
    if (priv_abs(fa) < priv_abs(fb))
    {
        tmp = a;
        a = b;
        b = tmp;
        tmp = fa;
        fa = fb;
        fb = tmp;
    }

    c = a;
    fc = fa;
    d = (numx_real_t)0.0;
    mflag = 1;

    for (i = 0; i < NUMX_MAX_ITER; i++)
    {
        if (priv_abs(fb) < NUMX_EPSILON || priv_abs(b - a) < tol)
        {
            *root = b;
            return NUMX_OK;
        }

        if (fa != fc && fb != fc)
        {
            /* Inverse quadratic interpolation. */
            s = a * fb * fc / ((fa - fb) * (fa - fc)) + b * fa * fc / ((fb - fa) * (fb - fc)) + c * fa * fb / ((fc - fa) * (fc - fb));
        }
        else
        {
            /* Secant method. */
            s = b - fb * (b - a) / (fb - fa);
        }

        /* Brent conditions: fall back to bisection if s is out of range. */
        p = ((numx_real_t)3.0 * a + b) * (numx_real_t)0.25;
        qlo = p < b ? p : b;
        qhi = p < b ? b : p;
        q = mflag ? priv_abs(b - c) : priv_abs(c - d);

        if ((s < qlo || s > qhi) || priv_abs(s - b) >= q * (numx_real_t)0.5 || q < tol)
        {
            s = (a + b) * (numx_real_t)0.5;
            mflag = 1;
        }
        else
        {
            mflag = 0;
        }

        fs = f(s);
        d = c;
        c = b;
        fc = fb;

        if (fa * fs < (numx_real_t)0.0)
        {
            b = s;
            fb = fs;
        }
        else
        {
            a = s;
            fa = fs;
        }

        if (priv_abs(fa) < priv_abs(fb))
        {
            tmp = a;
            a = b;
            b = tmp;
            tmp = fa;
            fa = fb;
            fb = tmp;
        }
    }
    *root = b;
    return NUMX_ERR_NO_CONVERGE;
}
