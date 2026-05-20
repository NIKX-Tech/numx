/**
 * @file numx_types.h
 * @brief Core type definitions for numx.
 *
 * All numeric types in numx are defined here. Never use float, double,
 * or int directly in public API headers — always use these typedefs.
 * This allows retargeting precision globally via numx_config.h.
 */

#ifndef NUMX_TYPES_H
#define NUMX_TYPES_H

#include <stdint.h>
#include <stddef.h>

/* ── Floating-point precision ──────────────────────────────────────── */

#ifdef NUMX_USE_DOUBLE
typedef double numx_real_t;
#else
typedef float numx_real_t; /* default: single precision */
#endif

/* ── Fixed-point types ─────────────────────────────────────────────── */

typedef int16_t numx_q15_t; /* Q1.15 fixed point  */
typedef int32_t numx_q31_t; /* Q1.31 fixed point  */
typedef int64_t numx_q63_t; /* Q1.63 fixed point  */

/* ── Index and size types ──────────────────────────────────────────── */

typedef uint32_t numx_size_t;
typedef int32_t numx_idx_t;

/* ── Error codes ───────────────────────────────────────────────────── */

typedef enum
{
  NUMX_OK = 0,                /* success                           */
  NUMX_ERR_NULL_PTR = -1,     /* null pointer argument             */
  NUMX_ERR_INVALID_ARG = -2,  /* argument out of valid range       */
  NUMX_ERR_SINGULAR = -3,     /* singular matrix or division/zero  */
  NUMX_ERR_NO_CONVERGE = -4,  /* iterative method did not converge */
  NUMX_ERR_OVERFLOW = -5,     /* arithmetic overflow detected      */
  NUMX_ERR_UNDERFLOW = -6,    /* arithmetic underflow detected     */
  NUMX_ERR_BUFFER_SMALL = -7, /* caller-supplied buffer too small  */
  NUMX_ERR_NOT_IMPL = -99     /* not yet implemented               */
} numx_status_t;

/* ── Shared function-pointer types ────────────────────────────────── */

/* A 1-D real-valued function y = f(x). Used by roots, integrate, diff. */
typedef numx_real_t (*numx_func1d_t)(numx_real_t x);

/* ── Epsilon and limit constants ───────────────────────────────────── */

#ifdef NUMX_USE_DOUBLE
#define NUMX_EPSILON 2.220446049250313e-16
#define NUMX_PI 3.141592653589793238462643383279502884
#else
#define NUMX_EPSILON 1.1920929e-7f
#define NUMX_PI 3.14159265358979323846f
#endif

#define NUMX_VERSION_MAJOR 0
#define NUMX_VERSION_MINOR 1
#define NUMX_VERSION_PATCH 0

#endif /* NUMX_TYPES_H */
