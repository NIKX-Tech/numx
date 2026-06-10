/*
 * Example 05 — Compressed sensing: exact sparse recovery via OMP.
 *
 * A 2-sparse signal of length n=8 is compressed to m=4 measurements
 * (2× compression).  OMP recovers it exactly.
 *
 * The sensing matrix A has unit-norm columns; the two active columns
 * (0 and 1) are standard basis vectors so that OMP is guaranteed to
 * select the correct support regardless of the other column values.
 * Remaining columns have near-zero first two rows and unit L2 norm.
 */

#include "numx/compressed_sensing.h"
#include <stdio.h>

#define M 4
#define N 8

int main(void)
{
    /* 4×8 sensing matrix — columns are unit-norm, column 0 = e0, column 1 = e1 */
    static const numx_real_t A[M * N] = {
        /* col: 0      1      2      3      4      5      6      7    */
           1.0f,  0.0f,  0.03f, -0.02f,  0.04f, -0.01f,  0.02f,  0.03f,
           0.0f,  1.0f,  0.04f,  0.03f, -0.02f,  0.04f, -0.03f,  0.02f,
           0.0f,  0.0f,  0.700f, 0.712f,  0.650f, 0.726f,  0.688f, 0.707f,
           0.0f,  0.0f,  0.713f, 0.701f,  0.759f, 0.687f,  0.725f, 0.706f,
    };

    /* 2-sparse: nonzero at indices 0 and 1 */
    numx_real_t x_true[N] = { 3.0f, -1.5f, 0, 0, 0, 0, 0, 0 };

    /* Compute measurements y = A * x_true */
    numx_real_t y[M] = { 0.0f };
    numx_size_t i, j;
    for (i = 0; i < M; i++) {
        for (j = 0; j < N; j++) {
            y[i] += A[i * N + j] * x_true[j];
        }
    }

    /* Recover x via OMP with sparsity k=2 */
    numx_real_t x_rec[N] = { 0.0f };
    numx_status_t s = numx_cs_omp(A, y, M, N, 2, x_rec);
    if (s != NUMX_OK) { printf("OMP failed: %d\n", (int)s); return 1; }

    printf("Index  True     Recovered\n");
    for (i = 0; i < N; i++) {
        numx_real_t diff = x_rec[i] - x_true[i];
        if (diff < 0.0f) diff = -diff;
        if (x_true[i] != 0.0f || diff > 0.01f) {
            printf("  %u    %6.3f    %6.3f\n",
                   (unsigned)i, (double)x_true[i], (double)x_rec[i]);
        }
    }
    printf("(OMP recovers exactly when the sensing matrix has low mutual coherence)\n");
    return 0;
}
