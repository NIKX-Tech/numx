/*
 * Example 06 — NTT: polynomial multiplication in the CRYSTALS-Kyber ring.
 *
 * Multiplies two small polynomials in Z_3329[x]/(x^256+1) via the Number
 * Theoretic Transform, then demonstrates the ring's negacyclic wraparound
 * (x^255 * x = x^256 = -1), the defining property that distinguishes this
 * ring from ordinary polynomial multiplication.
 *
 * No floating-point, no dynamic allocation, no external dependencies.
 */

#include "numx/ntt.h"
#include "numx/numx.h"
#include <stdio.h>

int main(void)
{
       numx_q15_t f[NUMX_NTT_N] = {0}; /* f(x) = 1 + 2x           */
       numx_q15_t g[NUMX_NTT_N] = {0}; /* g(x) = 3 + x             */
       numx_q15_t h[NUMX_NTT_N];       /* h = f * g                */
       numx_status_t s;

       f[0] = 1;
       f[1] = 2;
       g[0] = 3;
       g[1] = 1;

       s = numx_ntt_polymul(f, g, h);
       if (s != NUMX_OK)
       {
              printf("polymul failed: %d\n", (int)s);
              return 1;
       }

       /* (1+2x)(3+x) = 3 + 7x + 2x^2 */
       printf("(1+2x)(3+x) = %d + %dx + %dx^2  (expect 3 + 7x + 2x^2)\n",
              h[0], h[1], h[2]);

       /* Negacyclic wraparound: x^255 * x = x^256 = -1 = q-1 in this ring. */
       numx_q15_t a[NUMX_NTT_N] = {0}, b[NUMX_NTT_N] = {0}, w[NUMX_NTT_N];
       a[255] = 1;
       b[1] = 1;

       s = numx_ntt_polymul(a, b, w);
       if (s != NUMX_OK)
       {
              printf("polymul failed: %d\n", (int)s);
              return 1;
       }

       printf("x^255 * x = %d  (expect %d, i.e. -1 mod %d)\n",
              w[0], NUMX_NTT_Q - 1, NUMX_NTT_Q);

       return 0;
}
