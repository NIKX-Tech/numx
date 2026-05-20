/**
 * @file differentiate.c
 * @brief Numerical differentiation implementation for numx.
 */

#include "numx/differentiate.h"

numx_status_t numx_diff_forward(
    numx_func1d_t f,
    numx_real_t x,
    numx_real_t h,
    numx_real_t *result)
{
    if (!f || !result)
        return NUMX_ERR_NULL_PTR;
    if (h <= (numx_real_t)0.0)
        return NUMX_ERR_INVALID_ARG;

    *result = (f(x + h) - f(x)) / h;
    return NUMX_OK;
}

numx_status_t numx_diff_central(
    numx_func1d_t f,
    numx_real_t x,
    numx_real_t h,
    numx_real_t *result)
{
    if (!f || !result)
        return NUMX_ERR_NULL_PTR;
    if (h <= (numx_real_t)0.0)
        return NUMX_ERR_INVALID_ARG;

    *result = (f(x + h) - f(x - h)) / ((numx_real_t)2.0 * h);
    return NUMX_OK;
}

numx_status_t numx_diff_richardson(
    numx_func1d_t f,
    numx_real_t x,
    numx_real_t h,
    numx_real_t *result)
{
    numx_real_t h2, Dh, Dh2;

    if (!f || !result)
        return NUMX_ERR_NULL_PTR;
    if (h <= (numx_real_t)0.0)
        return NUMX_ERR_INVALID_ARG;

    h2 = h * (numx_real_t)0.5;
    Dh = (f(x + h) - f(x - h)) / ((numx_real_t)2.0 * h);
    Dh2 = (f(x + h2) - f(x - h2)) / ((numx_real_t)2.0 * h2);

    /* Richardson extrapolation cancels O(h^2): (4*Dh2 - Dh) / 3 */
    *result = ((numx_real_t)4.0 * Dh2 - Dh) / (numx_real_t)3.0;
    return NUMX_OK;
}
