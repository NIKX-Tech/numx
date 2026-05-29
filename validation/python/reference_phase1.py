#!/usr/bin/env python3
"""
reference_phase1.py
Comprehensive reference script for ALL numx Phase 1 functions.
Uses numpy and scipy to compute reference values.
Compares against numx x86 output values — tolerance 1e-5.

numx commit: d81b386
Generated: 2026-05-25
Platform: Python 3 + numpy + scipy
"""

import sys
import math

try:
    import numpy as np
    from scipy import integrate as sci_integrate
    from scipy import interpolate as sci_interp
    from scipy import optimize as sci_opt
    from scipy.integrate import odeint
except ImportError as e:
    print(f"Missing dependency: {e}")
    print("Install: pip install numpy scipy")
    sys.exit(1)


# ══════════════════════════════════════════════════════════════════════
# Section 1: linalg
# ══════════════════════════════════════════════════════════════════════
def ref_linalg():
    # Uses float64 as ground-truth double-precision reference.
    # For float32 reference values that match numx's internal precision,
    # see reference_linalg.py.
    print("\n" + "="*70)
    print("1. linalg")
    print("="*70)

    # vec_dot
    print("\n  [vec_dot]")
    a = np.array([1,2,3,4], dtype=np.float64)
    b = np.array([5,6,7,8], dtype=np.float64)
    print(f"  a·b (expected 70) = {np.dot(a,b):.1f}")
    print(f"  x·y orthogonal    = {np.dot([1,0,0],[0,1,0]):.1f}  (expected 0)")

    # vec_norm
    print("\n  [vec_norm]")
    v = np.array([3.0, 4.0])
    print(f"  [3,4] L2={np.linalg.norm(v,2):.6f} (expected 5.000000)")
    print(f"  [3,4] L1={np.linalg.norm(v,1):.6f} (expected 7.000000)")
    print(f"  [3,4] Linf={np.linalg.norm(v,np.inf):.6f} (expected 4.000000)")

    # vec_cross3
    print("\n  [vec_cross3]")
    c = np.cross([1,2,3],[4,5,6])
    print(f"  [1,2,3]×[4,5,6] = {c.tolist()}  (expected [-3,6,-3])")
    print(f"  x×y = {np.cross([1,0,0],[0,1,0]).tolist()}  (expected [0,0,1])")

    # mat_mul
    print("\n  [mat_mul]")
    A = np.array([[1,2],[3,4]], dtype=np.float64)
    B = np.array([[5,6],[7,8]], dtype=np.float64)
    C = A@B
    print(f"  2x2: [[19,22],[43,50]] = {C.tolist()}")

    # mat_det
    print("\n  [mat_det]")
    print(f"  1x1 [[5]] det = {np.linalg.det(np.array([[5.0]])):.6f}  (expected 5.0)")
    print(f"  2x2 [[1,2],[3,4]] det = {np.linalg.det(np.array([[1,2],[3,4]],dtype=float)):.6f}  (expected -2.0)")
    M3 = np.array([[2,1,1],[4,3,3],[8,7,9]], dtype=float)
    print(f"  3x3 det = {np.linalg.det(M3):.6f}")
    M4 = np.array([[4,3,2,1],[3,4,3,2],[2,3,4,3],[1,2,3,4]], dtype=float)
    print(f"  4x4 det = {np.linalg.det(M4):.6f}")

    # lu_solve
    print("\n  [lu_solve  A*x=b]")
    A_lu = np.array([[2,1,1,0],[4,3,3,1],[8,7,9,5],[6,7,9,8]], dtype=float)
    b_lu = np.array([1.0,2.0,4.0,5.0])
    x = np.linalg.solve(A_lu, b_lu)
    print(f"  x = {[f'{xi:.7f}' for xi in x]}")
    print(f"  residual ||Ax-b|| = {np.linalg.norm(A_lu@x - b_lu):.2e}")


# ══════════════════════════════════════════════════════════════════════
# Section 2: stats
# ══════════════════════════════════════════════════════════════════════
def ref_stats():
    print("\n" + "="*70)
    print("2. stats")
    print("="*70)

    data = np.array([2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0], dtype=np.float64)
    print(f"\n  data = {data.tolist()}")
    print(f"  mean             = {np.mean(data):.7f}  (expected 5.0)")
    print(f"  variance (pop)   = {np.var(data, ddof=0):.7f}  (expected 4.0)")
    print(f"  variance (sample)= {np.var(data, ddof=1):.7f}  (expected 4.571429)")
    print(f"  median           = {np.median(data):.7f}  (expected 4.5)")
    print(f"  percentile p25   = {np.percentile(data, 25, method='inverted_cdf'):.7f}")
    print(f"  percentile p50   = {np.percentile(data, 50, method='inverted_cdf'):.7f}")
    print(f"  percentile p75   = {np.percentile(data, 75, method='inverted_cdf'):.7f}")
    print(f"  percentile p100  = {np.percentile(data, 100, method='inverted_cdf'):.7f}  (max = {np.max(data)})")


# ══════════════════════════════════════════════════════════════════════
# Section 3: roots
# ══════════════════════════════════════════════════════════════════════
def ref_roots():
    print("\n" + "="*70)
    print("3. roots")
    print("="*70)

    def f_quad(x): return x**2 - 2
    def f_cubic(x): return x**3 - x - 2

    r1 = sci_opt.brentq(f_quad, 1, 2, xtol=1e-10)
    print(f"\n  bisect/brent x^2-2 on [1,2] = {r1:.10f}  (sqrt(2)={math.sqrt(2):.10f})")
    r2 = sci_opt.brentq(f_cubic, 1, 2, xtol=1e-10)
    print(f"  brent x^3-x-2 on [1,2]      = {r2:.10f}")
    r3 = sci_opt.newton(f_quad, 1.5, fprime=lambda x: 2*x, tol=1e-10)
    print(f"  newton x^2-2 from x0=1.5     = {r3:.10f}")

    # Precision note: single float sqrt(2)
    import struct
    sq2_f32 = struct.unpack('f', struct.pack('f', math.sqrt(2)))[0]
    print(f"  sqrt(2) as float32           = {sq2_f32:.10f}  (max reachable in float)")
    print(f"  NOTE: numx uses float32 by default; expect error ~1e-7 vs double ref")


# ══════════════════════════════════════════════════════════════════════
# Section 4: integrate
# ══════════════════════════════════════════════════════════════════════
def ref_integrate():
    print("\n" + "="*70)
    print("4. integrate")
    print("="*70)

    def f(x): return x**3 + 1.0   # exact integral on [0,1] = 0.25 + 1 = 1.25

    exact, _ = sci_integrate.quad(f, 0, 1)
    print(f"\n  exact integral x^3+1 on [0,1] = {exact:.10f}  (exact: 1.25)")

    # Manual trapezoidal
    x_t100 = np.linspace(0, 1, 101)
    trap_np100 = np.trapz([f(xi) for xi in x_t100], x_t100)
    print(f"  trapezoidal n=100              = {trap_np100:.10f}  (err vs exact: {abs(trap_np100-1.25):.2e})")

    x_t1000 = np.linspace(0, 1, 1001)
    trap_np1000 = np.trapz([f(xi) for xi in x_t1000], x_t1000)
    print(f"  trapezoidal n=1000             = {trap_np1000:.10f}  (err: {abs(trap_np1000-1.25):.2e})")

    # Simpson (scipy.integrate.simpson requires scipy ≥1.6)
    try:
        from scipy.integrate import simpson
        s100 = simpson([f(xi) for xi in x_t100], x=x_t100)
        print(f"  simpson n=100                  = {s100:.10f}  (err: {abs(s100-1.25):.2e})")
    except ImportError:
        pass

    # Gauss-Legendre via scipy.integrate.fixed_quad
    for npts in [2, 4, 8]:
        gval, _ = sci_integrate.fixed_quad(f, 0, 1, n=npts)
        print(f"  gauss npts={npts}                    = {gval:.10f}  (err: {abs(gval-1.25):.2e})")


# ══════════════════════════════════════════════════════════════════════
# Section 5: differentiate
# ══════════════════════════════════════════════════════════════════════
def ref_differentiate():
    print("\n" + "="*70)
    print("5. differentiate")
    print("="*70)

    def f(x): return x**3         # f'(x) = 3x^2; at x=2: f'=12
    exact = 3 * 2.0**2            # = 12.0
    h = 1e-4

    fwd   = (f(2+h) - f(2)) / h
    cen   = (f(2+h) - f(2-h)) / (2*h)
    D_h   = (f(2+h) - f(2-h)) / (2*h)
    D_h2  = (f(2+h/2) - f(2-h/2)) / h
    rich  = (4*D_h2 - D_h) / 3

    print(f"\n  f(x)=x^3 at x=2, exact f'=12.0  (h={h})")
    print(f"  forward    = {fwd:.10f}  err={abs(fwd-exact):.2e}")
    print(f"  central    = {cen:.10f}  err={abs(cen-exact):.2e}")
    print(f"  richardson = {rich:.10f}  err={abs(rich-exact):.2e}")
    print(f"  NOTE: numx uses float32 — forward/central err will be ~1e-3, richardson ~1e-5")


# ══════════════════════════════════════════════════════════════════════
# Section 6: interpolate
# ══════════════════════════════════════════════════════════════════════
def ref_interpolate():
    print("\n" + "="*70)
    print("6. interpolate")
    print("="*70)

    xs = np.array([0.0, 1.0, 2.0, 3.0, 4.0], dtype=np.float64)
    ys = np.array([0.0, 1.0, 4.0, 9.0, 16.0], dtype=np.float64)  # y = x^2

    # linear at x=1.5
    f_lin = sci_interp.interp1d(xs, ys, kind='linear')
    print(f"\n  linear x^2 nodes, eval at 1.5 = {f_lin(1.5):.8f}  (expected 2.5)")

    # cubic spline
    cs = sci_interp.CubicSpline(xs, ys, bc_type='natural')
    print(f"  cubic spline eval at 1.5       = {cs(1.5):.8f}  (exact x^2=2.25)")
    print(f"  cubic spline eval at 2.5       = {cs(2.5):.8f}  (exact x^2=6.25)")

    # chebyshev via Barycentric (scipy)
    def f_func(x): return x**2
    cheb_nodes = sci_interp.BarycentricInterpolator(
        np.cos(np.pi * (2*np.arange(1,9)-1) / 16) * 2 + 2,  # 8 cheb nodes on [0,4]
        [f_func(xi) for xi in np.cos(np.pi * (2*np.arange(1,9)-1) / 16) * 2 + 2]
    )
    print(f"  chebyshev-bary x^2 eval at 1.5 = {cheb_nodes(1.5):.8f}  (exact 2.25)")


# ══════════════════════════════════════════════════════════════════════
# Section 7: poly
# ══════════════════════════════════════════════════════════════════════
def ref_poly():
    print("\n" + "="*70)
    print("7. poly")
    print("="*70)

    # Horner eval: p(x) = x^3 - 6x^2 + 11x - 6
    coeffs = np.array([1, -6, 11, -6], dtype=np.float64)
    p = np.poly1d(coeffs)
    print(f"\n  p(x)=x^3-6x^2+11x-6:")
    for x in [1.0, 2.0, 3.0, 2.5, 0.0]:
        print(f"    p({x}) = {p(x):.8f}  (numx Horner should match exactly)")

    roots = np.roots(coeffs)
    print(f"  roots: {sorted(roots.real.tolist())}  (expected [1.0, 2.0, 3.0])")

    # Degree 8 eval
    c8 = np.array([1,0,0,0,-3,0,0,0,2], dtype=np.float64)
    p8 = np.poly1d(c8)
    print(f"  p8(1.5) = {p8(1.5):.8f}")
    print(f"  p8(2.0) = {p8(2.0):.8f}")


# ══════════════════════════════════════════════════════════════════════
# Section 8: ode
# ══════════════════════════════════════════════════════════════════════
def ref_ode():
    print("\n" + "="*70)
    print("8. ode")
    print("="*70)

    # dy/dt = -y, y(0)=1 — exact: y(t) = e^{-t}
    def decay(y, t): return [-y[0]]
    t_span = np.linspace(0, 1.0, 101)
    sol = odeint(decay, [1.0], t_span)
    exact_t1 = math.exp(-1.0)
    print(f"\n  decay dy/dt=-y, y(0)=1, y(1.0):")
    print(f"    exact  = {exact_t1:.10f}")
    print(f"    scipy  = {sol[-1,0]:.10f}")

    # rk4 manual (100 steps, h=0.01)
    y = 1.0
    for _ in range(100):
        k1 = -y
        k2 = -(y + 0.01*k1/2)
        k3 = -(y + 0.01*k2/2)
        k4 = -(y + 0.01*k3)
        y += (0.01/6) * (k1 + 2*k2 + 2*k3 + k4)
    print(f"    rk4 (h=0.01, 100 steps) = {y:.10f}  err={abs(y-exact_t1):.2e}")

    # harmonic oscillator: [x,v]' = [v, -x], exact x(t)=cos(t)
    def harmonic(y, t): return [y[1], -y[0]]
    sol2 = odeint(harmonic, [1.0, 0.0], [0.0, 1.0])
    exact_x1 = math.cos(1.0)
    exact_v1 = -math.sin(1.0)
    print(f"\n  harmonic [x,v]'=[v,-x], x(0)=1,v(0)=0, at t=1.0:")
    print(f"    x(1) exact={exact_x1:.10f}  scipy={sol2[-1,0]:.10f}")
    print(f"    v(1) exact={exact_v1:.10f}  scipy={sol2[-1,1]:.10f}")

    # energy conservation check
    x1, v1 = sol2[-1]
    energy = 0.5*(x1**2 + v1**2)
    print(f"    energy at t=1: {energy:.10f}  (should be 0.5, initial was 0.5)")


# ══════════════════════════════════════════════════════════════════════
# Main
# ══════════════════════════════════════════════════════════════════════
if __name__ == "__main__":
    print("numx Phase 1 — Python/numpy/scipy reference values")
    print(f"Python {sys.version.split()[0]}  numpy {np.__version__}")
    try:
        import scipy
        print(f"scipy {scipy.__version__}")
    except Exception:
        pass

    ref_linalg()
    ref_stats()
    ref_roots()
    ref_integrate()
    ref_differentiate()
    ref_interpolate()
    ref_poly()
    ref_ode()

    print("\n" + "="*70)
    print("Reference values generated. Fill numx outputs into result .md files")
    print("and compare. Flag any absolute error > 1e-5 (float32 limit is ~1e-7).")
    print("="*70)
