/*
 * Example 01 — Linear algebra: solve a 3×3 system and compute a dot product.
 *
 * Solves:
 *   2x  +  y  -  z =  8
 *  -3x  -  y  + 2z = -11
 *  -2x  +  y  + 2z = -3
 *
 * Expected solution: x = 2, y = 3, z = -1
 */

#include "numx/linalg.h"
#include <stdio.h>

int main(void)
{
    /* Row-major: A[row][col] at index row*cols + col */
    numx_real_t A[] = {  2.0f,  1.0f, -1.0f,
                        -3.0f, -1.0f,  2.0f,
                        -2.0f,  1.0f,  2.0f };
    numx_real_t b[] = {  8.0f, -11.0f, -3.0f };

    numx_real_t LU[9];
    numx_idx_t  pivot[3];
    numx_real_t x[3];
    numx_status_t s;

    s = numx_lu_decompose(A, 3, LU, pivot);
    if (s != NUMX_OK) { printf("lu_decompose failed: %d\n", (int)s); return 1; }

    s = numx_lu_solve(LU, pivot, 3, b, x);
    if (s != NUMX_OK) { printf("lu_solve failed: %d\n", (int)s); return 1; }

    printf("Solution: x=%.4f  y=%.4f  z=%.4f\n",
           (double)x[0], (double)x[1], (double)x[2]);

    /* Dot product of x with itself = ||x||^2 */
    numx_real_t dot;
    s = numx_vec_dot(x, x, 3, &dot);
    if (s != NUMX_OK) { printf("vec_dot failed: %d\n", (int)s); return 1; }

    printf("||x||^2 = %.4f  (expect 14.0000)\n", (double)dot);
    return 0;
}
