#ifndef NUMX_MATH_HELPERS_H
#define NUMX_MATH_HELPERS_H

#include "numx/numx_types.h"

numx_real_t f_lin(numx_real_t x);
numx_real_t df_lin(numx_real_t x);
numx_real_t f_quad(numx_real_t x);
numx_real_t df_quad(numx_real_t x);
numx_real_t f_cubic(numx_real_t x);
numx_real_t df_cubic(numx_real_t x);
numx_real_t f_dbl(numx_real_t x);
numx_real_t df_dbl(numx_real_t x);
numx_real_t f_one(numx_real_t x);
numx_real_t f_xsq(numx_real_t x);
numx_real_t f_xcb(numx_real_t x);
numx_real_t f_xlin(numx_real_t x);
numx_real_t f_x3(numx_real_t x);
numx_real_t f_sq(numx_real_t x);
numx_real_t f_neg_x(numx_real_t x);
numx_real_t f_c3(numx_real_t x);

numx_status_t ode_decay(numx_real_t t, const numx_real_t *y, numx_size_t n, numx_real_t *d);
numx_status_t ode_growth(numx_real_t t, const numx_real_t *y, numx_size_t n, numx_real_t *d);
numx_status_t ode_harm(numx_real_t t, const numx_real_t *y, numx_size_t n, numx_real_t *d);
numx_status_t ode_err(numx_real_t t, const numx_real_t *y, numx_size_t n, numx_real_t *d);

#endif /* NUMX_MATH_HELPERS_H */
