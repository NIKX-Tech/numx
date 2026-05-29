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

## ESP32-S3 — ESP-IDF v5.5.x / Xtensa LX7 / xtensa-esp32s3-elf-gcc / float32
**Validator:** — | **Date:** — | **Commit:** —

> **Memory note:** `numx_ad_tape_t` is ~82 KB; declared `static` in `test_autodiff.c` to keep it in `.bss`.

### Forward-mode test cases

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| dual_const re=5 | 5.0 | — | — | — |
| dual_const du=0 | 0.0 | — | — | — |
| dual_var re=3 | 3.0 | — | — | — |
| dual_var du=1 | 1.0 | — | — | — |
| dual_add re=5 | 5.0 | — | — | — |
| dual_mul x² re=9 | 9.0 | — | — | — |
| dual_mul x² du=6 | 6.0 | — | — | — |
| dual_div du=1/3 | 0.3333 | — | — | — |
| dual_sin du=1 | 1.0 | — | — | — |
| dual_cos re=1 | 1.0 | — | — | — |
| dual_exp du=1 | 1.0 | — | — | — |
| dual_log du=1 | 1.0 | — | — | — |
| dual_sqrt re=2 | 2.0 | — | — | — |
| dual_sqrt du=0.25 | 0.25 | — | — | — |
| dual_chain quadratic re=14 | 14.0 | — | — | — |
| dual_chain quadratic du=6 | 6.0 | — | — | — |
| dual_div/0 → {0,0} | 0.0 | — | — | — |

### Reverse-mode test cases

| Test | Expected | Computed | Error | Pass |
|------|----------|----------|-------|------|
| ad_init len=0 | 0 | — | — | — |
| ad_add val=7 | 7.0 | — | — | — |
| ad_add dz/dx=1 | 1.0 | — | — | — |
| ad_mul val=12 | 12.0 | — | — | — |
| ad_mul dz/dx=4 | 4.0 | — | — | — |
| ad_div dz/dx=0.5 | 0.5 | — | — | — |
| ad_neg val=-5 | −5.0 | — | — | — |
| ad_sin grad=1 | 1.0 | — | — | — |
| ad_exp val=1 | 1.0 | — | — | — |
| ad_sqrt grad=0.25 | 0.25 | — | — | — |
| tape_quad val=25 | 25.0 | — | — | — |
| tape_quad dz/dx=6 | 6.0 | — | — | — |
| tape_quad dz/dy=8 | 8.0 | — | — | — |
| ad_div/0 | rc=SINGULAR | — | — | — |
| ad_log(0) | rc=INVALID | — | — | — |
| ad_sqrt(−1) | rc=INVALID | — | — | — |

**RESULTS: — PASS / — FAIL / — TOTAL**
