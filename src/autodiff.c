/**
 * @file autodiff.c
 * @brief Automatic differentiation — forward-mode dual numbers and
 *        reverse-mode static Wengert tape.
 */

#include "numx/autodiff.h"

/* ── Private math helpers ──────────────────────────────────────────────
 * These are intentionally duplicated (static) per-file; numx has no
 * shared internal header so each module is self-contained.
 * ─────────────────────────────────────────────────────────────────────── */

/* Core Taylor-series cosine for x in [0, π]. */
static numx_real_t priv_cos_core(numx_real_t x)
{
    numx_real_t sign, x2, term, sum;
    int k;
    sign = (numx_real_t)1.0;
    if (x > NUMX_PI * (numx_real_t)0.5)
    {
        x    = NUMX_PI - x;
        sign = -(numx_real_t)1.0;
    }
    x2   = x * x;
    term = (numx_real_t)1.0;
    sum  = term;
    for (k = 1; k <= 12; k++)
    {
        term *= -x2 / (numx_real_t)((2 * k - 1) * (2 * k));
        sum  += term;
    }
    return sign * sum;
}

/* Full-range cosine (any real x). */
static numx_real_t priv_cos_ad(numx_real_t x)
{
    numx_real_t two_pi;
    two_pi = (numx_real_t)2.0 * NUMX_PI;
    if (x < (numx_real_t)0.0)
        x = -x; /* cos is even */
    while (x > two_pi)
        x -= two_pi; /* reduce to [0, 2π] */
    if (x > NUMX_PI)
        x = two_pi - x; /* reflect to [0, π] */
    return priv_cos_core(x);
}

/* Full-range sine (any real x).
 * sin(x) for x in [0,π]: cos(π/2 − x).
 * sin is odd; sin(x) for x in [π,2π]: −sin(2π − x). */
static numx_real_t priv_sin_ad(numx_real_t x)
{
    numx_real_t two_pi, sign, arg;
    two_pi = (numx_real_t)2.0 * NUMX_PI;
    sign   = (numx_real_t)1.0;
    if (x < (numx_real_t)0.0)
    {
        x    = -x;
        sign = -(numx_real_t)1.0;
    }
    while (x > two_pi)
        x -= two_pi;
    if (x > NUMX_PI)
    {
        x    = two_pi - x;
        sign = -sign;
    }
    arg = NUMX_PI * (numx_real_t)0.5 - x;
    if (arg < (numx_real_t)0.0)
        arg = -arg;
    return sign * priv_cos_core(arg);
}

/* Newton-Raphson square root. Returns 0 for x <= 0. */
static numx_real_t priv_sqrt_ad(numx_real_t x)
{
    numx_real_t r;
    int k;
    if (x <= (numx_real_t)0.0)
        return (numx_real_t)0.0;
    r = x;
    for (k = 0; k < 64; k++)
        r = (numx_real_t)0.5 * (r + x / r);
    return r;
}

/* exp via range-reduction: compute e^(x/128) with 20-term Taylor,
 * then square 7 times (2^7 = 128). */
static numx_real_t priv_exp_ad(numx_real_t x)
{
    numx_real_t y, term, result;
    int k;
    y      = x * (numx_real_t)(1.0 / 128.0);
    term   = (numx_real_t)1.0;
    result = term;
    for (k = 1; k <= 20; k++)
    {
        term   *= y / (numx_real_t)k;
        result += term;
    }
    for (k = 0; k < 7; k++)
        result *= result;
    return result;
}

/* Natural log via atanh series: ln(x) = 2·atanh((x-1)/(x+1)).
 * Range-reduce to [1, 2) first; track n halvings/doublings. */
static numx_real_t priv_log_ad(numx_real_t x)
{
    static const numx_real_t LN2 = (numx_real_t)0.6931471805599453;
    numx_real_t u, u2, term, result;
    int n, k;
    if (x <= (numx_real_t)0.0)
        return (numx_real_t)0.0;
    n = 0;
    while (x >= (numx_real_t)2.0)
    {
        x *= (numx_real_t)0.5;
        n++;
    }
    while (x < (numx_real_t)1.0)
    {
        x *= (numx_real_t)2.0;
        n--;
    }
    /* x now in [1, 2): atanh((x-1)/(x+1)) */
    u      = (x - (numx_real_t)1.0) / (x + (numx_real_t)1.0);
    u2     = u * u;
    term   = u;
    result = term;
    for (k = 1; k <= 20; k++)
    {
        term   *= u2;
        result += term / (numx_real_t)(2 * k + 1);
    }
    return (numx_real_t)2.0 * result + (numx_real_t)n * LN2;
}

/* ═══════════════════════════════════════════════════════════════════════
 *  Forward-mode: dual number operations
 * ═══════════════════════════════════════════════════════════════════════ */

numx_dual_t numx_dual_const(numx_real_t c)
{
    numx_dual_t r;
    r.re = c;
    r.du = (numx_real_t)0.0;
    return r;
}

numx_dual_t numx_dual_var(numx_real_t x)
{
    numx_dual_t r;
    r.re = x;
    r.du = (numx_real_t)1.0;
    return r;
}

numx_dual_t numx_dual_add(numx_dual_t a, numx_dual_t b)
{
    numx_dual_t r;
    r.re = a.re + b.re;
    r.du = a.du + b.du;
    return r;
}

numx_dual_t numx_dual_sub(numx_dual_t a, numx_dual_t b)
{
    numx_dual_t r;
    r.re = a.re - b.re;
    r.du = a.du - b.du;
    return r;
}

numx_dual_t numx_dual_mul(numx_dual_t a, numx_dual_t b)
{
    numx_dual_t r;
    r.re = a.re * b.re;
    r.du = a.re * b.du + a.du * b.re;
    return r;
}

numx_dual_t numx_dual_div(numx_dual_t a, numx_dual_t b)
{
    numx_dual_t r;
    if (b.re == (numx_real_t)0.0)
    {
        r.re = (numx_real_t)0.0;
        r.du = (numx_real_t)0.0;
        return r;
    }
    r.re = a.re / b.re;
    r.du = (a.du * b.re - a.re * b.du) / (b.re * b.re);
    return r;
}

numx_dual_t numx_dual_neg(numx_dual_t a)
{
    numx_dual_t r;
    r.re = -a.re;
    r.du = -a.du;
    return r;
}

numx_dual_t numx_dual_sin(numx_dual_t a)
{
    numx_dual_t r;
    r.re = priv_sin_ad(a.re);
    r.du = priv_cos_ad(a.re) * a.du;
    return r;
}

numx_dual_t numx_dual_cos(numx_dual_t a)
{
    numx_dual_t r;
    r.re = priv_cos_ad(a.re);
    r.du = -priv_sin_ad(a.re) * a.du;
    return r;
}

numx_dual_t numx_dual_exp(numx_dual_t a)
{
    numx_dual_t r;
    r.re = priv_exp_ad(a.re);
    r.du = r.re * a.du;
    return r;
}

numx_dual_t numx_dual_log(numx_dual_t a)
{
    numx_dual_t r;
    if (a.re <= (numx_real_t)0.0)
    {
        r.re = (numx_real_t)0.0;
        r.du = (numx_real_t)0.0;
        return r;
    }
    r.re = priv_log_ad(a.re);
    r.du = a.du / a.re;
    return r;
}

numx_dual_t numx_dual_sqrt(numx_dual_t a)
{
    numx_dual_t r;
    numx_real_t sv;
    if (a.re <= (numx_real_t)0.0)
    {
        r.re = (numx_real_t)0.0;
        r.du = (numx_real_t)0.0;
        return r;
    }
    sv   = priv_sqrt_ad(a.re);
    r.re = sv;
    r.du = a.du / ((numx_real_t)2.0 * sv);
    return r;
}

/* ═══════════════════════════════════════════════════════════════════════
 *  Reverse-mode: Wengert tape
 * ═══════════════════════════════════════════════════════════════════════ */

/* Push a new node onto the tape and return its index. */
static numx_status_t priv_tape_push(
    numx_ad_tape_t *tape,
    numx_ad_op_t    op,
    numx_size_t     arg0,
    numx_size_t     arg1,
    numx_real_t     val,
    numx_size_t    *out_idx)
{
    numx_ad_node_t *nd;
    if (tape->len >= NUMX_MAX_AD_TAPE_LEN)
        return NUMX_ERR_BUFFER_SMALL;
    nd       = &tape->nodes[tape->len];
    nd->op   = op;
    nd->arg0 = arg0;
    nd->arg1 = arg1;
    nd->val  = val;
    nd->adj  = (numx_real_t)0.0;
    *out_idx = tape->len;
    tape->len++;
    return NUMX_OK;
}

numx_status_t numx_ad_init(numx_ad_tape_t *tape)
{
    numx_size_t i;
    if (!tape)
        return NUMX_ERR_NULL_PTR;
    tape->len = 0;
    for (i = 0; i < NUMX_MAX_AD_TAPE_LEN; i++)
        tape->nodes[i].adj = (numx_real_t)0.0;
    return NUMX_OK;
}

numx_status_t numx_ad_var(numx_ad_tape_t *tape, numx_real_t val,
                           numx_size_t *out_idx)
{
    if (!tape || !out_idx)
        return NUMX_ERR_NULL_PTR;
    return priv_tape_push(tape, NUMX_AD_OP_LEAF, 0, 0, val, out_idx);
}

numx_status_t numx_ad_add(numx_ad_tape_t *tape,
                           numx_size_t a, numx_size_t b,
                           numx_size_t *out_idx)
{
    if (!tape || !out_idx)
        return NUMX_ERR_NULL_PTR;
    if (a >= tape->len || b >= tape->len)
        return NUMX_ERR_INVALID_ARG;
    return priv_tape_push(tape, NUMX_AD_OP_ADD, a, b,
                          tape->nodes[a].val + tape->nodes[b].val, out_idx);
}

numx_status_t numx_ad_sub(numx_ad_tape_t *tape,
                           numx_size_t a, numx_size_t b,
                           numx_size_t *out_idx)
{
    if (!tape || !out_idx)
        return NUMX_ERR_NULL_PTR;
    if (a >= tape->len || b >= tape->len)
        return NUMX_ERR_INVALID_ARG;
    return priv_tape_push(tape, NUMX_AD_OP_SUB, a, b,
                          tape->nodes[a].val - tape->nodes[b].val, out_idx);
}

numx_status_t numx_ad_mul(numx_ad_tape_t *tape,
                           numx_size_t a, numx_size_t b,
                           numx_size_t *out_idx)
{
    if (!tape || !out_idx)
        return NUMX_ERR_NULL_PTR;
    if (a >= tape->len || b >= tape->len)
        return NUMX_ERR_INVALID_ARG;
    return priv_tape_push(tape, NUMX_AD_OP_MUL, a, b,
                          tape->nodes[a].val * tape->nodes[b].val, out_idx);
}

numx_status_t numx_ad_div(numx_ad_tape_t *tape,
                           numx_size_t a, numx_size_t b,
                           numx_size_t *out_idx)
{
    if (!tape || !out_idx)
        return NUMX_ERR_NULL_PTR;
    if (a >= tape->len || b >= tape->len)
        return NUMX_ERR_INVALID_ARG;
    if (tape->nodes[b].val == (numx_real_t)0.0)
        return NUMX_ERR_SINGULAR;
    return priv_tape_push(tape, NUMX_AD_OP_DIV, a, b,
                          tape->nodes[a].val / tape->nodes[b].val, out_idx);
}

numx_status_t numx_ad_neg(numx_ad_tape_t *tape,
                           numx_size_t a,
                           numx_size_t *out_idx)
{
    if (!tape || !out_idx)
        return NUMX_ERR_NULL_PTR;
    if (a >= tape->len)
        return NUMX_ERR_INVALID_ARG;
    return priv_tape_push(tape, NUMX_AD_OP_NEG, a, 0,
                          -tape->nodes[a].val, out_idx);
}

numx_status_t numx_ad_sin(numx_ad_tape_t *tape,
                           numx_size_t a,
                           numx_size_t *out_idx)
{
    if (!tape || !out_idx)
        return NUMX_ERR_NULL_PTR;
    if (a >= tape->len)
        return NUMX_ERR_INVALID_ARG;
    return priv_tape_push(tape, NUMX_AD_OP_SIN, a, 0,
                          priv_sin_ad(tape->nodes[a].val), out_idx);
}

numx_status_t numx_ad_cos(numx_ad_tape_t *tape,
                           numx_size_t a,
                           numx_size_t *out_idx)
{
    if (!tape || !out_idx)
        return NUMX_ERR_NULL_PTR;
    if (a >= tape->len)
        return NUMX_ERR_INVALID_ARG;
    return priv_tape_push(tape, NUMX_AD_OP_COS, a, 0,
                          priv_cos_ad(tape->nodes[a].val), out_idx);
}

numx_status_t numx_ad_exp(numx_ad_tape_t *tape,
                           numx_size_t a,
                           numx_size_t *out_idx)
{
    if (!tape || !out_idx)
        return NUMX_ERR_NULL_PTR;
    if (a >= tape->len)
        return NUMX_ERR_INVALID_ARG;
    return priv_tape_push(tape, NUMX_AD_OP_EXP, a, 0,
                          priv_exp_ad(tape->nodes[a].val), out_idx);
}

numx_status_t numx_ad_log(numx_ad_tape_t *tape,
                           numx_size_t a,
                           numx_size_t *out_idx)
{
    if (!tape || !out_idx)
        return NUMX_ERR_NULL_PTR;
    if (a >= tape->len)
        return NUMX_ERR_INVALID_ARG;
    if (tape->nodes[a].val <= (numx_real_t)0.0)
        return NUMX_ERR_INVALID_ARG;
    return priv_tape_push(tape, NUMX_AD_OP_LOG, a, 0,
                          priv_log_ad(tape->nodes[a].val), out_idx);
}

numx_status_t numx_ad_sqrt(numx_ad_tape_t *tape,
                            numx_size_t a,
                            numx_size_t *out_idx)
{
    if (!tape || !out_idx)
        return NUMX_ERR_NULL_PTR;
    if (a >= tape->len)
        return NUMX_ERR_INVALID_ARG;
    if (tape->nodes[a].val < (numx_real_t)0.0)
        return NUMX_ERR_INVALID_ARG;
    return priv_tape_push(tape, NUMX_AD_OP_SQRT, a, 0,
                          priv_sqrt_ad(tape->nodes[a].val), out_idx);
}

numx_status_t numx_ad_backward(numx_ad_tape_t *tape, numx_size_t output_idx)
{
    numx_size_t i;
    numx_ad_node_t *nd;
    numx_real_t g, av, bv;

    if (!tape)
        return NUMX_ERR_NULL_PTR;
    if (output_idx >= tape->len)
        return NUMX_ERR_INVALID_ARG;

    /* Zero all adjoints. */
    for (i = 0; i < tape->len; i++)
        tape->nodes[i].adj = (numx_real_t)0.0;

    tape->nodes[output_idx].adj = (numx_real_t)1.0;

    /* Reverse sweep — unsigned-wrap trick stops naturally at i==0. */
    i = output_idx + 1;
    while (i-- > 0)
    {
        nd = &tape->nodes[i];
        g  = nd->adj;
        if (g == (numx_real_t)0.0)
            continue;

        switch (nd->op)
        {
        case NUMX_AD_OP_LEAF:
            break;
        case NUMX_AD_OP_ADD:
            tape->nodes[nd->arg0].adj += g;
            tape->nodes[nd->arg1].adj += g;
            break;
        case NUMX_AD_OP_SUB:
            tape->nodes[nd->arg0].adj += g;
            tape->nodes[nd->arg1].adj -= g;
            break;
        case NUMX_AD_OP_MUL:
            tape->nodes[nd->arg0].adj += g * tape->nodes[nd->arg1].val;
            tape->nodes[nd->arg1].adj += g * tape->nodes[nd->arg0].val;
            break;
        case NUMX_AD_OP_DIV:
            av = tape->nodes[nd->arg0].val;
            bv = tape->nodes[nd->arg1].val;
            tape->nodes[nd->arg0].adj += g / bv;
            tape->nodes[nd->arg1].adj -= g * av / (bv * bv);
            break;
        case NUMX_AD_OP_NEG:
            tape->nodes[nd->arg0].adj -= g;
            break;
        case NUMX_AD_OP_SIN:
            tape->nodes[nd->arg0].adj += g * priv_cos_ad(tape->nodes[nd->arg0].val);
            break;
        case NUMX_AD_OP_COS:
            tape->nodes[nd->arg0].adj -= g * priv_sin_ad(tape->nodes[nd->arg0].val);
            break;
        case NUMX_AD_OP_EXP:
            /* d/da e^a = e^a = nd->val */
            tape->nodes[nd->arg0].adj += g * nd->val;
            break;
        case NUMX_AD_OP_LOG:
            tape->nodes[nd->arg0].adj += g / tape->nodes[nd->arg0].val;
            break;
        case NUMX_AD_OP_SQRT:
            /* d/da sqrt(a) = 1/(2*sqrt(a)) = 1/(2*nd->val) */
            tape->nodes[nd->arg0].adj += g / ((numx_real_t)2.0 * nd->val);
            break;
        default:
            break;
        }
    }
    return NUMX_OK;
}

numx_real_t numx_ad_val(const numx_ad_tape_t *tape, numx_size_t idx)
{
    if (!tape || idx >= tape->len)
        return (numx_real_t)0.0;
    return tape->nodes[idx].val;
}

numx_real_t numx_ad_grad(const numx_ad_tape_t *tape, numx_size_t idx)
{
    if (!tape || idx >= tape->len)
        return (numx_real_t)0.0;
    return tape->nodes[idx].adj;
}
