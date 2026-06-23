#ifndef NUMX_TEST_HELPERS_H
#define NUMX_TEST_HELPERS_H

#include "esp32_tests.h"      /* numxt_ctx_t */
#include "numx/numx_types.h"  /* numx_real_t, numx_status_t */

/**
 * @brief  Absolute value of a numx_real_t — no <math.h> required.
 * @param[in] x  Input value.
 * @return |x|
 */
static inline numx_real_t numxt_absf(numx_real_t x)
{
    return x < (numx_real_t)0 ? -x : x;
}

/**
 * @brief  Print a suite header line.
 * @param[in] name  Suite name string.
 */
void suite(const char *name);

/**
 * @brief  Assert |got - ref| <= tol and record the result in ctx.
 * @param[in,out] ctx    Test context.
 * @param[in]     label  Human-readable assertion label.
 * @param[in]     got    Computed value.
 * @param[in]     ref    Expected value.
 * @param[in]     tol    Absolute tolerance.
 */
void chk_f(numxt_ctx_t *ctx, const char *label,
           numx_real_t got, numx_real_t ref, numx_real_t tol);

/**
 * @brief  Assert got == exp (status codes) and record the result in ctx.
 * @param[in,out] ctx    Test context.
 * @param[in]     label  Human-readable assertion label.
 * @param[in]     got    Returned status code.
 * @param[in]     exp    Expected status code.
 */
void chk_rc(numxt_ctx_t *ctx, const char *label,
            numx_status_t got, numx_status_t exp);

/**
 * @brief  Assert cond != 0 and record the result in ctx.
 * @param[in,out] ctx    Test context.
 * @param[in]     label  Human-readable assertion label.
 * @param[in]     cond   Boolean condition.
 */
void chk_true(numxt_ctx_t *ctx, const char *label, int cond);

#endif /* NUMX_TEST_HELPERS_H */
