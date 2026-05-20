/**
 * @file autodiff.h
 * @brief Automatic differentiation — forward-mode (dual numbers) and
 *        reverse-mode (static Wengert tape).
 *
 * Two independent modes are provided:
 *
 *  Forward-mode  — @c numx_dual_t structs propagate derivatives alongside
 *                  primal values.  No tape, no state.  Best for functions
 *                  of a single input (or a small number of inputs).
 *
 *  Reverse-mode  — a fixed-size @c numx_ad_tape_t records the computation
 *                  graph.  A single backward pass computes all partial
 *                  derivatives simultaneously.  Best when the output is
 *                  scalar and the number of inputs is large.
 *
 * No dynamic allocation is used.  The tape is a flat array of at most
 * @c NUMX_MAX_AD_TAPE_LEN nodes (default 4096).
 */

#ifndef NUMX_AUTODIFF_H
#define NUMX_AUTODIFF_H

#include "numx_config.h"
#include "numx_types.h"

/* ═══════════════════════════════════════════════════════════════════════
 *  Forward-mode: dual numbers
 * ═══════════════════════════════════════════════════════════════════════ */

/**
 * @brief A dual number (re + du·ε), where ε² = 0.
 *
 * @c re  holds the primal (function) value.
 * @c du  holds the derivative (directional) component.
 *
 * Set @c du = 1 for the differentiation variable and 0 for constants:
 * @code
 *   numx_dual_t x = numx_dual_var(3.0f);   // diff w.r.t. x
 *   numx_dual_t c = numx_dual_const(5.0f); // constant
 * @endcode
 */
typedef struct {
    numx_real_t re; /**< Primal value.     */
    numx_real_t du; /**< Derivative value. */
} numx_dual_t;

/**
 * @brief Create a dual constant (derivative = 0).
 * @param c  Primal value.
 * @return   @c {c, 0}.
 */
numx_dual_t numx_dual_const(numx_real_t c);

/**
 * @brief Create a dual variable (derivative = 1).
 * @param x  Primal value.
 * @return   @c {x, 1}.
 */
numx_dual_t numx_dual_var(numx_real_t x);

/** @brief Dual addition:      a + b. */
numx_dual_t numx_dual_add(numx_dual_t a, numx_dual_t b);

/** @brief Dual subtraction:   a - b. */
numx_dual_t numx_dual_sub(numx_dual_t a, numx_dual_t b);

/** @brief Dual multiplication: a * b. */
numx_dual_t numx_dual_mul(numx_dual_t a, numx_dual_t b);

/**
 * @brief Dual division: a / b.
 *
 * Returns @c {0,0} if @c b.re == 0.
 */
numx_dual_t numx_dual_div(numx_dual_t a, numx_dual_t b);

/** @brief Dual negation: -a. */
numx_dual_t numx_dual_neg(numx_dual_t a);

/** @brief Dual sine:        sin(a), derivative = cos(a.re) * a.du. */
numx_dual_t numx_dual_sin(numx_dual_t a);

/** @brief Dual cosine:      cos(a), derivative = -sin(a.re) * a.du. */
numx_dual_t numx_dual_cos(numx_dual_t a);

/** @brief Dual exponential: exp(a), derivative = exp(a.re) * a.du. */
numx_dual_t numx_dual_exp(numx_dual_t a);

/**
 * @brief Dual natural log:  log(a), derivative = a.du / a.re.
 *
 * Returns @c {0,0} if @c a.re <= 0.
 */
numx_dual_t numx_dual_log(numx_dual_t a);

/**
 * @brief Dual square root:  sqrt(a), derivative = a.du / (2 * sqrt(a.re)).
 *
 * Returns @c {0,0} if @c a.re <= 0.
 */
numx_dual_t numx_dual_sqrt(numx_dual_t a);

/* ═══════════════════════════════════════════════════════════════════════
 *  Reverse-mode: static Wengert tape
 * ═══════════════════════════════════════════════════════════════════════ */

/**
 * @brief Operation codes stored in each tape node.
 */
typedef enum {
    NUMX_AD_OP_LEAF, /**< Input variable — no parents. */
    NUMX_AD_OP_ADD,
    NUMX_AD_OP_SUB,
    NUMX_AD_OP_MUL,
    NUMX_AD_OP_DIV,
    NUMX_AD_OP_NEG,
    NUMX_AD_OP_SIN,
    NUMX_AD_OP_COS,
    NUMX_AD_OP_EXP,
    NUMX_AD_OP_LOG,
    NUMX_AD_OP_SQRT
} numx_ad_op_t;

/**
 * @brief A single node on the Wengert tape.
 *
 * @c val  is filled during the forward pass.
 * @c adj  is filled during the backward pass (adjoint / gradient).
 * @c arg0 and @c arg1 are tape indices of parent nodes (undefined for LEAF).
 */
typedef struct {
    numx_ad_op_t op;
    numx_size_t  arg0;
    numx_size_t  arg1;
    numx_real_t  val;
    numx_real_t  adj;
} numx_ad_node_t;

/**
 * @brief The static Wengert tape — at most @c NUMX_MAX_AD_TAPE_LEN nodes.
 */
typedef struct {
    numx_ad_node_t nodes[NUMX_MAX_AD_TAPE_LEN];
    numx_size_t    len;
} numx_ad_tape_t;

/**
 * @brief Initialise a tape (set length to zero, zero all adjoints).
 * @param tape  Non-null tape pointer.
 * @return @c NUMX_OK or @c NUMX_ERR_NULL_PTR.
 */
numx_status_t numx_ad_init(numx_ad_tape_t *tape);

/**
 * @brief Record an input variable on the tape.
 * @param tape     Non-null tape pointer.
 * @param val      Primal value.
 * @param out_idx  Receives the new node's index.
 * @return @c NUMX_OK, @c NUMX_ERR_NULL_PTR, or @c NUMX_ERR_BUFFER_SMALL.
 */
numx_status_t numx_ad_var(numx_ad_tape_t *tape, numx_real_t val,
                           numx_size_t *out_idx);

/**
 * @brief Record @c nodes[a].val + nodes[b].val on the tape.
 * @return @c NUMX_OK, @c NUMX_ERR_NULL_PTR, @c NUMX_ERR_INVALID_ARG,
 *         or @c NUMX_ERR_BUFFER_SMALL.
 */
numx_status_t numx_ad_add(numx_ad_tape_t *tape,
                           numx_size_t a, numx_size_t b,
                           numx_size_t *out_idx);

/** @brief Record @c nodes[a].val - nodes[b].val on the tape. */
numx_status_t numx_ad_sub(numx_ad_tape_t *tape,
                           numx_size_t a, numx_size_t b,
                           numx_size_t *out_idx);

/** @brief Record @c nodes[a].val * nodes[b].val on the tape. */
numx_status_t numx_ad_mul(numx_ad_tape_t *tape,
                           numx_size_t a, numx_size_t b,
                           numx_size_t *out_idx);

/**
 * @brief Record @c nodes[a].val / nodes[b].val on the tape.
 * @return @c NUMX_ERR_SINGULAR if @c nodes[b].val == 0.
 */
numx_status_t numx_ad_div(numx_ad_tape_t *tape,
                           numx_size_t a, numx_size_t b,
                           numx_size_t *out_idx);

/** @brief Record negation of @c nodes[a].val on the tape. */
numx_status_t numx_ad_neg(numx_ad_tape_t *tape,
                           numx_size_t a,
                           numx_size_t *out_idx);

/** @brief Record @c sin(nodes[a].val) on the tape. */
numx_status_t numx_ad_sin(numx_ad_tape_t *tape,
                           numx_size_t a,
                           numx_size_t *out_idx);

/** @brief Record @c cos(nodes[a].val) on the tape. */
numx_status_t numx_ad_cos(numx_ad_tape_t *tape,
                           numx_size_t a,
                           numx_size_t *out_idx);

/** @brief Record @c exp(nodes[a].val) on the tape. */
numx_status_t numx_ad_exp(numx_ad_tape_t *tape,
                           numx_size_t a,
                           numx_size_t *out_idx);

/**
 * @brief Record @c log(nodes[a].val) on the tape.
 * @return @c NUMX_ERR_INVALID_ARG if @c nodes[a].val <= 0.
 */
numx_status_t numx_ad_log(numx_ad_tape_t *tape,
                           numx_size_t a,
                           numx_size_t *out_idx);

/**
 * @brief Record @c sqrt(nodes[a].val) on the tape.
 * @return @c NUMX_ERR_INVALID_ARG if @c nodes[a].val < 0.
 */
numx_status_t numx_ad_sqrt(numx_ad_tape_t *tape,
                            numx_size_t a,
                            numx_size_t *out_idx);

/**
 * @brief Run the backward pass from @p output_idx.
 *
 * Zeros all adjoints, sets @c nodes[output_idx].adj = 1, then sweeps
 * from @p output_idx down to 0 accumulating adjoints via the chain rule.
 *
 * @param tape        Non-null tape pointer.
 * @param output_idx  Index of the scalar output node.
 * @return @c NUMX_OK, @c NUMX_ERR_NULL_PTR, or @c NUMX_ERR_INVALID_ARG.
 */
numx_status_t numx_ad_backward(numx_ad_tape_t *tape, numx_size_t output_idx);

/**
 * @brief Return the primal value of node @p idx.
 * @return @c 0 if @p tape is null or @p idx is out of range.
 */
numx_real_t numx_ad_val(const numx_ad_tape_t *tape, numx_size_t idx);

/**
 * @brief Return the gradient (adjoint) of node @p idx after backward().
 * @return @c 0 if @p tape is null or @p idx is out of range.
 */
numx_real_t numx_ad_grad(const numx_ad_tape_t *tape, numx_size_t idx);

#endif /* NUMX_AUTODIFF_H */
