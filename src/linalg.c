/**
 * @file linalg.c
 * @brief Linear algebra implementation for numx.
 */

#include "numx/linalg.h"

/* ── Private helpers ───────────────────────────────────────────────── */

/* Row-major element access: element (r,c) in a matrix with nc columns. */
#define MAT(A, nc, r, c)  ((A)[(numx_size_t)(r) * (numx_size_t)(nc) + (numx_size_t)(c)])

static numx_real_t priv_abs(numx_real_t x)
{
    return x < (numx_real_t)0.0 ? -x : x;
}

/* Newton-Raphson square root — no <math.h> dependency. */
static numx_real_t priv_sqrt(numx_real_t x)
{
    numx_real_t g, ng;
    int i;
    if (x <= (numx_real_t)0.0) return (numx_real_t)0.0;
    g = x > (numx_real_t)1.0 ? x * (numx_real_t)0.5 : (numx_real_t)1.0;
    for (i = 0; i < 64; i++) {
        ng = (numx_real_t)0.5 * (g + x / g);
        if (ng == g) break;
        g = ng;
    }
    return g;
}

/* 2x2 determinant: |a b; c d| */
static numx_real_t priv_det2(numx_real_t a, numx_real_t b,
                              numx_real_t c, numx_real_t d)
{
    return a * d - b * c;
}

/* 3x3 determinant from 9 individual elements (row-major order). */
static numx_real_t priv_det3(numx_real_t a, numx_real_t b, numx_real_t c,
                              numx_real_t d, numx_real_t e, numx_real_t f,
                              numx_real_t g, numx_real_t h, numx_real_t k)
{
    return a * priv_det2(e, f, h, k)
         - b * priv_det2(d, f, g, k)
         + c * priv_det2(d, e, g, h);
}

/* ── Vector operations ─────────────────────────────────────────────── */

numx_status_t numx_vec_dot(
    const numx_real_t *a,
    const numx_real_t *b,
    numx_size_t        n,
    numx_real_t       *result)
{
    numx_size_t i;
    numx_real_t sum;

    if (!a || !b || !result)             return NUMX_ERR_NULL_PTR;
    if (n == 0 || n > NUMX_MAX_VEC_SIZE) return NUMX_ERR_INVALID_ARG;

    sum = (numx_real_t)0.0;
    for (i = 0; i < n; i++)
        sum += a[i] * b[i];

    *result = sum;
    return NUMX_OK;
}

numx_status_t numx_vec_norm(
    const numx_real_t *a,
    numx_size_t        n,
    numx_norm_t        type,
    numx_real_t       *result)
{
    numx_size_t i;
    numx_real_t acc, v;

    if (!a || !result)                   return NUMX_ERR_NULL_PTR;
    if (n == 0 || n > NUMX_MAX_VEC_SIZE) return NUMX_ERR_INVALID_ARG;

    acc = (numx_real_t)0.0;
    switch (type) {
    case NUMX_NORM_L1:
        for (i = 0; i < n; i++) acc += priv_abs(a[i]);
        break;
    case NUMX_NORM_L2:
        for (i = 0; i < n; i++) acc += a[i] * a[i];
        acc = priv_sqrt(acc);
        break;
    case NUMX_NORM_INF:
        for (i = 0; i < n; i++) {
            v = priv_abs(a[i]);
            if (v > acc) acc = v;
        }
        break;
    default:
        return NUMX_ERR_INVALID_ARG;
    }

    *result = acc;
    return NUMX_OK;
}

numx_status_t numx_vec_cross3(
    const numx_real_t *a,
    const numx_real_t *b,
    numx_real_t       *result)
{
    numx_real_t r0, r1, r2;

    if (!a || !b || !result) return NUMX_ERR_NULL_PTR;

    /* Compute into temporaries first to allow result to alias a or b. */
    r0 = a[1] * b[2] - a[2] * b[1];
    r1 = a[2] * b[0] - a[0] * b[2];
    r2 = a[0] * b[1] - a[1] * b[0];

    result[0] = r0;
    result[1] = r1;
    result[2] = r2;
    return NUMX_OK;
}

/* ── Matrix operations ─────────────────────────────────────────────── */

numx_status_t numx_mat_mul(
    const numx_real_t *A, numx_size_t ra, numx_size_t ca,
    const numx_real_t *B, numx_size_t rb, numx_size_t cb,
    numx_real_t       *C)
{
    numx_size_t i, j, k;
    numx_real_t sum;

    if (!A || !B || !C)                                    return NUMX_ERR_NULL_PTR;
    if (ra == 0 || ca == 0 || cb == 0)                     return NUMX_ERR_INVALID_ARG;
    if (ra > NUMX_MAX_MAT_ROWS || ca > NUMX_MAX_MAT_COLS)  return NUMX_ERR_INVALID_ARG;
    if (rb > NUMX_MAX_MAT_ROWS || cb > NUMX_MAX_MAT_COLS)  return NUMX_ERR_INVALID_ARG;
    if (ca != rb)                                          return NUMX_ERR_INVALID_ARG;

    for (i = 0; i < ra; i++) {
        for (j = 0; j < cb; j++) {
            sum = (numx_real_t)0.0;
            for (k = 0; k < ca; k++)
                sum += MAT(A, ca, i, k) * MAT(B, cb, k, j);
            MAT(C, cb, i, j) = sum;
        }
    }
    return NUMX_OK;
}

numx_status_t numx_mat_transpose(
    const numx_real_t *A,
    numx_size_t        rows,
    numx_size_t        cols,
    numx_real_t       *AT)
{
    numx_size_t r, c;

    if (!A || !AT)                              return NUMX_ERR_NULL_PTR;
    if (rows == 0 || rows > NUMX_MAX_MAT_ROWS)  return NUMX_ERR_INVALID_ARG;
    if (cols == 0 || cols > NUMX_MAX_MAT_COLS)  return NUMX_ERR_INVALID_ARG;

    /* AT is (cols x rows): AT[c][r] = A[r][c] */
    for (r = 0; r < rows; r++)
        for (c = 0; c < cols; c++)
            MAT(AT, rows, c, r) = MAT(A, cols, r, c);

    return NUMX_OK;
}

numx_status_t numx_mat_transpose_sq(
    numx_real_t *A,
    numx_size_t  n)
{
    numx_size_t r, c;
    numx_real_t tmp;

    if (!A)                              return NUMX_ERR_NULL_PTR;
    if (n == 0 || n > NUMX_MAX_MAT_ROWS) return NUMX_ERR_INVALID_ARG;

    for (r = 0; r < n; r++) {
        for (c = r + 1; c < n; c++) {
            tmp             = MAT(A, n, r, c);
            MAT(A, n, r, c) = MAT(A, n, c, r);
            MAT(A, n, c, r) = tmp;
        }
    }
    return NUMX_OK;
}

numx_status_t numx_mat_det(
    const numx_real_t *A,
    numx_size_t        n,
    numx_real_t       *result)
{
    numx_real_t LU[NUMX_MAX_MAT_ROWS * NUMX_MAX_MAT_COLS];
    numx_idx_t  pivot[NUMX_MAX_MAT_ROWS];
    numx_real_t det;
    numx_size_t i;
    numx_status_t s;
    /* permutation parity tracking */
    numx_idx_t  visited[NUMX_MAX_MAT_ROWS];
    numx_idx_t  num_swaps, j2;
    numx_size_t cycle_len;

    if (!A || !result)                   return NUMX_ERR_NULL_PTR;
    if (n == 0 || n > NUMX_MAX_MAT_ROWS) return NUMX_ERR_INVALID_ARG;

    if (n == 1) { *result = A[0]; return NUMX_OK; }

    if (n == 2) {
        *result = priv_det2(A[0], A[1], A[2], A[3]);
        return NUMX_OK;
    }

    if (n == 3) {
        *result = priv_det3(A[0],A[1],A[2], A[3],A[4],A[5], A[6],A[7],A[8]);
        return NUMX_OK;
    }

    if (n == 4) {
        /* Cofactor expansion along first row. */
        *result =
            A[0]  * priv_det3(A[5], A[6], A[7],  A[9], A[10],A[11], A[13],A[14],A[15])
          - A[1]  * priv_det3(A[4], A[6], A[7],  A[8], A[10],A[11], A[12],A[14],A[15])
          + A[2]  * priv_det3(A[4], A[5], A[7],  A[8], A[9], A[11], A[12],A[13],A[15])
          - A[3]  * priv_det3(A[4], A[5], A[6],  A[8], A[9], A[10], A[12],A[13],A[14]);
        return NUMX_OK;
    }

    /* n > 4: LU decomposition path. */
    s = numx_lu_decompose(A, n, LU, pivot);
    if (s == NUMX_ERR_SINGULAR) { *result = (numx_real_t)0.0; return NUMX_OK; }
    if (s != NUMX_OK) return s;

    det = (numx_real_t)1.0;
    for (i = 0; i < n; i++)
        det *= MAT(LU, n, i, i);

    /* Count parity of permutation via cycle decomposition. */
    for (i = 0; i < n; i++) visited[i] = 0;
    num_swaps = 0;
    for (i = 0; i < n; i++) {
        if (!visited[i]) {
            cycle_len = 0;
            j2 = (numx_idx_t)i;
            while (!visited[j2]) {
                visited[j2] = 1;
                j2 = pivot[j2];
                cycle_len++;
            }
            num_swaps += (numx_idx_t)cycle_len - 1;
        }
    }
    if (num_swaps % 2 != 0) det = -det;

    *result = det;
    return NUMX_OK;
}

/* ── LU decomposition ──────────────────────────────────────────────── */

numx_status_t numx_lu_decompose(
    const numx_real_t *A,
    numx_size_t        n,
    numx_real_t       *LU,
    numx_idx_t        *pivot)
{
    numx_size_t i, j, k, max_row;
    numx_real_t max_val, val, tmp;
    numx_idx_t  ptmp;

    if (!A || !LU || !pivot)             return NUMX_ERR_NULL_PTR;
    if (n == 0 || n > NUMX_MAX_MAT_ROWS) return NUMX_ERR_INVALID_ARG;

    for (i = 0; i < n * n; i++) LU[i] = A[i];
    for (i = 0; i < n; i++)     pivot[i] = (numx_idx_t)i;

    for (k = 0; k < n; k++) {
        /* Partial pivoting: find row with largest absolute value in column k. */
        max_val = priv_abs(MAT(LU, n, k, k));
        max_row = k;
        for (i = k + 1; i < n; i++) {
            val = priv_abs(MAT(LU, n, i, k));
            if (val > max_val) { max_val = val; max_row = i; }
        }

        if (max_val < NUMX_EPSILON) return NUMX_ERR_SINGULAR;

        if (max_row != k) {
            for (j = 0; j < n; j++) {
                tmp                    = MAT(LU, n, k,       j);
                MAT(LU, n, k,       j) = MAT(LU, n, max_row, j);
                MAT(LU, n, max_row, j) = tmp;
            }
            ptmp           = pivot[k];
            pivot[k]       = pivot[max_row];
            pivot[max_row] = ptmp;
        }

        for (i = k + 1; i < n; i++) {
            MAT(LU, n, i, k) /= MAT(LU, n, k, k);
            for (j = k + 1; j < n; j++)
                MAT(LU, n, i, j) -= MAT(LU, n, i, k) * MAT(LU, n, k, j);
        }
    }
    return NUMX_OK;
}

numx_status_t numx_lu_solve(
    const numx_real_t *LU,
    const numx_idx_t  *pivot,
    numx_size_t        n,
    const numx_real_t *b,
    numx_real_t       *x)
{
    numx_real_t y[NUMX_MAX_MAT_ROWS];
    numx_size_t i, j;
    numx_real_t sum;

    if (!LU || !pivot || !b || !x)       return NUMX_ERR_NULL_PTR;
    if (n == 0 || n > NUMX_MAX_MAT_ROWS) return NUMX_ERR_INVALID_ARG;

    /* Forward substitution: L * y = P * b */
    for (i = 0; i < n; i++) {
        sum = b[(numx_size_t)pivot[i]];
        for (j = 0; j < i; j++)
            sum -= MAT(LU, n, i, j) * y[j];
        y[i] = sum;
    }

    /* Backward substitution: U * x = y */
    for (i = n; i-- > 0; ) {
        if (priv_abs(MAT(LU, n, i, i)) < NUMX_EPSILON) return NUMX_ERR_SINGULAR;
        sum = y[i];
        for (j = i + 1; j < n; j++)
            sum -= MAT(LU, n, i, j) * x[j];
        x[i] = sum / MAT(LU, n, i, i);
    }
    return NUMX_OK;
}
