#!/usr/bin/env python3
"""
reference_linalg.py
Reference values for numx linalg Phase 1 functions using numpy.
Compare these against numx output to verify precision (tolerance 1e-5).

numx commit: d81b386
Generated: 2026-05-25
"""

import numpy as np
import sys

TOL = 1e-5
PASS = "✅ PASS"
FAIL = "❌ FAIL"


def check(label, numx_val, ref_val, tol=TOL):
    err = abs(numx_val - ref_val)
    status = PASS if err <= tol else FAIL
    print(f"  {status}  {label:40s}  numx={numx_val:.8f}  ref={ref_val:.8f}  err={err:.2e}")
    return err <= tol


def main():
    results = []
    print("=" * 75)
    print("numx linalg — Python/numpy reference")
    print("=" * 75)

    # ── vec_dot ────────────────────────────────────────────────────────
    print("\n[vec_dot]")
    a = np.array([1.0, 2.0, 3.0, 4.0], dtype=np.float32)
    b = np.array([5.0, 6.0, 7.0, 8.0], dtype=np.float32)
    ref = float(np.dot(a, b))
    print(f"  a={a.tolist()}, b={b.tolist()}")
    print(f"  reference result = {ref}  (expected: 70.0)")
    # Orthogonal vectors
    a2 = np.array([1.0, 0.0, 0.0], dtype=np.float32)
    b2 = np.array([0.0, 1.0, 0.0], dtype=np.float32)
    ref2 = float(np.dot(a2, b2))
    print(f"  orthogonal dot = {ref2}  (expected: 0.0)")

    # ── vec_norm ───────────────────────────────────────────────────────
    print("\n[vec_norm]")
    v = np.array([3.0, 4.0], dtype=np.float32)
    l2 = float(np.linalg.norm(v, 2))
    l1 = float(np.linalg.norm(v, 1))
    linf = float(np.linalg.norm(v, np.inf))
    print(f"  v=[3,4]: L2={l2:.8f} (expected 5.0), L1={l1:.8f} (expected 7.0), Linf={linf:.8f} (expected 4.0)")

    v2 = np.array([1.0, 2.0, 3.0, 4.0, 5.0], dtype=np.float32)
    l2_2 = float(np.linalg.norm(v2, 2))
    print(f"  v=[1..5]: L2={l2_2:.8f}")

    # ── vec_cross3 ─────────────────────────────────────────────────────
    print("\n[vec_cross3]")
    ax = np.array([1.0, 0.0, 0.0], dtype=np.float32)
    ay = np.array([0.0, 1.0, 0.0], dtype=np.float32)
    cross = np.cross(ax, ay).tolist()
    print(f"  x × y = {cross}  (expected [0,0,1])")

    a3 = np.array([1.0, 2.0, 3.0], dtype=np.float32)
    b3 = np.array([4.0, 5.0, 6.0], dtype=np.float32)
    cross2 = np.cross(a3, b3).tolist()
    print(f"  [1,2,3] × [4,5,6] = {cross2}  (expected [-3,6,-3])")

    # ── mat_mul ────────────────────────────────────────────────────────
    print("\n[mat_mul]")
    A = np.array([[1, 2], [3, 4]], dtype=np.float32)
    B = np.array([[5, 6], [7, 8]], dtype=np.float32)
    C = A @ B
    print(f"  2x2 A@B =\n{C}")
    print(f"  (expected [[19,22],[43,50]])")

    A2 = np.array([[1,2,3],[4,5,6]], dtype=np.float32)
    B2 = np.array([[7,8],[9,10],[11,12]], dtype=np.float32)
    C2 = A2 @ B2
    print(f"  2x3 @ 3x2 =\n{C2}")

    # ── mat_det ────────────────────────────────────────────────────────
    print("\n[mat_det]")
    for n, M in [
        (2, np.array([[1,2],[3,4]], dtype=np.float64)),
        (3, np.array([[2,1,1],[4,3,3],[8,7,9]], dtype=np.float64)),
        (4, np.array([[4,3,2,1],[3,4,3,2],[2,3,4,3],[1,2,3,4]], dtype=np.float64)),
    ]:
        d = np.linalg.det(M)
        print(f"  {n}x{n} det = {d:.8f}")

    # ── lu_solve ───────────────────────────────────────────────────────
    print("\n[lu_solve  A*x=b]")
    A_lu = np.array([[2,1,1,0],[4,3,3,1],[8,7,9,5],[6,7,9,8]], dtype=np.float64)
    b_lu = np.array([1,2,4,5], dtype=np.float64)
    x_ref = np.linalg.solve(A_lu, b_lu)
    print(f"  x_ref = {x_ref.tolist()}")
    residual = np.linalg.norm(A_lu @ x_ref - b_lu)
    print(f"  residual ||Ax-b|| = {residual:.2e}  (should be ~0)")

    # 3x3 textbook
    A3 = np.array([[2,1,-1],[4,1,1],[2,-1,1]], dtype=np.float64)
    b3 = np.array([8,12,6], dtype=np.float64)
    x3 = np.linalg.solve(A3, b3)
    print(f"  3x3 textbook x = {x3.tolist()}")

    print("\nAll reference values printed — compare against numx output manually or")
    print("via the numx_compare_linalg.c fixture with above expected values.\n")


if __name__ == "__main__":
    main()
