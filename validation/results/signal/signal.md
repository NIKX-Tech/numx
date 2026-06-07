# Validation: numx_signal

Covers: `window_rect` · `window_hann` · `window_hamming` · `window_blackman` ·
`convolve` · `correlate` · `fir` · `iir_biquad` · `peaks` · `ema`

---

## x86-64 — Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-06-07 | **Commit:** d81b386

### Test cases

| Function | Input / scenario | Expected | Computed | Error | Pass |
|----------|-----------------|----------|----------|-------|------|
| window_rect | n=4, all samples | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| window_hann | n=5, endpoint w[0] | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| window_hann | n=5, peak w[2] | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| window_hamming | n=5, w[0] | 0.08 | 0.0800000 | 1.49e-08 | ✅ |
| window_hamming | n=5, peak w[2] | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| window_blackman | n=5, w[0] | 0.0 | −0.0000000 | 1.49e-08 | ✅ |
| window_blackman | n=5, peak w[2] | 1.0 | 0.9999999 | 5.96e-08 | ✅ |
| convolve | [1,0,0] * [1,2,3] → [1,2,3,0,0] | see array | [1,2,3,0,0] | 0.00e+00 | ✅ |
| correlate | auto-correlation peak at lag 0 | 14.0 | 14.0000000 | 0.00e+00 | ✅ |
| fir | identity tap [1] → pass-through | x[i] | x[i] | 0.00e+00 | ✅ |
| fir | box [0.5,0.5], out[1] | 1.5 | 1.5000000 | 0.00e+00 | ✅ |
| iir_biquad | all-pass b=[1,0,0] a=[0,0] | x[i] | x[i] | 0.00e+00 | ✅ |
| peaks | [1,3,1,2,1] → count | 2 | 2 | — | ✅ |
| peaks | monotone [1,2,3,4] → count | 0 | 0 | — | ✅ |
| ema | alpha=1 → identity | x[i] | x[i] | 0.00e+00 | ✅ |
| ema | alpha=0 → constant | x[0] | x[0] | 0.00e+00 | ✅ |
| ema | alpha=0.5, [1,2,3] → [1,1.5,2.25] | 2.25 | 2.2500000 | 0.00e+00 | ✅ |
| null-ptr guards | all functions | rc=-1 | rc=-1 | — | ✅ |
| invalid-arg guards | n=0, alpha out of [0,1] | rc=-2 | rc=-2 | — | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| window_hann n=512 | 100,000 | 905,816 µs | 9,058 ns |
| convolve xn=256 hn=32 | 10,000 | 32,414 µs | 3,241 ns |
| fir xn=256 ntaps=32 | 10,000 | 30,527 µs | 3,052 ns |
| iir_biquad n=256 | 50,000 | 25,419 µs | 508 ns |
| ema n=256 | 50,000 | 15,923 µs | 318 ns |

### Precision vs reference

| Function | Reference | numx (float32) | Error |
|----------|-----------|----------------|-------|
| window_hann peak | 1.0 | 1.0000000 | 0.00e+00 |
| convolve [1,1]*[1,1] centre | 2.0 | 2.0000000 | 0.00e+00 |
| ema alpha=0.5 final | 2.25 | 2.2500000 | 0.00e+00 |

---

## ARM64 — macOS / Apple M4 Pro / Apple clang / float32
**Validator:** — | **Date:** — | **Commit:** —

### Test cases

| Function | Input / scenario | Expected | Computed | Error | Pass |
|----------|-----------------|----------|----------|-------|------|
| *(awaiting results)* | | | | | |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| *(awaiting results)* | | | |

---

## ESP32-S3 — ESP-IDF v5.5.2 / Xtensa LX7 / xtensa-esp32s3-elf-gcc / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-29 | **Commit:** d81b386

### Test cases — window_rect

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| rect n=4 rc | rc=0 | rc=0 | — | ✅ |
| rect w[0]=1 | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| rect w[1]=1 | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| rect w[3]=1 | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| rect n=1 rc | rc=0 | rc=0 | — | ✅ |
| rect n=1 w[0]=1 | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| rect null | rc=-1 | rc=-1 | — | ✅ |
| rect n=0 | rc=-2 | rc=-2 | — | ✅ |

### Test cases — window_hann

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| hann n=5 rc | rc=0 | rc=0 | — | ✅ |
| hann w[0]=0 (left) | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| hann w[4]=0 (right) | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| hann w[2]=1 (peak) | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| hann symmetric w[1]=w[3] | equal | 0.5000001 | 0.00e+00 | ✅ |
| hann null | rc=-1 | rc=-1 | — | ✅ |
| hann n=0 | rc=-2 | rc=-2 | — | ✅ |

### Test cases — window_hamming

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| hamming n=5 rc | rc=0 | rc=0 | — | ✅ |
| hamming w[0]=0.08 | 0.08 | 0.0800000 | 1.49e-08 | ✅ |
| hamming w[2]=1 (peak) | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| hamming symmetric w[1]=w[3] | equal | 0.5400000 | 5.96e-08 | ✅ |
| hamming null | rc=-1 | rc=-1 | — | ✅ |
| hamming n=0 | rc=-2 | rc=-2 | — | ✅ |

### Test cases — window_blackman

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| blackman n=5 rc | rc=0 | rc=0 | — | ✅ |
| blackman w[0]≈0 | 0.0 | −0.0000000 | 1.49e-08 | ✅ |
| blackman w[2]=1 (peak) | 1.0 | 0.9999999 | 5.96e-08 | ✅ |
| blackman symmetric w[1]=w[3] | equal | 0.3400000 | 0.00e+00 | ✅ |
| blackman null | rc=-1 | rc=-1 | — | ✅ |
| blackman n=0 | rc=-2 | rc=-2 | — | ✅ |

### Test cases — convolve

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| convolve impulse rc | rc=0 | rc=0 | — | ✅ |
| convolve out[0]=1 | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| convolve out[1]=2 | 2.0 | 2.0000000 | 0.00e+00 | ✅ |
| convolve out[2]=3 | 3.0 | 3.0000000 | 0.00e+00 | ✅ |
| convolve out[3]=0 | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| convolve out[4]=0 | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| convolve [1,1]*[1,1] out[1]=2 | 2.0 | 2.0000000 | 0.00e+00 | ✅ |
| convolve null-x | rc=-1 | rc=-1 | — | ✅ |
| convolve null-h | rc=-1 | rc=-1 | — | ✅ |
| convolve null-out | rc=-1 | rc=-1 | — | ✅ |
| convolve xn=0 | rc=-2 | rc=-2 | — | ✅ |
| convolve hn=0 | rc=-2 | rc=-2 | — | ✅ |

### Test cases — correlate

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| correlate auto rc | rc=0 | rc=0 | — | ✅ |
| correlate auto peak=14 | 14.0 | 14.0000000 | 0.00e+00 | ✅ |
| correlate auto symmetric | 8.0 | 8.0000000 | 0.00e+00 | ✅ |
| correlate null-x | rc=-1 | rc=-1 | — | ✅ |
| correlate null-y | rc=-1 | rc=-1 | — | ✅ |
| correlate null-out | rc=-1 | rc=-1 | — | ✅ |
| correlate xn=0 | rc=-2 | rc=-2 | — | ✅ |

### Test cases — fir

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| fir identity rc | rc=0 | rc=0 | — | ✅ |
| fir identity out[0]=1 | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| fir identity out[2]=3 | 3.0 | 3.0000000 | 0.00e+00 | ✅ |
| fir box out[1]=1.5 | 1.5 | 1.5000000 | 0.00e+00 | ✅ |
| fir null-x | rc=-1 | rc=-1 | — | ✅ |
| fir null-taps | rc=-1 | rc=-1 | — | ✅ |
| fir null-out | rc=-1 | rc=-1 | — | ✅ |
| fir xn=0 | rc=-2 | rc=-2 | — | ✅ |
| fir ntaps=0 | rc=-2 | rc=-2 | — | ✅ |

### Test cases — iir_biquad

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| biquad all-pass rc | rc=0 | rc=0 | — | ✅ |
| biquad all-pass out[0]=1 | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| biquad all-pass out[2]=3 | 3.0 | 3.0000000 | 0.00e+00 | ✅ |
| biquad null-x | rc=-1 | rc=-1 | — | ✅ |
| biquad null-b | rc=-1 | rc=-1 | — | ✅ |
| biquad null-a | rc=-1 | rc=-1 | — | ✅ |
| biquad null-out | rc=-1 | rc=-1 | — | ✅ |
| biquad n=0 | rc=-2 | rc=-2 | — | ✅ |

### Test cases — peaks

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| peaks 2-peak rc | rc=0 | rc=0 | — | ✅ |
| peaks count=2 | 2 | 2 | — | ✅ |
| peaks pk[0]=1 | idx=1 | idx=1 | — | ✅ |
| peaks pk[1]=3 | idx=3 | idx=3 | — | ✅ |
| peaks monotone=0 | 0 | 0 | — | ✅ |
| peaks n=2 → 0 | 0 | 0 | — | ✅ |
| peaks null-x | rc=-1 | rc=-1 | — | ✅ |
| peaks null-peaks | rc=-1 | rc=-1 | — | ✅ |
| peaks null-npeaks | rc=-1 | rc=-1 | — | ✅ |

### Test cases — ema

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| ema alpha=1 rc | rc=0 | rc=0 | — | ✅ |
| ema alpha=1 out[0]=1 | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| ema alpha=1 out[1]=3 | 3.0 | 3.0000000 | 0.00e+00 | ✅ |
| ema alpha=1 out[2]=2 | 2.0 | 2.0000000 | 0.00e+00 | ✅ |
| ema alpha=0 out[1]=1 | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| ema alpha=0 out[2]=1 | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| ema alpha=0.5 out[0]=1.0 | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| ema alpha=0.5 out[1]=1.5 | 1.5 | 1.5000000 | 0.00e+00 | ✅ |
| ema alpha=0.5 out[2]=2.25 | 2.25 | 2.2500000 | 0.00e+00 | ✅ |
| ema null-x | rc=-1 | rc=-1 | — | ✅ |
| ema null-out | rc=-1 | rc=-1 | — | ✅ |
| ema n=0 | rc=-2 | rc=-2 | — | ✅ |
| ema alpha<0 | rc=-2 | rc=-2 | — | ✅ |
| ema alpha>1 | rc=-2 | rc=-2 | — | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| signal_window_hann n=64 | 10,000 | 4,520,091 µs | 452,009 ns |
| signal_convolve x=64 h=8 | 1,000 | 56,642 µs | 56,642 ns |
| signal_correlate x=64 y=8 | 1,000 | 63,059 µs | 63,059 ns |
| signal_fir x=128 ntaps=8 | 1,000 | 123,815 µs | 123,815 ns |
| signal_iir_biquad n=128 | 5,000 | 161,501 µs | 32,300 ns |
| signal_peaks n=64 | 5,000 | 45,493 µs | 9,098 ns |
| signal_ema n=128 alpha=0.1 | 5,000 | 89,029 µs | 17,805 ns |

*⚠️ `signal_window_hann` takes 452 µs/call (vs 3.5 ns on x86) because `window_hann` calls `priv_cos` which uses `__divsf3` (software-float ROM division) — same root cause as the FFT trig issue. All other signal functions use only arithmetic and run at expected embedded speeds.*

### Precision vs reference

| Function | Input | Exact | Computed | Error |
|----------|-------|-------|----------|-------|
| window_hamming | w[0], n=5 | 0.08 | 0.0800000 | 1.49e-08 |
| window_hamming | w[1] (symm), n=5 | 0.54 | 0.5400000 | 5.96e-08 |
| window_blackman | w[0], n=5 | 0.0 | −0.0000000 | 1.49e-08 |
| window_blackman | w[2] (peak), n=5 | 1.0 | 0.9999999 | 5.96e-08 |
| correlate | auto-corr peak | 14.0 | 14.0000000 | 0.00e+00 |
| ema | alpha=0.5, out[2] | 2.25 | 2.2500000 | 0.00e+00 |

*Window function errors (1.49e-08 – 5.96e-08) are sub-epsilon float32 trig rounding — expected. Blackman w[0] computes as −0.0 in float32 due to cancellation in 0.42−0.5+0.08; equivalent to 0 for all practical purposes.*

**RESULTS: 86 PASS / 0 FAIL / 86 TOTAL**

---

## Windows x64 — Windows 11 / MSVC 14.51 (VS 2026 Build Tools) / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-06-05 | **Commit:** 4c4c0f0

### Test cases

| Test | Result |
|------|--------|
| test_window_rect_all_ones | ✅ |
| test_window_rect_n1 | ✅ |
| test_window_hann_endpoints_zero | ✅ |
| test_window_hann_midpoint_one | ✅ |
| test_window_hann_n1_returns_one | ✅ |
| test_window_hamming_endpoints | ✅ |
| test_window_hamming_midpoint_one | ✅ |
| test_window_blackman_endpoints_zero | ✅ |
| test_window_blackman_midpoint_one | ✅ |
| test_convolve_box | ✅ |
| test_convolve_unit_impulse | ✅ |
| test_convolve_null_returns_error | ✅ |
| test_correlate_autocorr_peak_at_lag0 | ✅ |
| test_correlate_output_length | ✅ |
| test_fir_identity_tap | ✅ |
| test_fir_moving_average | ✅ |
| test_fir_null_returns_error | ✅ |
| test_iir_biquad_identity | ✅ |
| test_iir_biquad_scale | ✅ |
| test_iir_biquad_null_returns_error | ✅ |
| test_peaks_two_peaks | ✅ |
| test_peaks_monotone_no_peaks | ✅ |
| test_peaks_short_signal_no_peaks | ✅ |
| test_peaks_buffer_too_small | ✅ |
| test_ema_alpha1_copies_input | ✅ |
| test_ema_alpha0_stays_at_first | ✅ |
| test_ema_known_sequence | ✅ |
| test_ema_invalid_alpha_returns_error | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| signal_window_hann n=64 | 100,000 | 347,909 µs | 3,479 ns |
| signal_convolve x=64 h=8 | 10,000 | 8,551 µs | 855 ns |
| signal_correlate x=64 y=8 | 10,000 | 8,410 µs | 841 ns |
| signal_fir x=128 ntaps=8 | 10,000 | 13,235 µs | 1,323 ns |
| signal_iir_biquad n=128 | 50,000 | 26,819 µs | 536 ns |
| signal_peaks n=64 | 50,000 | 3,828 µs | 76 ns |
| signal_ema n=128 alpha=0.1 | 50,000 | 14,604 µs | 292 ns |

**RESULTS: 28 PASS / 0 FAIL / 28 TOTAL**

---

## Windows x64 — Windows 11 / MSVC 14.51 (VS 2026 Build Tools) / float64
**Validator:** Amir Ab Khoshk | **Date:** 2026-06-06 | **Commit:** 1bba399

### Test cases

| Test | Result |
|------|--------|
| test_window_rect_all_ones | ✅ |
| test_window_rect_n1 | ✅ |
| test_window_hann_endpoints_zero | ✅ |
| test_window_hann_midpoint_one | ✅ |
| test_window_hann_n1_returns_one | ✅ |
| test_window_hamming_endpoints | ✅ |
| test_window_hamming_midpoint_one | ✅ |
| test_window_blackman_endpoints_zero | ✅ |
| test_window_blackman_midpoint_one | ✅ |
| test_convolve_box | ✅ |
| test_convolve_unit_impulse | ✅ |
| test_convolve_null_returns_error | ✅ |
| test_correlate_autocorr_peak_at_lag0 | ✅ |
| test_correlate_output_length | ✅ |
| test_fir_identity_tap | ✅ |
| test_fir_moving_average | ✅ |
| test_fir_null_returns_error | ✅ |
| test_iir_biquad_identity | ✅ |
| test_iir_biquad_scale | ✅ |
| test_iir_biquad_null_returns_error | ✅ |
| test_peaks_two_peaks | ✅ |
| test_peaks_monotone_no_peaks | ✅ |
| test_peaks_short_signal_no_peaks | ✅ |
| test_peaks_buffer_too_small | ✅ |
| test_ema_alpha1_copies_input | ✅ |
| test_ema_alpha0_stays_at_first | ✅ |
| test_ema_known_sequence | ✅ |
| test_ema_invalid_alpha_returns_error | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| signal_window_hann n=64 | 100,000 | 453,166 µs | 4,531 ns |
| signal_convolve x=64 h=8 | 10,000 | 6,688 µs | 668 ns |
| signal_correlate x=64 y=8 | 10,000 | 7,345 µs | 734 ns |
| signal_fir x=128 ntaps=8 | 10,000 | 13,449 µs | 1,344 ns |
| signal_iir_biquad n=128 | 50,000 | 26,058 µs | 521 ns |
| signal_peaks n=64 | 50,000 | 3,574 µs | 71 ns |
| signal_ema n=128 alpha=0.1 | 50,000 | 15,187 µs | 303 ns |

**RESULTS: 28 PASS / 0 FAIL / 28 TOTAL**
