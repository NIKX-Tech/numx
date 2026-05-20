/**
 * @file ode.c
 * @brief ODE solver implementation for numx (RK4 and RKF45).
 */

#include "numx/ode.h"

static numx_real_t priv_abs(numx_real_t x)
{
    return x < (numx_real_t)0.0 ? -x : x;
}

/* ── RK4 fixed-step ────────────────────────────────────────────────── */

numx_status_t numx_ode_rk4(
    numx_ode_func_t f,
    numx_real_t t0,
    const numx_real_t *y0,
    numx_size_t n,
    numx_real_t h,
    numx_size_t steps,
    numx_real_t *y)
{
    numx_real_t k1[NUMX_MAX_ODE_DIM], k2[NUMX_MAX_ODE_DIM];
    numx_real_t k3[NUMX_MAX_ODE_DIM], k4[NUMX_MAX_ODE_DIM];
    numx_real_t ytmp[NUMX_MAX_ODE_DIM];
    numx_real_t t;
    numx_size_t i, step;
    numx_status_t s;

    if (!f || !y0 || !y)
        return NUMX_ERR_NULL_PTR;
    if (n == 0 || n > NUMX_MAX_ODE_DIM)
        return NUMX_ERR_INVALID_ARG;
    if (h <= (numx_real_t)0.0 || steps == 0)
        return NUMX_ERR_INVALID_ARG;

    for (i = 0; i < n; i++)
        y[i] = y0[i];
    t = t0;

    for (step = 0; step < steps; step++)
    {
        /* k1 = f(t, y) */
        s = f(t, y, n, k1);
        if (s != NUMX_OK)
            return s;

        /* k2 = f(t + h/2, y + h*k1/2) */
        for (i = 0; i < n; i++)
            ytmp[i] = y[i] + (numx_real_t)0.5 * h * k1[i];
        s = f(t + (numx_real_t)0.5 * h, ytmp, n, k2);
        if (s != NUMX_OK)
            return s;

        /* k3 = f(t + h/2, y + h*k2/2) */
        for (i = 0; i < n; i++)
            ytmp[i] = y[i] + (numx_real_t)0.5 * h * k2[i];
        s = f(t + (numx_real_t)0.5 * h, ytmp, n, k3);
        if (s != NUMX_OK)
            return s;

        /* k4 = f(t + h, y + h*k3) */
        for (i = 0; i < n; i++)
            ytmp[i] = y[i] + h * k3[i];
        s = f(t + h, ytmp, n, k4);
        if (s != NUMX_OK)
            return s;

        for (i = 0; i < n; i++)
            y[i] += h * (k1[i] + (numx_real_t)2.0 * k2[i] + (numx_real_t)2.0 * k3[i] + k4[i]) / (numx_real_t)6.0;
        t += h;
    }
    return NUMX_OK;
}

/* ── RKF45 adaptive-step (Fehlberg coefficients) ───────────────────── */

numx_status_t numx_ode_rk45(
    numx_ode_func_t f,
    numx_real_t t0,
    numx_real_t t1,
    const numx_real_t *y0,
    numx_size_t n,
    numx_real_t tol,
    numx_real_t *y)
{
    numx_real_t k1[NUMX_MAX_ODE_DIM], k2[NUMX_MAX_ODE_DIM];
    numx_real_t k3[NUMX_MAX_ODE_DIM], k4[NUMX_MAX_ODE_DIM];
    numx_real_t k5[NUMX_MAX_ODE_DIM], k6[NUMX_MAX_ODE_DIM];
    numx_real_t ytmp[NUMX_MAX_ODE_DIM], y4[NUMX_MAX_ODE_DIM];
    numx_real_t t, h, err, scale, ei;
    numx_size_t i;
    int iter;
    numx_status_t s;

    if (!f || !y0 || !y)
        return NUMX_ERR_NULL_PTR;
    if (n == 0 || n > NUMX_MAX_ODE_DIM)
        return NUMX_ERR_INVALID_ARG;
    if (tol <= (numx_real_t)0.0 || t1 <= t0)
        return NUMX_ERR_INVALID_ARG;

    for (i = 0; i < n; i++)
        y[i] = y0[i];
    t = t0;
    h = (t1 - t0) / (numx_real_t)10.0; /* initial step = 1/10 of interval */

    for (iter = 0; iter < NUMX_MAX_ITER; iter++)
    {
        if (t >= t1)
            return NUMX_OK;
        if (t + h > t1)
            h = t1 - t;

        /* k1 */
        s = f(t, y, n, k1);
        if (s != NUMX_OK)
            return s;
        /* k2 */
        for (i = 0; i < n; i++)
            ytmp[i] = y[i] + h * (numx_real_t)(1.0 / 4.0) * k1[i];
        s = f(t + h * (numx_real_t)0.25, ytmp, n, k2);
        if (s != NUMX_OK)
            return s;
        /* k3 */
        for (i = 0; i < n; i++)
            ytmp[i] = y[i] + h * ((numx_real_t)(3.0 / 32.0) * k1[i] + (numx_real_t)(9.0 / 32.0) * k2[i]);
        s = f(t + h * (numx_real_t)(3.0 / 8.0), ytmp, n, k3);
        if (s != NUMX_OK)
            return s;
        /* k4 */
        for (i = 0; i < n; i++)
            ytmp[i] = y[i] + h * ((numx_real_t)(1932.0 / 2197.0) * k1[i] - (numx_real_t)(7200.0 / 2197.0) * k2[i] + (numx_real_t)(7296.0 / 2197.0) * k3[i]);
        s = f(t + h * (numx_real_t)(12.0 / 13.0), ytmp, n, k4);
        if (s != NUMX_OK)
            return s;
        /* k5 */
        for (i = 0; i < n; i++)
            ytmp[i] = y[i] + h * ((numx_real_t)(439.0 / 216.0) * k1[i] - (numx_real_t)8.0 * k2[i] + (numx_real_t)(3680.0 / 513.0) * k3[i] - (numx_real_t)(845.0 / 4104.0) * k4[i]);
        s = f(t + h, ytmp, n, k5);
        if (s != NUMX_OK)
            return s;
        /* k6 */
        for (i = 0; i < n; i++)
            ytmp[i] = y[i] + h * (-(numx_real_t)(8.0 / 27.0) * k1[i] + (numx_real_t)2.0 * k2[i] - (numx_real_t)(3544.0 / 2565.0) * k3[i] + (numx_real_t)(1859.0 / 4104.0) * k4[i] - (numx_real_t)(11.0 / 40.0) * k5[i]);
        s = f(t + h * (numx_real_t)0.5, ytmp, n, k6);
        if (s != NUMX_OK)
            return s;

        /* 4th-order solution */
        for (i = 0; i < n; i++)
            y4[i] = y[i] + h * ((numx_real_t)(25.0 / 216.0) * k1[i] + (numx_real_t)(1408.0 / 2565.0) * k3[i] + (numx_real_t)(2197.0 / 4104.0) * k4[i] - (numx_real_t)(1.0 / 5.0) * k5[i]);

        /* 5th-order solution — used only for error estimate */
        /* err = ||y5 - y4||_inf */
        err = (numx_real_t)0.0;
        for (i = 0; i < n; i++)
        {
            ei = priv_abs(h * ((numx_real_t)(1.0 / 360.0) * k1[i] - (numx_real_t)(128.0 / 4275.0) * k3[i] - (numx_real_t)(2197.0 / 75240.0) * k4[i] + (numx_real_t)(1.0 / 50.0) * k5[i] + (numx_real_t)(2.0 / 55.0) * k6[i]));
            if (ei > err)
                err = ei;
        }

        if (err <= tol || err < NUMX_EPSILON)
        {
            /* Accept step. */
            for (i = 0; i < n; i++)
                y[i] = y4[i];
            t += h;
        }

        /* Adjust step size. */
        if (err > NUMX_EPSILON)
        {
            scale = (numx_real_t)0.9 * (err > (numx_real_t)0.0
                                            ? /* (tol/err)^(1/5) approximated */ (tol / err) * (tol / err) * (tol / err) * (tol / err) * (tol / err)
                                            : (numx_real_t)10.0);
            /* Clamp scale to [0.1, 10] */
            if (scale < (numx_real_t)0.1)
                scale = (numx_real_t)0.1;
            if (scale > (numx_real_t)10.0)
                scale = (numx_real_t)10.0;
            h *= scale;
        }
    }
    return NUMX_ERR_NO_CONVERGE;
}
