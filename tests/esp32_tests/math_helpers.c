#include "math_helpers.h"
#include "numx/numx_types.h"

numx_real_t f_lin(numx_real_t x)   { return x - (numx_real_t)2.0; }
numx_real_t df_lin(numx_real_t x)  { (void)x; return (numx_real_t)1.0; }

numx_real_t f_quad(numx_real_t x)  { return x*x - (numx_real_t)4.0; }
numx_real_t df_quad(numx_real_t x) { return (numx_real_t)2.0*x; }

numx_real_t f_cubic(numx_real_t x)  { return x*x*x - x; }
numx_real_t df_cubic(numx_real_t x) { return (numx_real_t)3.0*x*x - (numx_real_t)1.0; }

numx_real_t f_dbl(numx_real_t x)  { numx_real_t d = x - (numx_real_t)1.0; return d*d; }
numx_real_t df_dbl(numx_real_t x) { return (numx_real_t)2.0*(x - (numx_real_t)1.0); }

numx_real_t f_one(numx_real_t x)  { (void)x; return (numx_real_t)1.0; }
numx_real_t f_xsq(numx_real_t x)  { return x*x; }
numx_real_t f_xcb(numx_real_t x)  { return x*x*x; }
numx_real_t f_xlin(numx_real_t x) { return x; }
numx_real_t f_x3(numx_real_t x)   { return x*x*x; }
numx_real_t f_sq(numx_real_t x)   { return x*x; }
numx_real_t f_neg_x(numx_real_t x) { return -x; }
numx_real_t f_c3(numx_real_t x)   { (void)x; return (numx_real_t)3.0; }

numx_status_t ode_decay(numx_real_t t, const numx_real_t *y, numx_size_t n, numx_real_t *d)
{ (void)t; (void)n; d[0] = -y[0]; return NUMX_OK; }

numx_status_t ode_growth(numx_real_t t, const numx_real_t *y, numx_size_t n, numx_real_t *d)
{ (void)t; (void)n; d[0] = y[0]; return NUMX_OK; }

numx_status_t ode_harm(numx_real_t t, const numx_real_t *y, numx_size_t n, numx_real_t *d)
{ (void)t; (void)n; d[0] = y[1]; d[1] = -y[0]; return NUMX_OK; }

numx_status_t ode_err(numx_real_t t, const numx_real_t *y, numx_size_t n, numx_real_t *d)
{ (void)t; (void)y; (void)n; (void)d; return NUMX_ERR_SINGULAR; }
