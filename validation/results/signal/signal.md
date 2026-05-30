# Validation: numx_signal

Covers: `window_rect` · `window_hann` · `window_hamming` · `window_blackman` ·
`convolve` · `correlate` · `fir` · `iir_biquad` · `peaks` · `ema`

---

## x86-64 — Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 / float32
**Validator:** — | **Date:** — | **Commit:** —

### Test cases

| Function | Input / scenario | Expected | Computed | Error | Pass |
|----------|-----------------|----------|----------|-------|------|
| window_rect | n=4, all samples | 1.0 | — | — | — |
| window_hann | n=5, endpoint w[0] | 0.0 | — | — | — |
| window_hann | n=5, peak w[2] | 1.0 | — | — | — |
| window_hamming | n=5, w[0] | 0.08 | — | — | — |
| window_hamming | n=5, peak w[2] | 1.0 | — | — | — |
| window_blackman | n=5, w[0] | 0.0 | — | — | — |
| window_blackman | n=5, peak w[2] | 1.0 | — | — | — |
| convolve | [1,0,0] * [1,2,3] → [1,2,3,0,0] | see array | — | — | — |
| correlate | auto-correlation peak at lag 0 | 14.0 | — | — | — |
| fir | identity tap [1] → pass-through | x[i] | — | — | — |
| fir | box [0.5,0.5], out[1] | 1.5 | — | — | — |
| iir_biquad | all-pass b=[1,0,0] a=[0,0] | x[i] | — | — | — |
| peaks | [1,3,1,2,1] → count | 2 | — | — | — |
| peaks | monotone [1,2,3,4] → count | 0 | — | — | — |
| ema | alpha=1 → identity | x[i] | — | — | — |
| ema | alpha=0 → constant | x[0] | — | — | — |
| ema | alpha=0.5, [1,2,3] → [1,1.5,2.25] | 2.25 | — | — | — |
| null-ptr guards | all functions | rc=-1 | — | — | — |
| invalid-arg guards | n=0, alpha out of [0,1] | rc=-2 | — | — | — |

### Performance

| Function | N | Total | Per call |
|----------|---|-------|----------|
| window_hann n=512 | — | — | — |
| convolve xn=256 hn=32 | — | — | — |
| fir xn=256 ntaps=32 | — | — | — |
| iir_biquad n=256 | — | — | — |
| ema n=256 | — | — | — |

### Precision vs reference

| Function | Reference | numx (float32) | Error |
|----------|-----------|----------------|-------|
| window_hann peak | 1.0 | — | — |
| convolve [1,1]*[1,1] centre | 2.0 | — | — |
| ema alpha=0.5 final | 2.25 | — | — |

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
| rect n=4 rc | rc=0 | — | — | — |
| rect w[0]=1 | 1.0 | — | — | — |
| hann n=5 rc | rc=0 | — | — | — |
| hann w[0]=0 | 0.0 | — | — | — |
| hann w[2]=1 | 1.0 | — | — | — |
| hann symmetric | w[1]=w[3] | — | — | — |
| hamming w[0]=0.08 | 0.08 | — | — | — |
| hamming center=1 | 1.0 | — | — | — |
| blackman w[0]≈0 | 0.0 | — | — | — |
| blackman center=1 | 1.0 | — | — | — |
| convolve impulse rc | rc=0 | — | — | — |
| convolve out[0]=1 | 1.0 | — | — | — |
| convolve out[1]=2 | 2.0 | — | — | — |
| correlate auto peak=14 | 14.0 | — | — | — |
| fir identity out[0]=1 | 1.0 | — | — | — |
| fir box out[1]=1.5 | 1.5 | — | — | — |
| biquad all-pass [0]=1 | 1.0 | — | — | — |
| peaks count=2 | 2 | — | — | — |
| peaks monotone=0 | 0 | — | — | — |
| ema alpha=1 identity | x[i] | — | — | — |
| ema alpha=0.5 [2]=2.25 | 2.25 | — | — | — |
| null-ptr guards | rc=-1 | — | — | — |
| invalid-arg guards | rc=-2 | — | — | — |

**RESULTS: — PASS / — FAIL / — TOTAL**
