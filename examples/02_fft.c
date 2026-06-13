/*
 * Example 02 — FFT: detect the dominant frequency in a synthetic signal.
 *
 * Constructs a 64-point real signal containing a 4 Hz tone (bin 4 at a
 * sample rate of 64 Hz), runs the FFT, computes the magnitude spectrum,
 * and identifies the peak bin.
 *
 * Note: signal generation uses cos() from <math.h>. The numx FFT itself has
 * no such dependency; this is normal user code wrapping the library.
 */

#include "numx/fft.h"
#include "numx/numx.h"
#include <math.h>
#include <stdio.h>

#define N 64

int main(void)
{
    /* Interleaved complex buffer: [re0, im0, re1, im1, ...] */
    numx_real_t buf[2 * N];
    numx_real_t mag[N / 2 + 1];
    numx_status_t s;
    numx_size_t i;

    /* cos(2*pi * 4 * n / N): 4 complete cycles over 64 samples */
    for (i = 0; i < N; i++) {
        buf[2 * i]     = (numx_real_t)cos(2.0 * NUMX_PI * 4.0 * i / N);
        buf[2 * i + 1] = 0.0f;
    }

    s = numx_fft_f32(buf, N);
    if (s != NUMX_OK) { printf("fft_f32 failed: %d\n", (int)s); return 1; }

    s = numx_fft_magnitude(buf, N, mag);
    if (s != NUMX_OK) { printf("fft_magnitude failed: %d\n", (int)s); return 1; }

    /* Find peak bin (DC excluded) */
    numx_size_t peak_bin = 1;
    numx_real_t peak_val = mag[1];
    for (i = 2; i <= N / 2; i++) {
        if (mag[i] > peak_val) { peak_val = mag[i]; peak_bin = i; }
    }

    printf("Peak bin: %u  magnitude: %.2f  (expect bin 4, magnitude ~32)\n",
           (unsigned)peak_bin, (double)peak_val);
    return 0;
}
