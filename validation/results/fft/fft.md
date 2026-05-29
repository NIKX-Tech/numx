# Validation: numx_fft

Covers: `fft_f32` · `ifft_f32` · `fft_q15` · `fft_magnitude`

---

## x86-64 — Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 / float32
**Validator:** — | **Date:** — | **Commit:** —

### Test cases

| Function | Input / scenario | Expected | Computed | Error | Pass |
|----------|-----------------|----------|----------|-------|------|
| fft_f32 | DC N=8, X[0].re | 8.0 | — | — | — |
| fft_f32 | DC N=8, X[0].im | 0.0 | — | — | — |
| fft_f32 | DC N=8, X[1].re | ≈0.0 | — | — | — |
| fft_f32 | impulse N=4, X[k].re (all bins) | 1.0 | — | — | — |
| ifft_f32 | round-trip IFFT(FFT(x))=x | x[i] | — | — | — |
| fft_magnitude | DC N=4, mag[0] | 4.0 | — | — | — |
| fft_magnitude | DC N=4, mag[1] | ≈0.0 | — | — | — |
| fft_q15 | DC N=2, X[0].re>0 | >0 | — | — | — |
| fft_q15 | impulse N=2, X[0].re>0 | >0 | — | — | — |
| null-ptr guards | fft_f32, ifft_f32, fft_q15, magnitude | rc=-1 | — | — | — |
| invalid-arg guards | n=0, n=3 (non-pow2), n>MAX | rc=-2 | — | — | — |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| fft_f32 N=64 | — | — | — |
| fft_f32 N=256 | — | — | — |
| fft_f32 N=512 | — | — | — |
| ifft_f32 N=256 | — | — | — |
| fft_q15 N=256 | — | — | — |
| fft_magnitude N=256 | — | — | — |

### Precision vs reference (scipy.fft)

| Scenario | Bin | scipy (float64) | numx (float32) | Error |
|----------|-----|----------------|----------------|-------|
| DC N=8 | X[0].re | 8.0 | — | — |
| Impulse N=4 | X[k].re (all) | 1.0 | — | — |
| Round-trip N=4 | re[0] | 3.0 | — | — |

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

## ESP32-S3 — ESP-IDF v5.5.x / Xtensa LX7 / xtensa-esp32s3-elf-gcc / float32
**Validator:** — | **Date:** — | **Commit:** —

### Test cases

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| fft_f32 dc n=8 rc | rc=0 | — | — | — |
| fft_f32 dc X[0].re=8 | 8.0 | — | — | — |
| fft_f32 dc X[0].im=0 | 0.0 | — | — | — |
| fft_f32 dc X[1].re≈0 | 0.0 | — | — | — |
| fft_f32 impulse X[0]=1 | 1.0 | — | — | — |
| fft_f32 impulse X[1]=1 | 1.0 | — | — | — |
| ifft_f32 roundtrip rc | rc=0 | — | — | — |
| roundtrip re[0] | 3.0 | — | — | — |
| roundtrip re[1] | 1.0 | — | — | — |
| magnitude dc rc | rc=0 | — | — | — |
| magnitude dc mag[0]=4 | 4.0 | — | — | — |
| magnitude dc mag[1]≈0 | 0.0 | — | — | — |
| fft_q15 dc n=2 rc | rc=0 | — | — | — |
| fft_q15 dc X[0].re>0 | >0 | — | — | — |
| fft_q15 dc X[1].re=0 | 0 | — | — | — |
| fft_q15 impulse X[0]>0 | >0 | — | — | — |
| null-ptr guards | rc=-1 | — | — | — |
| invalid-arg guards | rc=-2 | — | — | — |

**RESULTS: — PASS / — FAIL / — TOTAL**
