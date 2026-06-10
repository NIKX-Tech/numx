/*
 * Example 03 — ODE solver: simulate a simple pendulum.
 *
 * State: y = [theta, omega]  (angle in radians, angular velocity in rad/s)
 * ODE:   d(theta)/dt = omega
 *        d(omega)/dt = -(g/L) * sin(theta)
 *
 * With g/L = 9.81 (unit pendulum), initial angle 0.2 rad, zero velocity.
 * Integrated from t=0 to t=2 s with adaptive RK45 (tol=1e-5).
 * The period of a small-angle pendulum is 2*pi ≈ 6.28 s, so at t=2 s
 * the pendulum has completed ~1/3 of its first swing.
 */

#include "numx/ode.h"
#include <stdio.h>

#define G_OVER_L 9.81f

static numx_status_t pendulum(numx_real_t t, const numx_real_t *y,
                               numx_size_t n, numx_real_t *dydt)
{
    (void)t; (void)n;
    /* sin approximation: sin(x) ≈ x - x^3/6 + x^5/120 (good for |x| < 0.5) */
    numx_real_t th = y[0];
    numx_real_t th3 = th * th * th;
    numx_real_t th5 = th3 * th * th;
    numx_real_t sin_th = th - th3 / 6.0f + th5 / 120.0f;

    dydt[0] =  y[1];
    dydt[1] = -G_OVER_L * sin_th;
    return NUMX_OK;
}

int main(void)
{
    numx_real_t y0[2] = { 0.2f, 0.0f };
    numx_real_t y[2];
    numx_status_t s;

    s = numx_ode_rk45(pendulum, 0.0f, 2.0f, y0, 2, 1e-5f, y);
    if (s != NUMX_OK) { printf("rk45 failed: %d\n", (int)s); return 1; }

    printf("t=2.0 s:  theta=%.6f rad  omega=%.6f rad/s\n",
           (double)y[0], (double)y[1]);
    /* Period T=2pi/sqrt(g/L)=2pi/sqrt(9.81)≈2.007 s, so t=2.0 s is ~one full
     * oscillation: theta returns near 0.2, omega near 0. */
    printf("(expect theta≈0.200 rad, omega≈0.022 rad/s — near full period)\n");
    return 0;
}
