# Validation: numx_fft

Covers: `fft_f32` · `ifft_f32` · `fft_q15` · `fft_magnitude`

---

## x86-64 — Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-06-07 | **Commit:** d81b386

### Test cases

| Function | Input / scenario | Expected | Computed | Error | Pass |
|----------|-----------------|----------|----------|-------|------|
| fft_f32 | DC N=8, X[0].re | 8.0 | 8.0000000 | 0.00e+00 | ✅ |
| fft_f32 | DC N=8, X[0].im | 0.0 | 0.0000007 | 7.50e-07 | ✅ |
| fft_f32 | DC N=8, X[1].re | ≈0.0 | −0.0000002 | 1.51e-07 | ✅ |
| fft_f32 | impulse N=4, X[k].re (all bins) | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| ifft_f32 | round-trip IFFT(FFT(x))=x | x[i] | x[i] | 0.00e+00 | ✅ |
| fft_magnitude | DC N=4, mag[0] | 4.0 | 4.0000000 | 0.00e+00 | ✅ |
| fft_magnitude | DC N=4, mag[1] | ≈0.0 | 0.0000001 | 8.84e-08 | ✅ |
| fft_q15 | DC N=2, X[0].re>0 | >0 | confirmed | — | ✅ |
| fft_q15 | impulse N=2, X[0].re>0 | >0 | confirmed | — | ✅ |
| null-ptr guards | fft_f32, ifft_f32, fft_q15, magnitude | rc=-1 | rc=-1 | — | ✅ |
| invalid-arg guards | n=0, n=3 (non-pow2), n>MAX | rc=-2 | rc=-2 | — | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| fft_f32 N=64 | 10,000 | 35,824 µs | 3,582 ns |
| fft_f32 N=256 | 5,000 | 112,890 µs | 22,578 ns |
| fft_f32 N=512 | 1,000 | 54,699 µs | 54,699 ns |
| ifft_f32 N=256 | 5,000 | 112,377 µs | 22,475 ns |
| fft_q15 N=256 | 5,000 | 115,149 µs | 23,029 ns |
| fft_magnitude N=256 | 100,000 | 1,633,641 µs | 16,336 ns |

### Precision vs reference (scipy.fft)

| Scenario | Bin | scipy (float64) | numx (float32) | Error |
|----------|-----|----------------|----------------|-------|
| DC N=8 | X[0].re | 8.0 | 8.0000000 | 0.00e+00 |
| Impulse N=4 | X[k].re (all) | 1.0 | 1.0000000 | 0.00e+00 |
| Round-trip N=4 | re[0] | 3.0 | 3.0000000 | 0.00e+00 |

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

### Test cases — f32_dc

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| fft_f32 dc n=8 rc | rc=0 | rc=0 | — | ✅ |
| fft_f32 dc X[0].re=8 | 8.0 | 8.0000000 | 0.00e+00 | ✅ |
| fft_f32 dc X[0].im=0 | 0.0 | 0.0000007 | 7.50e-07 | ✅ |
| fft_f32 dc X[1].re≈0 | 0.0 | −0.0000002 | 1.51e-07 | ✅ |
| fft_f32 dc X[4].re≈0 | 0.0 | 0.0000000 | 0.00e+00 | ✅ |

### Test cases — f32_impulse

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| fft_f32 impulse rc | rc=0 | rc=0 | — | ✅ |
| fft_f32 impulse X[0]=1 | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| fft_f32 impulse X[1]=1 | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| fft_f32 impulse X[2]=1 | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| fft_f32 impulse X[3]=1 | 1.0 | 1.0000000 | 0.00e+00 | ✅ |

### Test cases — f32_roundtrip

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| ifft_f32 roundtrip rc | rc=0 | rc=0 | — | ✅ |
| roundtrip re[0] | 3.0 | 3.0000000 | 0.00e+00 | ✅ |
| roundtrip re[1] | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| roundtrip re[2] | 4.0 | 4.0000000 | 0.00e+00 | ✅ |
| roundtrip re[3] | 1.0 | 1.0000000 | 0.00e+00 | ✅ |

### Test cases — magnitude

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| magnitude dc rc | rc=0 | rc=0 | — | ✅ |
| magnitude dc mag[0]=4 | 4.0 | 4.0000000 | 0.00e+00 | ✅ |
| magnitude dc mag[1]≈0 | 0.0 | 0.0000001 | 8.84e-08 | ✅ |
| magnitude dc mag[2]≈0 | 0.0 | 0.0000001 | 1.25e-07 | ✅ |
| magnitude null-fft | rc=-1 | rc=-1 | — | ✅ |
| magnitude null-mag | rc=-1 | rc=-1 | — | ✅ |
| magnitude n=1 | rc=-2 | rc=-2 | — | ✅ |
| magnitude n=3 (non-pow2) | rc=-2 | rc=-2 | — | ✅ |

### Test cases — q15

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| fft_q15 dc n=2 rc | rc=0 | rc=0 | — | ✅ |
| fft_q15 dc X[0].re>0 | >0 | confirmed | — | ✅ |
| fft_q15 dc X[1].re=0 | 0 | confirmed | — | ✅ |
| fft_q15 impulse X[0]>0 | >0 | confirmed | — | ✅ |
| fft_q15 impulse X[1]>0 | >0 | confirmed | — | ✅ |
| fft_q15 null | rc=-1 | rc=-1 | — | ✅ |
| fft_q15 n=0 | rc=-2 | rc=-2 | — | ✅ |
| fft_q15 n=3 (non-pow2) | rc=-2 | rc=-2 | — | ✅ |

### Test cases — f32_errors

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| fft_f32 null | rc=-1 | rc=-1 | — | ✅ |
| fft_f32 n=0 | rc=-2 | rc=-2 | — | ✅ |
| fft_f32 n=3 | rc=-2 | rc=-2 | — | ✅ |
| fft_f32 n>max | rc=-2 | rc=-2 | — | ✅ |
| ifft_f32 null | rc=-1 | rc=-1 | — | ✅ |
| ifft_f32 n=0 | rc=-2 | rc=-2 | — | ✅ |
| ifft_f32 n=3 | rc=-2 | rc=-2 | — | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| fft_f32 N=64 | 5,000 | 12,908,018 µs | 2,581,603 ns |
| fft_f32 N=256 | 1,000 | 13,768,864 µs | 13,768,864 ns |
| fft_f32 N=512 | 100 | 3,110,345 µs | 31,103,450 ns |
| ifft_f32 N=256 | 1,000 | 13,836,714 µs | 13,836,714 ns |
| fft_q15 N=256 | 1,000 | 14,278,583 µs | 14,278,583 ns |
| fft_magnitude N=256 | 10,000 | 36,844,459 µs | 3,684,445 ns |

*⚠️ All FFT timings are ~1000× slower than expected. Root cause: `priv_cos`/`priv_sin` in `fft.c` call `__divsf3` (software-float division from ROM) instead of the hardware FPU, making each trig evaluation ~100 µs. This is a performance bug in the FFT implementation, not a measurement error. CPU ran at 160 MHz (benchmark header text "240 MHz" does not reflect actual clock). Task Watchdog (WDT) triggered during longer loops; timing values are valid — printed after WDT reset.*

### Precision vs reference

| Scenario | Bin | Exact | Computed | Error |
|----------|-----|-------|----------|-------|
| DC n=8 | X[0].re | 8.0 | 8.0000000 | 0.00e+00 |
| DC n=8 | X[0].im | 0.0 | 0.0000007 | 7.50e-07 |
| DC n=8 | X[1].re | 0.0 | −0.0000002 | 1.51e-07 |
| Impulse n=4 | X[k].re (all) | 1.0 | 1.0000000 | 0.00e+00 |
| Roundtrip n=4 | re[0..3] | [3,1,4,1] | exact | 0.00e+00 |
| Magnitude DC n=4 | mag[0] | 4.0 | 4.0000000 | 0.00e+00 |
| Magnitude DC n=4 | mag[1] | 0.0 | 0.0000001 | 8.84e-08 |
| Magnitude DC n=4 | mag[2] | 0.0 | 0.0000001 | 1.25e-07 |

*DC bin X[0].im error of 7.50e-07 and non-DC bin X[1].re error of 1.51e-07 are float32 trig rounding in the butterfly operations — expected. Impulse spectrum and IFFT roundtrip are exact. Magnitude near-zero bins are sub-epsilon.*

**RESULTS: 38 PASS / 0 FAIL / 38 TOTAL**

---

## Windows x64 — Windows 11 / MSVC 14.51 (VS 2026 Build Tools) / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-06-05 | **Commit:** 4c4c0f0

### Test cases

| Test | Result |
|------|--------|
| test_fft_dc_n4 | ✅ |
| test_fft_single_tone_n4 | ✅ |
| test_fft_dc_n2 | ✅ |
| test_fft_n1_passthrough | ✅ |
| test_fft_delta_at_0 | ✅ |
| test_fft_null_returns_error | ✅ |
| test_fft_n_not_power_of_two | ✅ |
| test_fft_n_exceeds_max | ✅ |
| test_ifft_roundtrip | ✅ |
| test_ifft_dc_spectrum | ✅ |
| test_ifft_null_returns_error | ✅ |
| test_fft_q15_dc_n4 | ✅ |
| test_fft_q15_null_returns_error | ✅ |
| test_fft_q15_n_not_power_of_two | ✅ |
| test_fft_magnitude_dc | ✅ |
| test_fft_magnitude_complex_bin | ✅ |
| test_fft_magnitude_null_returns_error | ✅ |
| test_fft_magnitude_n1_invalid | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| fft_f32 N=64 | 10,000 | 124,203 µs | 12,420 ns |
| fft_f32 N=256 | 5,000 | 353,729 µs | 70,745 ns |
| fft_f32 N=512 | 1,000 | 161,765 µs | 161,765 ns |
| ifft_f32 N=256 | 5,000 | 357,367 µs | 71,473 ns |
| fft_q15 N=256 | 5,000 | 346,085 µs | 69,217 ns |
| fft_magnitude N=256 | 100,000 | 2,980,047 µs | 29,800 ns |

**RESULTS: 18 PASS / 0 FAIL / 18 TOTAL**

---

## Windows x64 — Windows 11 / MSVC 14.51 (VS 2026 Build Tools) / float64
**Validator:** Amir Ab Khoshk | **Date:** 2026-06-06 | **Commit:** 1bba399

> Build note: Some Unity assertions use float32 tolerances even when built with
> NUMX_USE_DOUBLE. Affected tests still pass because the errors are well within
> float32 tolerance, but the assertion threshold does not tighten to double precision.
> This is a test harness configuration issue, not a library bug.

### Test cases

| Test | Result |
|------|--------|
| test_fft_dc_n4 | ✅ |
| test_fft_single_tone_n4 | ✅ |
| test_fft_dc_n2 | ✅ |
| test_fft_n1_passthrough | ✅ |
| test_fft_delta_at_0 | ✅ |
| test_fft_null_returns_error | ✅ |
| test_fft_n_not_power_of_two | ✅ |
| test_fft_n_exceeds_max | ✅ |
| test_ifft_roundtrip | ✅ |
| test_ifft_dc_spectrum | ✅ |
| test_ifft_null_returns_error | ✅ |
| test_fft_q15_dc_n4 | ✅ |
| test_fft_q15_null_returns_error | ✅ |
| test_fft_q15_n_not_power_of_two | ✅ |
| test_fft_magnitude_dc | ✅ |
| test_fft_magnitude_complex_bin | ✅ |
| test_fft_magnitude_null_returns_error | ✅ |
| test_fft_magnitude_n1_invalid | ✅ |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| fft_f32 N=64 | 10,000 | 238,883 µs | 23,888 ns |
| fft_f32 N=256 | 5,000 | 634,781 µs | 126,956 ns |
| fft_f32 N=512 | 1,000 | 289,183 µs | 289,183 ns |
| ifft_f32 N=256 | 5,000 | 639,834 µs | 127,966 ns |
| fft_q15 N=256 | 5,000 | 618,086 µs | 123,617 ns |
| fft_magnitude N=256 | 100,000 | 3,139,824 µs | 31,398 ns |

**RESULTS: 18 PASS / 0 FAIL / 18 TOTAL**
