# Validation: numx_autodiff

Covers: **forward-mode** (`numx_dual_t` — const, var, add, sub, mul, div, neg, sin, cos, exp, log, sqrt) ·
**reverse-mode** Wengert tape (`numx_ad_init`, `numx_ad_var`, `numx_ad_add/sub/mul/div/neg/sin/cos/exp/log/sqrt`, `numx_ad_backward`)

---

## x86-64 — Ubuntu 22.04 / Intel i7-13700H / gcc 11.4.0 / float32
**Validator:** — | **Date:** — | **Commit:** —

### Forward-mode test cases

| Expression | x | f(x) expected | f′(x) expected | f(x) computed | f′(x) computed | Pass |
|-----------|---|--------------|---------------|--------------|---------------|------|
| d/dx(x+3) at x=2 | 2 | 5.0 | 1.0 | — | — | — |
| d/dx(x−3) at x=5 | 5 | 2.0 | 1.0 | — | — | — |
| d/dx(x²) at x=3 | 3 | 9.0 | 6.0 | — | — | — |
| d/dx(x/3) at x=6 | 6 | 2.0 | 0.3333 | — | — | — |
| d/dx(−x) at x=3 | 3 | −3.0 | −1.0 | — | — | — |
| d/dx(sin x) at x=0 | 0 | 0.0 | 1.0 | — | — | — |
| d/dx(cos x) at x=0 | 0 | 1.0 | 0.0 | — | — | — |
| d/dx(exp x) at x=0 | 0 | 1.0 | 1.0 | — | — | — |
| d/dx(log x) at x=1 | 1 | 0.0 | 1.0 | — | — | — |
| d/dx(√x) at x=4 | 4 | 2.0 | 0.25 | — | — | — |
| d/dx(x²+5) at x=3 | 3 | 14.0 | 6.0 | — | — | — |
| div by zero → {0,0} | — | 0.0 | 0.0 | — | — | — |
| log(0) → {0,0} | — | 0.0 | — | — | — | — |
| √(−4) → {0,0} | — | 0.0 | — | — | — | — |

### Reverse-mode test cases

| Expression | Input(s) | ∂z/∂x expected | ∂z/∂y expected | Computed grad x | Computed grad y | Pass |
|-----------|----------|---------------|---------------|----------------|----------------|------|
| z = x + y at (3,4) | x=3, y=4 | 1.0 | 1.0 | — | — | — |
| z = x − y at (5,2) | x=5, y=2 | 1.0 | −1.0 | — | — | — |
| z = x·y at (3,4) | x=3, y=4 | 4.0 | 3.0 | — | — | — |
| z = x/y at (6,2) | x=6, y=2 | 0.5 | −1.5 | — | — | — |
| z = −x at x=5 | x=5 | −1.0 | — | — | — | — |
| z = sin(x) at x=0 | x=0 | 1.0 | — | — | — | — |
| z = cos(x) at x=0 | x=0 | 0.0 | — | — | — | — |
| z = exp(x) at x=0 | x=0 | 1.0 | — | — | — | — |
| z = log(x) at x=1 | x=1 | 1.0 | — | — | — | — |
| z = √x at x=4 | x=4 | 0.25 | — | — | — | — |
| z = x²+y² at (3,4) | x=3, y=4 | 6.0 | 8.0 | — | — | — |
| div by zero on tape | — | rc=SINGULAR | — | — | — | — |
| log(0) on tape | — | rc=INVALID | — | — | — | — |
| √(−1) on tape | — | rc=INVALID | — | — | — | — |

### Performance

| Operation | N tape nodes | Total | Per call |
|-----------|-------------|-------|----------|
| forward mul-chain depth 10 | — | — | — |
| reverse grad x²+y² | — | — | — |
| reverse grad x²+y²+z² (3 vars) | — | — | — |

---

## ARM64 — macOS / Apple M4 Pro / Apple clang / float32
**Validator:** — | **Date:** — | **Commit:** —

### Test cases

| Mode | Expression | Expected | Computed | Pass |
|------|-----------|----------|----------|------|
| *(awaiting results)* | | | | |

---

## ESP32-S3 — ESP-IDF v5.5.2 / Xtensa LX7 / xtensa-esp32s3-elf-gcc / float32
**Validator:** Amir Ab Khoshk | **Date:** 2026-05-29 | **Commit:** d81b386

> **Memory note:** `numx_ad_tape_t` is ~82 KB; declared `static` in `test_autodiff.c` to keep it in `.bss`.

### Forward-mode — dual_const_var

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| dual_const re=5 | 5.0 | 5.0000000 | 0.00e+00 | ✅ |
| dual_const du=0 | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| dual_var re=3 | 3.0 | 3.0000000 | 0.00e+00 | ✅ |
| dual_var du=1 | 1.0 | 1.0000000 | 0.00e+00 | ✅ |

### Forward-mode — dual_arith

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| dual_add re=5 | 5.0 | 5.0000000 | 0.00e+00 | ✅ |
| dual_add du=1 | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| dual_sub re=2 | 2.0 | 2.0000000 | 0.00e+00 | ✅ |
| dual_sub du=1 | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| dual_mul x² re=9 | 9.0 | 9.0000000 | 0.00e+00 | ✅ |
| dual_mul x² du=6 | 6.0 | 6.0000000 | 0.00e+00 | ✅ |
| dual_div re=2 | 2.0 | 2.0000000 | 0.00e+00 | ✅ |
| dual_div du=1/3 | 0.3333333 | 0.3333333 | 0.00e+00 | ✅ |
| dual_neg re=−3 | −3.0 | −3.0000000 | 0.00e+00 | ✅ |
| dual_neg du=−1 | −1.0 | −1.0000000 | 0.00e+00 | ✅ |

### Forward-mode — dual_transcendental

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| dual_sin re=0 (at x=0) | 0.0 | −0.0000001 | 6.25e-08 | ✅ |
| dual_sin du=1 (cos 0) | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| dual_cos re=1 (at x=0) | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| dual_cos du=0 (−sin 0) | 0.0 | 0.0000001 | 6.25e-08 | ✅ |
| dual_exp re=1 (at x=0) | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| dual_exp du=1 (exp 0) | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| dual_log re=0 (at x=1) | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| dual_log du=1 (1/1) | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| dual_sqrt re=2 (√4) | 2.0 | 2.0000000 | 0.00e+00 | ✅ |
| dual_sqrt du=0.25 (1/(2√4)) | 0.25 | 0.2500000 | 0.00e+00 | ✅ |

### Forward-mode — dual_chain

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| dual_chain quadratic re=14 | 14.0 | 14.0000000 | 0.00e+00 | ✅ |
| dual_chain quadratic du=6 | 6.0 | 6.0000000 | 0.00e+00 | ✅ |

### Forward-mode — dual_degenerate

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| dual_div/0 re=0 | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| dual_div/0 du=0 | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| dual_log(0) re=0 | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| dual_sqrt(−4) re=0 | 0.0 | 0.0000000 | 0.00e+00 | ✅ |

### Reverse-mode — tape_init

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| ad_init rc | rc=0 | rc=0 | — | ✅ |
| ad_init len=0 | 0 | 0 | — | ✅ |
| ad_var rc | rc=0 | rc=0 | — | ✅ |
| ad_var idx=0 | 0 | 0 | — | ✅ |
| ad_var val=5 | 5.0 | 5.0000000 | 0.00e+00 | ✅ |
| ad_init null | rc=-1 | rc=-1 | — | ✅ |
| ad_var null-tape | rc=-1 | rc=-1 | — | ✅ |
| ad_var null-out | rc=-1 | rc=-1 | — | ✅ |

### Reverse-mode — tape_add_sub

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| ad_add rc | rc=0 | rc=0 | — | ✅ |
| ad_add backward rc | rc=0 | rc=0 | — | ✅ |
| ad_add val=7 | 7.0 | 7.0000000 | 0.00e+00 | ✅ |
| ad_add dz/dx=1 | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| ad_add dz/dy=1 | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| ad_sub dz/dx=1 | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| ad_sub dz/dy=−1 | −1.0 | −1.0000000 | 0.00e+00 | ✅ |

### Reverse-mode — tape_mul_div

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| ad_mul val=12 | 12.0 | 12.0000000 | 0.00e+00 | ✅ |
| ad_mul dz/dx=4 | 4.0 | 4.0000000 | 0.00e+00 | ✅ |
| ad_mul dz/dy=3 | 3.0 | 3.0000000 | 0.00e+00 | ✅ |
| ad_div dz/dx=0.5 | 0.5 | 0.5000000 | 0.00e+00 | ✅ |
| ad_div dz/dy=−1.5 | −1.5 | −1.5000000 | 0.00e+00 | ✅ |
| ad_neg val=−5 | −5.0 | −5.0000000 | 0.00e+00 | ✅ |
| ad_neg dz/dx=−1 | −1.0 | −1.0000000 | 0.00e+00 | ✅ |

### Reverse-mode — tape_transcendental

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| ad_sin grad=1 (cos 0) | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| ad_cos grad=0 (−sin 0) | 0.0 | 0.0000001 | 6.25e-08 | ✅ |
| ad_exp val=1 (exp 0) | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| ad_exp grad=1 (exp 0) | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| ad_log val=0 (log 1) | 0.0 | 0.0000000 | 0.00e+00 | ✅ |
| ad_log grad=1 (1/1) | 1.0 | 1.0000000 | 0.00e+00 | ✅ |
| ad_sqrt val=2 (√4) | 2.0 | 2.0000000 | 0.00e+00 | ✅ |
| ad_sqrt grad=0.25 | 0.25 | 0.2500000 | 0.00e+00 | ✅ |

### Reverse-mode — tape_multivar

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| tape_quad val=25 (3²+4²) | 25.0 | 25.0000000 | 0.00e+00 | ✅ |
| tape_quad dz/dx=6 (2·3) | 6.0 | 6.0000000 | 0.00e+00 | ✅ |
| tape_quad dz/dy=8 (2·4) | 8.0 | 8.0000000 | 0.00e+00 | ✅ |

### Reverse-mode — tape_errors

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| backward null | rc=-1 | rc=-1 | — | ✅ |
| backward oob | rc=-2 | rc=-2 | — | ✅ |
| ad_add oob-b | rc=-2 | rc=-2 | — | ✅ |
| ad_neg oob | rc=-2 | rc=-2 | — | ✅ |
| ad_div/0 | rc=-3 | rc=-3 | — | ✅ |
| ad_log(0) | rc=-2 | rc=-2 | — | ✅ |
| ad_sqrt(−1) | rc=-2 | rc=-2 | — | ✅ |

### Precision vs reference

| Mode | Expression | Exact | Computed | Error |
|------|-----------|-------|----------|-------|
| forward | sin(0) re | 0.0 | −0.0000001 | 6.25e-08 |
| forward | −sin(0) du (cos′) | 0.0 | 0.0000001 | 6.25e-08 |
| forward | d/dx(x/3) at 6 | 0.3333333 | 0.3333333 | 0.00e+00 |
| reverse | ad_cos grad (−sin 0) | 0.0 | 0.0000001 | 6.25e-08 |
| reverse | z=x²+y² at (3,4): ∂z/∂x | 6.0 | 6.0000000 | 0.00e+00 |
| reverse | z=x²+y² at (3,4): ∂z/∂y | 8.0 | 8.0000000 | 0.00e+00 |

*Errors of 6.25e-08 on sin/cos at x=0 are float32 trig rounding — sub-epsilon, expected. All arithmetic operations (add/sub/mul/div/neg) and exp/log/sqrt are exact in float32. Both forward and reverse modes agree.*

**RESULTS: 70 PASS / 0 FAIL / 70 TOTAL**
