/**
 * @file numx_config.h
 * @brief Compile-time configuration for numx.
 *
 * Override any of these by passing -DNUMX_<NAME>=<value> to the compiler,
 * or by defining them before including any numx header.
 * All limits determine static array sizes — no dynamic allocation occurs.
 */

#ifndef NUMX_CONFIG_H
#define NUMX_CONFIG_H

/* ── Precision ─────────────────────────────────────────────────────── */

/* Uncomment to switch the entire library to float64. */
/* #define NUMX_USE_DOUBLE */

/* ── Linear algebra ────────────────────────────────────────────────── */

#ifndef NUMX_MAX_VEC_SIZE
#define NUMX_MAX_VEC_SIZE 256 /* max elements in a vector          */
#endif

#ifndef NUMX_MAX_MAT_ROWS
#define NUMX_MAX_MAT_ROWS 32 /* max rows in a matrix              */
#endif

#ifndef NUMX_MAX_MAT_COLS
#define NUMX_MAX_MAT_COLS 32 /* max cols in a matrix              */
#endif

/* ── Polynomial ────────────────────────────────────────────────────── */

#ifndef NUMX_MAX_POLY_DEGREE
#define NUMX_MAX_POLY_DEGREE 32 /* max degree of polynomial          */
#endif

/* ── Interpolation ─────────────────────────────────────────────────── */

#ifndef NUMX_MAX_INTERP_NODES
#define NUMX_MAX_INTERP_NODES 128 /* max nodes for spline/Chebyshev    */
#endif

/* ── ODE solver ────────────────────────────────────────────────────── */

#ifndef NUMX_MAX_ODE_DIM
#define NUMX_MAX_ODE_DIM 16 /* max state dimension for ODE       */
#endif

/* ── Signal processing ─────────────────────────────────────────────── */

#ifndef NUMX_MAX_FFT_SIZE
#define NUMX_MAX_FFT_SIZE 512  /* max FFT length (must be power-of-2) */
#endif

#ifndef NUMX_MAX_FIR_TAPS
#define NUMX_MAX_FIR_TAPS 128 /* max FIR filter tap count          */
#endif

/* ── Iterative solvers ─────────────────────────────────────────────── */

#ifndef NUMX_MAX_ITER
#define NUMX_MAX_ITER 1000 /* max iterations for root/ODE/CS    */
#endif

/* ── Compressed sensing ────────────────────────────────────────────── */

#ifndef NUMX_MAX_CS_MEASUREMENTS
#define NUMX_MAX_CS_MEASUREMENTS 256 /* max rows of A (measurements)      */
#endif

#ifndef NUMX_MAX_CS_SIGNAL_DIM
#define NUMX_MAX_CS_SIGNAL_DIM 256   /* max cols of A (signal dimension)  */
#endif

#ifndef NUMX_MAX_CS_SPARSITY
#define NUMX_MAX_CS_SPARSITY 32      /* max sparsity level for OMP        */
#endif

/* ── Randomized sketch ─────────────────────────────────────────────── */

#ifndef NUMX_MAX_SKETCH_M
#define NUMX_MAX_SKETCH_M 64    /* max rows of input matrix              */
#endif

#ifndef NUMX_MAX_SKETCH_N
#define NUMX_MAX_SKETCH_N 64    /* max cols of input matrix              */
#endif

#ifndef NUMX_MAX_SKETCH_RANK
#define NUMX_MAX_SKETCH_RANK 16 /* max (rank + oversample)               */
#endif

/* ── Automatic differentiation ─────────────────────────────────────── */

#ifndef NUMX_MAX_AD_TAPE_LEN
#define NUMX_MAX_AD_TAPE_LEN 4096 /* max Wengert tape entries          */
#endif

/* ── NTT (post-quantum) ────────────────────────────────────────────── */

#ifndef NUMX_NTT_N
#define NUMX_NTT_N 256  /* CRYSTALS-Kyber/Dilithium ring dim */
#endif

#ifndef NUMX_NTT_Q
#define NUMX_NTT_Q 3329 /* NTT prime modulus (q-1 = 2^8 * 13) */
#endif

#endif /* NUMX_CONFIG_H */
