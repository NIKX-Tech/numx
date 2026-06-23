#include <string.h>
#include "test_helpers.h"
#include "numx/linalg.h"

void test_linalg(numxt_ctx_t *ctx)
{
    numx_real_t r;

    /* ── vec_dot ──────────────────────────────────────────────── */
    suite("linalg / vec_dot");
    {
        numx_real_t a3[3]={1,2,3}, b3[3]={4,5,6};
        chk_rc (ctx,"vec_dot known-answer rc",
                numx_vec_dot(a3,b3,3,&r), NUMX_OK);
        chk_f  (ctx,"vec_dot [1,2,3]·[4,5,6]=32", r, (numx_real_t)32.0, (numx_real_t)1e-5);

        numx_real_t ax[3]={1,0,0}, bx[3]={0,1,0};
        numx_vec_dot(ax,bx,3,&r);
        chk_f  (ctx,"vec_dot orthogonal = 0",  r, (numx_real_t)0.0, (numx_real_t)1e-6);

        numx_real_t r2;
        numx_vec_dot(a3,b3,3,&r);
        numx_vec_dot(b3,a3,3,&r2);
        chk_true(ctx,"vec_dot commutative", numxt_absf(r-r2)<(numx_real_t)1e-6);

        numx_real_t v[2]={3,4};
        numx_vec_dot(v,v,2,&r);
        chk_f  (ctx,"vec_dot self=[3,4] = 25", r, (numx_real_t)25.0, (numx_real_t)1e-5);

        numx_real_t a1[1]={7}, b1[1]={3};
        numx_vec_dot(a1,b1,1,&r);
        chk_f  (ctx,"vec_dot n=1: 7*3=21", r, (numx_real_t)21.0, (numx_real_t)1e-5);

        numx_real_t z3[3]={0,0,0};
        numx_vec_dot(z3,b3,3,&r);
        chk_f  (ctx,"vec_dot zero vec = 0", r, (numx_real_t)0.0, (numx_real_t)1e-6);

        chk_rc (ctx,"vec_dot null-a",  numx_vec_dot(NULL,b3,3,&r), NUMX_ERR_NULL_PTR);
        chk_rc (ctx,"vec_dot null-b",  numx_vec_dot(a3,NULL,3,&r), NUMX_ERR_NULL_PTR);
        chk_rc (ctx,"vec_dot null-out",numx_vec_dot(a3,b3,3,NULL), NUMX_ERR_NULL_PTR);
        chk_rc (ctx,"vec_dot n=0",     numx_vec_dot(a3,b3,0,&r),   NUMX_ERR_INVALID_ARG);
    }

    /* ── vec_norm ─────────────────────────────────────────────── */
    suite("linalg / vec_norm");
    {
        numx_real_t v[2]={3,4};
        chk_rc(ctx, "vec_norm L2 rc", numx_vec_norm(v,2,NUMX_NORM_L2,&r), NUMX_OK);
        chk_f (ctx,"vec_norm L2 [3,4]=5",   r, (numx_real_t)5.0, (numx_real_t)1e-5);

        numx_real_t v3[3]={3,-4,0};
        numx_vec_norm(v3,3,NUMX_NORM_L1,&r);
        chk_f (ctx,"vec_norm L1 [3,-4,0]=7", r, (numx_real_t)7.0, (numx_real_t)1e-5);

        numx_real_t v3b[3]={3,-5,4};
        numx_vec_norm(v3b,3,NUMX_NORM_INF,&r);
        chk_f (ctx,"vec_norm Linf [3,-5,4]=5", r, (numx_real_t)5.0, (numx_real_t)1e-5);

        numx_real_t unit[3]={1,0,0};
        numx_vec_norm(unit,3,NUMX_NORM_L2,&r);
        chk_f (ctx,"vec_norm unit = 1", r, (numx_real_t)1.0, (numx_real_t)1e-6);

        numx_real_t zero[3]={0,0,0};
        numx_vec_norm(zero,3,NUMX_NORM_L2,&r);
        chk_f (ctx,"vec_norm zero = 0", r, (numx_real_t)0.0, (numx_real_t)1e-6);

        chk_rc(ctx, "vec_norm null-a",   numx_vec_norm(NULL,3,NUMX_NORM_L2,&r), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "vec_norm null-out", numx_vec_norm(v,2,NUMX_NORM_L2,NULL),  NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "vec_norm bad-type", numx_vec_norm(v,2,(numx_norm_t)99,&r), NUMX_ERR_INVALID_ARG);
    }

    /* ── vec_cross3 ───────────────────────────────────────────── */
    suite("linalg / vec_cross3");
    {
        numx_real_t a[3]={1,0,0}, b[3]={0,1,0}, c[3];
        chk_rc(ctx, "vec_cross3 rc", numx_vec_cross3(a,b,c), NUMX_OK);
        chk_f (ctx,"vec_cross3 x×y [0]", c[0], (numx_real_t)0.0, (numx_real_t)1e-6);
        chk_f (ctx,"vec_cross3 x×y [1]", c[1], (numx_real_t)0.0, (numx_real_t)1e-6);
        chk_f (ctx,"vec_cross3 x×y [2]", c[2], (numx_real_t)1.0, (numx_real_t)1e-6);

        numx_real_t a2[3]={1,2,3}, b2[3]={4,5,6}, axb[3], bxa[3];
        numx_vec_cross3(a2,b2,axb);
        numx_vec_cross3(b2,a2,bxa);
        chk_f (ctx,"vec_cross3 anticomm [0]", axb[0]+bxa[0], (numx_real_t)0.0, (numx_real_t)1e-5);
        chk_f (ctx,"vec_cross3 anticomm [1]", axb[1]+bxa[1], (numx_real_t)0.0, (numx_real_t)1e-5);
        chk_f (ctx,"vec_cross3 anticomm [2]", axb[2]+bxa[2], (numx_real_t)0.0, (numx_real_t)1e-5);

        numx_real_t pa[3]={2,0,0}, pb[3]={5,0,0}, pc[3];
        numx_vec_cross3(pa,pb,pc);
        chk_f (ctx,"vec_cross3 parallel [0]", pc[0], (numx_real_t)0.0, (numx_real_t)1e-6);
        chk_f (ctx,"vec_cross3 parallel [2]", pc[2], (numx_real_t)0.0, (numx_real_t)1e-6);

        numx_real_t aa[3]={1,0,0}, ba[3]={0,1,0};
        numx_vec_cross3(aa,ba,aa);
        chk_f (ctx,"vec_cross3 alias-safe [0]", aa[0], (numx_real_t)0.0, (numx_real_t)1e-6);
        chk_f (ctx,"vec_cross3 alias-safe [1]", aa[1], (numx_real_t)0.0, (numx_real_t)1e-6);
        chk_f (ctx,"vec_cross3 alias-safe [2]", aa[2], (numx_real_t)1.0, (numx_real_t)1e-6);

        chk_rc(ctx, "vec_cross3 null-a", numx_vec_cross3(NULL,b,c), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "vec_cross3 null-b", numx_vec_cross3(a,NULL,c), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "vec_cross3 null-c", numx_vec_cross3(a,b,NULL), NUMX_ERR_NULL_PTR);
    }

    /* ── mat_mul ──────────────────────────────────────────────── */
    suite("linalg / mat_mul");
    {
        numx_real_t A[4]={1,2,3,4}, B[4]={5,6,7,8}, C[4];
        chk_rc(ctx, "mat_mul 2×2 rc", numx_mat_mul(A,2,2,B,2,2,C), NUMX_OK);
        chk_f (ctx,"mat_mul 2×2 C[0,0]=19", C[0], (numx_real_t)19.0, (numx_real_t)1e-5);
        chk_f (ctx,"mat_mul 2×2 C[0,1]=22", C[1], (numx_real_t)22.0, (numx_real_t)1e-5);
        chk_f (ctx,"mat_mul 2×2 C[1,0]=43", C[2], (numx_real_t)43.0, (numx_real_t)1e-5);
        chk_f (ctx,"mat_mul 2×2 C[1,1]=50", C[3], (numx_real_t)50.0, (numx_real_t)1e-5);

        numx_real_t A23[6]={1,2,3,4,5,6}, B32[6]={7,8,9,10,11,12}, C22[4];
        numx_mat_mul(A23,2,3,B32,3,2,C22);
        chk_f (ctx,"mat_mul 2x3*3x2 C[0,0]=58",  C22[0], (numx_real_t)58.0,  (numx_real_t)1e-4);
        chk_f (ctx,"mat_mul 2x3*3x2 C[0,1]=64",  C22[1], (numx_real_t)64.0,  (numx_real_t)1e-4);
        chk_f (ctx,"mat_mul 2x3*3x2 C[1,0]=139", C22[2], (numx_real_t)139.0, (numx_real_t)1e-4);
        chk_f (ctx,"mat_mul 2x3*3x2 C[1,1]=154", C22[3], (numx_real_t)154.0, (numx_real_t)1e-4);

        numx_real_t I[4]={1,0,0,1}, AI[4];
        numx_mat_mul(A,2,2,I,2,2,AI);
        chk_f (ctx,"mat_mul A*I=A [0]", AI[0], A[0], (numx_real_t)1e-6);
        chk_f (ctx,"mat_mul A*I=A [1]", AI[1], A[1], (numx_real_t)1e-6);
        chk_f (ctx,"mat_mul A*I=A [2]", AI[2], A[2], (numx_real_t)1e-6);
        chk_f (ctx,"mat_mul A*I=A [3]", AI[3], A[3], (numx_real_t)1e-6);

        numx_real_t bad[6]={0};
        chk_rc(ctx, "mat_mul dim-mismatch", numx_mat_mul(A23,2,3,B,2,2,C), NUMX_ERR_INVALID_ARG);
        (void)bad;
        chk_rc(ctx, "mat_mul null-A", numx_mat_mul(NULL,2,2,B,2,2,C), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "mat_mul null-C", numx_mat_mul(A,2,2,B,2,2,NULL), NUMX_ERR_NULL_PTR);
    }

    /* ── mat_transpose ────────────────────────────────────────── */
    suite("linalg / mat_transpose");
    {
        numx_real_t A[6]={1,2,3,4,5,6}, AT[6];
        chk_rc(ctx, "mat_transpose 2x3 rc", numx_mat_transpose(A,2,3,AT), NUMX_OK);
        chk_f (ctx,"mat_transpose AT[0]=1", AT[0], (numx_real_t)1.0, (numx_real_t)1e-6);
        chk_f (ctx,"mat_transpose AT[1]=4", AT[1], (numx_real_t)4.0, (numx_real_t)1e-6);
        chk_f (ctx,"mat_transpose AT[2]=2", AT[2], (numx_real_t)2.0, (numx_real_t)1e-6);
        chk_f (ctx,"mat_transpose AT[3]=5", AT[3], (numx_real_t)5.0, (numx_real_t)1e-6);
        chk_f (ctx,"mat_transpose AT[4]=3", AT[4], (numx_real_t)3.0, (numx_real_t)1e-6);
        chk_f (ctx,"mat_transpose AT[5]=6", AT[5], (numx_real_t)6.0, (numx_real_t)1e-6);

        numx_real_t ATT[6];
        numx_mat_transpose(AT,3,2,ATT);
        int ok=1;
        for(int i=0;i<6;i++) if(numxt_absf(ATT[i]-A[i])>(numx_real_t)1e-5){ok=0;break;}
        chk_true(ctx,"mat_transpose T(T(A))=A", ok);

        numx_real_t S[4]={1,2,3,4};
        chk_rc(ctx, "mat_transpose_sq 2x2 rc", numx_mat_transpose_sq(S,2), NUMX_OK);
        chk_f (ctx,"mat_transpose_sq S[0]=1", S[0], (numx_real_t)1.0, (numx_real_t)1e-6);
        chk_f (ctx,"mat_transpose_sq S[1]=3", S[1], (numx_real_t)3.0, (numx_real_t)1e-6);
        chk_f (ctx,"mat_transpose_sq S[2]=2", S[2], (numx_real_t)2.0, (numx_real_t)1e-6);
        chk_f (ctx,"mat_transpose_sq S[3]=4", S[3], (numx_real_t)4.0, (numx_real_t)1e-6);

        numx_real_t S3[9]={1,2,3,4,5,6,7,8,9};
        numx_real_t orig[9]; memcpy(orig,S3,sizeof(S3));
        numx_mat_transpose_sq(S3,3);
        numx_mat_transpose_sq(S3,3);
        int ok2=1;
        for(int i=0;i<9;i++) if(numxt_absf(S3[i]-orig[i])>(numx_real_t)1e-5){ok2=0;break;}
        chk_true(ctx,"mat_transpose_sq twice=identity", ok2);

        chk_rc(ctx, "mat_transpose null",    numx_mat_transpose(NULL,2,3,AT),  NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "mat_transpose_sq null", numx_mat_transpose_sq(NULL,2),    NUMX_ERR_NULL_PTR);
    }

    /* ── mat_det ──────────────────────────────────────────────── */
    suite("linalg / mat_det");
    {
        numx_real_t A1[1]={7};
        chk_rc(ctx, "mat_det 1×1 rc", numx_mat_det(A1,1,&r), NUMX_OK);
        chk_f (ctx,"mat_det 1×1 = 7",   r, (numx_real_t)7.0, (numx_real_t)1e-5);

        numx_real_t A2[4]={1,2,3,4};
        numx_mat_det(A2,2,&r);
        chk_f (ctx,"mat_det 2×2 = -2",   r, -(numx_real_t)2.0, (numx_real_t)1e-5);

        numx_real_t A3[9]={1,2,3,4,5,6,7,8,10};
        numx_mat_det(A3,3,&r);
        chk_f (ctx,"mat_det 3×3 = -3",   r, -(numx_real_t)3.0, (numx_real_t)1e-4);

        numx_real_t I3[9]={1,0,0,0,1,0,0,0,1};
        numx_mat_det(I3,3,&r);
        chk_f (ctx,"mat_det I = 1",      r, (numx_real_t)1.0,  (numx_real_t)1e-5);

        numx_real_t Sg[4]={1,2,2,4};
        numx_mat_det(Sg,2,&r);
        chk_f (ctx,"mat_det singular = 0", r, (numx_real_t)0.0, (numx_real_t)1e-4);

        chk_rc(ctx, "mat_det null-A",   numx_mat_det(NULL,2,&r), NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "mat_det null-out", numx_mat_det(A2,2,NULL), NUMX_ERR_NULL_PTR);
    }

    /* ── lu_decompose / lu_solve ──────────────────────────────── */
    suite("linalg / lu_solve");
    {
        numx_real_t A[9]={2,1,-1,-3,-1,2,-2,1,2};
        numx_real_t b[3]={8,-11,-3}, LU[9], x[3];
        numx_idx_t piv[3];
        chk_rc(ctx, "lu_decompose 3×3 rc", numx_lu_decompose(A,3,LU,piv), NUMX_OK);
        chk_rc(ctx, "lu_solve 3×3 rc",     numx_lu_solve(LU,piv,3,b,x),  NUMX_OK);
        chk_f (ctx,"lu_solve x[0]=2",  x[0],  (numx_real_t)2.0, (numx_real_t)1e-4);
        chk_f (ctx,"lu_solve x[1]=3",  x[1],  (numx_real_t)3.0, (numx_real_t)1e-4);
        chk_f (ctx,"lu_solve x[2]=-1", x[2], -(numx_real_t)1.0, (numx_real_t)1e-4);

        numx_real_t AI[4]={1,0,0,1}, bI[2]={5,7}, LUI[4], xI[2];
        numx_idx_t pivI[2];
        numx_lu_decompose(AI,2,LUI,pivI);
        numx_lu_solve(LUI,pivI,2,bI,xI);
        chk_f (ctx,"lu_solve I*x x[0]=5", xI[0], (numx_real_t)5.0, (numx_real_t)1e-5);
        chk_f (ctx,"lu_solve I*x x[1]=7", xI[1], (numx_real_t)7.0, (numx_real_t)1e-5);

        numx_real_t Ax[3];
        numx_mat_mul(A,3,3,x,3,1,Ax);
        chk_f (ctx,"lu_solve residual[0]", Ax[0], b[0], (numx_real_t)1e-3);
        chk_f (ctx,"lu_solve residual[1]", Ax[1], b[1], (numx_real_t)1e-3);
        chk_f (ctx,"lu_solve residual[2]", Ax[2], b[2], (numx_real_t)1e-3);

        numx_real_t Asg[4]={1,2,2,4}, LUsg[4];
        numx_idx_t pivsg[2];
        chk_rc(ctx, "lu_decompose singular", numx_lu_decompose(Asg,2,LUsg,pivsg), NUMX_ERR_SINGULAR);

        chk_rc(ctx, "lu_decompose null-A",  numx_lu_decompose(NULL,2,LUI,pivI),  NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "lu_decompose null-LU", numx_lu_decompose(AI,2,NULL,pivI),   NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "lu_solve null-LU",     numx_lu_solve(NULL,pivI,2,bI,xI),   NUMX_ERR_NULL_PTR);
        chk_rc(ctx, "lu_solve null-x",      numx_lu_solve(LUI,pivI,2,bI,NULL),  NUMX_ERR_NULL_PTR);
    }
}
