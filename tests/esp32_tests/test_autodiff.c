#include "test_helpers.h"
#include "numx/autodiff.h"

/*
 * numx_ad_tape_t is ~82 KB (4096 nodes × 20 bytes).
 * Declared static so it lives in .bss instead of the task stack.
 */
static numx_ad_tape_t s_tape;

void test_autodiff(numxt_ctx_t *ctx)
{
    /* ── forward-mode: constructors ──────────────────────────────── */
    suite("autodiff / dual_const_var");
    {
        numx_dual_t c = numx_dual_const((numx_real_t)5.0);
        chk_f(ctx, "dual_const re=5", c.re, (numx_real_t)5.0, (numx_real_t)1e-5);
        chk_f(ctx, "dual_const du=0", c.du, (numx_real_t)0.0, (numx_real_t)1e-5);

        numx_dual_t v = numx_dual_var((numx_real_t)3.0);
        chk_f(ctx, "dual_var re=3",   v.re, (numx_real_t)3.0, (numx_real_t)1e-5);
        chk_f(ctx, "dual_var du=1",   v.du, (numx_real_t)1.0, (numx_real_t)1e-5);
    }

    /* ── forward-mode: arithmetic ────────────────────────────────── */
    suite("autodiff / dual_arith");
    {
        numx_dual_t x, c3, r;

        /* d/dx(x+3) at x=2: re=5, du=1 */
        x  = numx_dual_var((numx_real_t)2.0);
        c3 = numx_dual_const((numx_real_t)3.0);
        r  = numx_dual_add(x, c3);
        chk_f(ctx, "dual_add re=5",   r.re, (numx_real_t)5.0,       (numx_real_t)1e-5);
        chk_f(ctx, "dual_add du=1",   r.du, (numx_real_t)1.0,       (numx_real_t)1e-5);

        /* d/dx(x-3) at x=5: re=2, du=1 */
        x = numx_dual_var((numx_real_t)5.0);
        r = numx_dual_sub(x, c3);
        chk_f(ctx, "dual_sub re=2",   r.re, (numx_real_t)2.0,       (numx_real_t)1e-5);
        chk_f(ctx, "dual_sub du=1",   r.du, (numx_real_t)1.0,       (numx_real_t)1e-5);

        /* d/dx(x^2) at x=3: re=9, du=6 */
        x = numx_dual_var((numx_real_t)3.0);
        r = numx_dual_mul(x, x);
        chk_f(ctx, "dual_mul x^2 re=9", r.re, (numx_real_t)9.0,     (numx_real_t)1e-5);
        chk_f(ctx, "dual_mul x^2 du=6", r.du, (numx_real_t)6.0,     (numx_real_t)1e-5);

        /* d/dx(x/3) at x=6: re=2, du=1/3 */
        x = numx_dual_var((numx_real_t)6.0);
        r = numx_dual_div(x, c3);
        chk_f(ctx, "dual_div re=2",     r.re, (numx_real_t)2.0,     (numx_real_t)1e-5);
        chk_f(ctx, "dual_div du=1/3",   r.du, (numx_real_t)1.0/(numx_real_t)3.0,(numx_real_t)1e-5);

        /* d/dx(-x) at x=3: re=-3, du=-1 */
        x = numx_dual_var((numx_real_t)3.0);
        r = numx_dual_neg(x);
        chk_f(ctx, "dual_neg re=-3",    r.re, -(numx_real_t)3.0,    (numx_real_t)1e-5);
        chk_f(ctx, "dual_neg du=-1",    r.du, -(numx_real_t)1.0,    (numx_real_t)1e-5);
    }

    /* ── forward-mode: transcendentals ──────────────────────────── */
    suite("autodiff / dual_transcendental");
    {
        numx_dual_t x, r;

        /* d/dx sin(x) at x=0: re=0, du=cos(0)=1 */
        x = numx_dual_var((numx_real_t)0.0);
        r = numx_dual_sin(x);
        chk_f(ctx, "dual_sin re=0", r.re, (numx_real_t)0.0, (numx_real_t)1e-3);
        chk_f(ctx, "dual_sin du=1", r.du, (numx_real_t)1.0, (numx_real_t)1e-3);

        /* d/dx cos(x) at x=0: re=1, du=-sin(0)=0 */
        r = numx_dual_cos(x);
        chk_f(ctx, "dual_cos re=1", r.re, (numx_real_t)1.0, (numx_real_t)1e-3);
        chk_f(ctx, "dual_cos du=0", r.du, (numx_real_t)0.0, (numx_real_t)1e-3);

        /* d/dx exp(x) at x=0: re=1, du=1 */
        r = numx_dual_exp(x);
        chk_f(ctx, "dual_exp re=1", r.re, (numx_real_t)1.0, (numx_real_t)1e-3);
        chk_f(ctx, "dual_exp du=1", r.du, (numx_real_t)1.0, (numx_real_t)1e-3);

        /* d/dx log(x) at x=1: re=0, du=1 */
        x = numx_dual_var((numx_real_t)1.0);
        r = numx_dual_log(x);
        chk_f(ctx, "dual_log re=0", r.re, (numx_real_t)0.0, (numx_real_t)1e-3);
        chk_f(ctx, "dual_log du=1", r.du, (numx_real_t)1.0, (numx_real_t)1e-3);

        /* d/dx sqrt(x) at x=4: re=2, du=0.25 */
        x = numx_dual_var((numx_real_t)4.0);
        r = numx_dual_sqrt(x);
        chk_f(ctx, "dual_sqrt re=2",    r.re, (numx_real_t)2.0,  (numx_real_t)1e-3);
        chk_f(ctx, "dual_sqrt du=0.25", r.du, (numx_real_t)0.25, (numx_real_t)1e-3);
    }

    /* ── forward-mode: chain rule ────────────────────────────────── */
    suite("autodiff / dual_chain");
    {
        /* d/dx(x^2+5) at x=3: re=14, du=6 */
        numx_dual_t x  = numx_dual_var((numx_real_t)3.0);
        numx_dual_t x2 = numx_dual_mul(x, x);
        numx_dual_t c5 = numx_dual_const((numx_real_t)5.0);
        numx_dual_t r  = numx_dual_add(x2, c5);
        chk_f(ctx, "dual_chain quadratic re=14", r.re, (numx_real_t)14.0, (numx_real_t)1e-5);
        chk_f(ctx, "dual_chain quadratic du=6",  r.du,  (numx_real_t)6.0, (numx_real_t)1e-5);
    }

    /* ── forward-mode: degenerate inputs ─────────────────────────── */
    suite("autodiff / dual_degenerate");
    {
        numx_dual_t r;

        /* div by zero → {0,0} */
        r = numx_dual_div(numx_dual_var((numx_real_t)1.0), numx_dual_const((numx_real_t)0.0));
        chk_f(ctx, "dual_div/0 re=0", r.re, (numx_real_t)0.0, (numx_real_t)1e-6);
        chk_f(ctx, "dual_div/0 du=0", r.du, (numx_real_t)0.0, (numx_real_t)1e-6);

        /* log(0) → {0,0} */
        r = numx_dual_log(numx_dual_const((numx_real_t)0.0));
        chk_f(ctx, "dual_log(0) re=0", r.re, (numx_real_t)0.0, (numx_real_t)1e-6);

        /* sqrt(-4) → {0,0} */
        r = numx_dual_sqrt(numx_dual_var(-(numx_real_t)4.0));
        chk_f(ctx, "dual_sqrt(-4) re=0", r.re, (numx_real_t)0.0, (numx_real_t)1e-6);
    }

    /* ── reverse-mode: tape init and var ─────────────────────────── */
    suite("autodiff / tape_init");
    {
        numx_size_t ix;
        chk_rc(ctx, "ad_init rc",       numx_ad_init(&s_tape),          NUMX_OK);
        chk_true(ctx, "ad_init len=0",  s_tape.len == 0);

        chk_rc(ctx, "ad_var rc",        numx_ad_var(&s_tape, (numx_real_t)5.0, &ix), NUMX_OK);
        chk_true(ctx, "ad_var idx=0",   ix == 0);
        chk_f  (ctx, "ad_var val=5",    numx_ad_val(&s_tape, ix), (numx_real_t)5.0, (numx_real_t)1e-5);

        chk_rc(ctx, "ad_init null",     numx_ad_init(NULL),              NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "ad_var null-tape", numx_ad_var(NULL, (numx_real_t)1.0, &ix),    NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "ad_var null-out",  numx_ad_var(&s_tape, (numx_real_t)1.0, NULL),NUMX_ERR_NULL_PTR);
    }

    /* ── reverse-mode: addition and subtraction ──────────────────── */
    suite("autodiff / tape_add_sub");
    {
        numx_size_t xi, yi, zi;

        /* z = x + y at (3,4): val=7, dz/dx=1, dz/dy=1 */
        numx_ad_init(&s_tape);
        numx_ad_var(&s_tape, (numx_real_t)3.0, &xi);
        numx_ad_var(&s_tape, (numx_real_t)4.0, &yi);
        chk_rc(ctx, "ad_add rc",         numx_ad_add(&s_tape, xi, yi, &zi), NUMX_OK);
        chk_rc(ctx, "ad_add backward rc",numx_ad_backward(&s_tape, zi),      NUMX_OK);
        chk_f (ctx, "ad_add val=7",      numx_ad_val(&s_tape, zi),   (numx_real_t)7.0, (numx_real_t)1e-5);
        chk_f (ctx, "ad_add dz/dx=1",   numx_ad_grad(&s_tape, xi),  (numx_real_t)1.0, (numx_real_t)1e-5);
        chk_f (ctx, "ad_add dz/dy=1",   numx_ad_grad(&s_tape, yi),  (numx_real_t)1.0, (numx_real_t)1e-5);

        /* z = x - y at (5,2): dz/dx=1, dz/dy=-1 */
        numx_ad_init(&s_tape);
        numx_ad_var(&s_tape, (numx_real_t)5.0, &xi);
        numx_ad_var(&s_tape, (numx_real_t)2.0, &yi);
        numx_ad_sub(&s_tape, xi, yi, &zi);
        numx_ad_backward(&s_tape, zi);
        chk_f(ctx, "ad_sub dz/dx=1",   numx_ad_grad(&s_tape, xi),  (numx_real_t)1.0, (numx_real_t)1e-5);
        chk_f(ctx, "ad_sub dz/dy=-1",  numx_ad_grad(&s_tape, yi), -(numx_real_t)1.0, (numx_real_t)1e-5);
    }

    /* ── reverse-mode: multiplication and division ───────────────── */
    suite("autodiff / tape_mul_div");
    {
        numx_size_t xi, yi, zi;

        /* z = x*y at (3,4): val=12, dz/dx=4, dz/dy=3 */
        numx_ad_init(&s_tape);
        numx_ad_var(&s_tape, (numx_real_t)3.0, &xi);
        numx_ad_var(&s_tape, (numx_real_t)4.0, &yi);
        numx_ad_mul(&s_tape, xi, yi, &zi);
        numx_ad_backward(&s_tape, zi);
        chk_f(ctx, "ad_mul val=12",   numx_ad_val(&s_tape,  zi), (numx_real_t)12.0, (numx_real_t)1e-5);
        chk_f(ctx, "ad_mul dz/dx=4", numx_ad_grad(&s_tape, xi),  (numx_real_t)4.0, (numx_real_t)1e-5);
        chk_f(ctx, "ad_mul dz/dy=3", numx_ad_grad(&s_tape, yi),  (numx_real_t)3.0, (numx_real_t)1e-5);

        /* z = x/y at (6,2): dz/dx=0.5, dz/dy=-1.5 */
        numx_ad_init(&s_tape);
        numx_ad_var(&s_tape, (numx_real_t)6.0, &xi);
        numx_ad_var(&s_tape, (numx_real_t)2.0, &yi);
        numx_ad_div(&s_tape, xi, yi, &zi);
        numx_ad_backward(&s_tape, zi);
        chk_f(ctx, "ad_div dz/dx=0.5",  numx_ad_grad(&s_tape, xi),  (numx_real_t)0.5, (numx_real_t)1e-5);
        chk_f(ctx, "ad_div dz/dy=-1.5", numx_ad_grad(&s_tape, yi), -(numx_real_t)1.5, (numx_real_t)1e-5);

        /* z = -x at x=5: val=-5, dz/dx=-1 */
        numx_ad_init(&s_tape);
        numx_ad_var(&s_tape, (numx_real_t)5.0, &xi);
        numx_ad_neg(&s_tape, xi, &zi);
        numx_ad_backward(&s_tape, zi);
        chk_f(ctx, "ad_neg val=-5",   numx_ad_val(&s_tape,  zi), -(numx_real_t)5.0, (numx_real_t)1e-5);
        chk_f(ctx, "ad_neg dz/dx=-1", numx_ad_grad(&s_tape, xi), -(numx_real_t)1.0, (numx_real_t)1e-5);
    }

    /* ── reverse-mode: transcendentals ──────────────────────────── */
    suite("autodiff / tape_transcendental");
    {
        numx_size_t xi, zi;

        /* sin(0): grad=cos(0)=1 */
        numx_ad_init(&s_tape);
        numx_ad_var(&s_tape, (numx_real_t)0.0, &xi);
        numx_ad_sin(&s_tape, xi, &zi);
        numx_ad_backward(&s_tape, zi);
        chk_f(ctx, "ad_sin grad=1",    numx_ad_grad(&s_tape, xi),  (numx_real_t)1.0, (numx_real_t)1e-3);

        /* cos(0): grad=-sin(0)=0 */
        numx_ad_init(&s_tape);
        numx_ad_var(&s_tape, (numx_real_t)0.0, &xi);
        numx_ad_cos(&s_tape, xi, &zi);
        numx_ad_backward(&s_tape, zi);
        chk_f(ctx, "ad_cos grad=0",    numx_ad_grad(&s_tape, xi),  (numx_real_t)0.0, (numx_real_t)1e-3);

        /* exp(0): val=1, grad=1 */
        numx_ad_init(&s_tape);
        numx_ad_var(&s_tape, (numx_real_t)0.0, &xi);
        numx_ad_exp(&s_tape, xi, &zi);
        numx_ad_backward(&s_tape, zi);
        chk_f(ctx, "ad_exp val=1",     numx_ad_val(&s_tape,  zi),  (numx_real_t)1.0, (numx_real_t)1e-3);
        chk_f(ctx, "ad_exp grad=1",    numx_ad_grad(&s_tape, xi),  (numx_real_t)1.0, (numx_real_t)1e-3);

        /* log(1): val=0, grad=1 */
        numx_ad_init(&s_tape);
        numx_ad_var(&s_tape, (numx_real_t)1.0, &xi);
        numx_ad_log(&s_tape, xi, &zi);
        numx_ad_backward(&s_tape, zi);
        chk_f(ctx, "ad_log val=0",     numx_ad_val(&s_tape,  zi),  (numx_real_t)0.0, (numx_real_t)1e-3);
        chk_f(ctx, "ad_log grad=1",    numx_ad_grad(&s_tape, xi),  (numx_real_t)1.0, (numx_real_t)1e-3);

        /* sqrt(4): val=2, grad=0.25 */
        numx_ad_init(&s_tape);
        numx_ad_var(&s_tape, (numx_real_t)4.0, &xi);
        numx_ad_sqrt(&s_tape, xi, &zi);
        numx_ad_backward(&s_tape, zi);
        chk_f(ctx, "ad_sqrt val=2",    numx_ad_val(&s_tape,  zi),  (numx_real_t)2.0,  (numx_real_t)1e-3);
        chk_f(ctx, "ad_sqrt grad=0.25",numx_ad_grad(&s_tape, xi),  (numx_real_t)0.25, (numx_real_t)1e-3);
    }

    /* ── reverse-mode: multi-variable gradient ───────────────────── */
    suite("autodiff / tape_multivar");
    {
        /* z = x^2 + y^2 at (3,4): val=25, dz/dx=6, dz/dy=8 */
        numx_size_t xi, yi, x2i, y2i, zi;
        numx_ad_init(&s_tape);
        numx_ad_var(&s_tape, (numx_real_t)3.0, &xi);
        numx_ad_var(&s_tape, (numx_real_t)4.0, &yi);
        numx_ad_mul(&s_tape, xi, xi, &x2i);
        numx_ad_mul(&s_tape, yi, yi, &y2i);
        numx_ad_add(&s_tape, x2i, y2i, &zi);
        numx_ad_backward(&s_tape, zi);
        chk_f(ctx, "tape_quad val=25",  numx_ad_val(&s_tape,  zi), (numx_real_t)25.0, (numx_real_t)1e-4);
        chk_f(ctx, "tape_quad dz/dx=6", numx_ad_grad(&s_tape, xi),  (numx_real_t)6.0, (numx_real_t)1e-4);
        chk_f(ctx, "tape_quad dz/dy=8", numx_ad_grad(&s_tape, yi),  (numx_real_t)8.0, (numx_real_t)1e-4);
    }

    /* ── reverse-mode: error cases ───────────────────────────────── */
    suite("autodiff / tape_errors");
    {
        numx_size_t xi, yi, zi;
        numx_ad_init(&s_tape);
        numx_ad_var(&s_tape, (numx_real_t)1.0, &xi);

        chk_rc(ctx, "backward null",   numx_ad_backward(NULL, 0),           NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "backward oob",    numx_ad_backward(&s_tape, 999),       NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "ad_add oob-b",    numx_ad_add(&s_tape, 0, 999, &zi),   NUMX_ERR_INVALID_ARG);
        chk_rc(ctx, "ad_neg oob",      numx_ad_neg(&s_tape, 999, &zi),       NUMX_ERR_INVALID_ARG);

        /* div by zero on tape */
        numx_ad_var(&s_tape, (numx_real_t)0.0, &yi);
        chk_rc(ctx, "ad_div/0",        numx_ad_div(&s_tape, xi, yi, &zi),   NUMX_ERR_SINGULAR);

        /* log of zero/negative */
        numx_ad_init(&s_tape);
        numx_ad_var(&s_tape, (numx_real_t)0.0, &xi);
        chk_rc(ctx, "ad_log(0)",       numx_ad_log(&s_tape, xi, &zi),        NUMX_ERR_INVALID_ARG);

        /* sqrt of negative */
        numx_ad_init(&s_tape);
        numx_ad_var(&s_tape, -(numx_real_t)1.0, &xi);
        chk_rc(ctx, "ad_sqrt(-1)",     numx_ad_sqrt(&s_tape, xi, &zi),       NUMX_ERR_INVALID_ARG);
    }
}
