# NTT external-reference validation

`tests/test_ntt.c`'s original known-answer tests checked the fast NTT-based
polynomial multiply against `poly_mul_ref()`, a naive O(n^2) reference
multiplication written for this project. That is self-consistency, not proof
against an independent ground truth: a shared systematic error (wrong root of
unity, wrong modulus constant) in both the fast and "reference" path would
pass every test. This directory closes that gap by cross-validating `numx`'s
NTT against an external, independently-citable implementation.

## What's here

`generate_ntt_pqclean_vectors.c` links against [PQClean](https://github.com/PQClean/PQClean)'s
`ml-kem-512` "clean" reference implementation (the FIPS 203-standardized
successor to CRYSTALS-Kyber-512) and produces `tests/vectors/ntt_pqclean_kat.h`,
a frozen fixture of forward-NTT and full negacyclic-polynomial-multiply
outputs. `tests/test_ntt.c` checks `numx_ntt_forward` and `numx_ntt_polymul`
against that fixture bit-for-bit.

This generator is validation tooling only. It is never built as part of the
numx library or its CI, so PQClean is never a build or runtime dependency of
numx, and the zero-external-dependency guarantee is untouched. CI only reads
the committed, frozen header.

## Provenance

- Source: https://github.com/PQClean/PQClean
- Pinned commit: `202a8f96315f9ed219387a50f7e40d04af037ea8` (2026-05-14)
- Files used: `crypto_kem/ml-kem-512/clean/ntt.c`, `ntt.h`, `reduce.c`,
  `reduce.h`, `params.h` (unmodified, used only to build the generator)

## Why the values line up despite different internal representations

`numx`'s `ntt.c` works entirely in "plain" domain: `priv_zetas[k] = 17^brv(k)
mod q`. PQClean's reference implementation works in Montgomery domain for
speed: its `zetas[k]` table is `17^brv(k) * R mod q` (`R = 2^16 mod q`), and
its `basemul`/`invntt_tomont` deliberately introduce and cancel factors of
`R` across the multiply-NTT-INTT chain. Working through the Montgomery
reduction identity (`fqmul(a,b) = a*b*R^-1 mod q`) shows that the sequence

```
poly_ntt(a); poly_ntt(b);
poly_basemul_montgomery(c, a, b);
poly_invntt_tomont(c);
poly_reduce(c);
```

produces exactly the plain negacyclic product `a*b mod (x^256+1, q)`, with no
leftover `R` factor. This is not a bespoke construction: it is the same
chain PQClean's own `indcpa.c` uses for its real polynomial multiplications
(see `polyvec_basemul_acc_montgomery` + `poly_invntt_tomont` + `poly_reduce`
in `indcpa.c`). `generate_ntt_pqclean_vectors.c` replicates that chain
directly against the raw `ntt.c`/`reduce.c` primitives (no hash/KEM
dependencies needed), then canonicalizes every output coefficient into
`[0, q)` for comparison against `numx`'s own `[0, q)` convention.

Confirmed by direct comparison before adoption: `numx_ntt_forward` and
`numx_ntt_polymul` match the PQClean-derived fixture bit-for-bit across all
generated vectors.

## Regenerating the fixture

```sh
git clone https://github.com/PQClean/PQClean.git
cd PQClean && git checkout 202a8f96315f9ed219387a50f7e40d04af037ea8
PQD=crypto_kem/ml-kem-512/clean

cc -std=c99 -O2 -Wall -Wextra \
  -I "$PQD" -I /path/to/numx/include \
  /path/to/numx/validation/reference/ntt/generate_ntt_pqclean_vectors.c \
  "$PQD/ntt.c" "$PQD/reduce.c" \
  -o generate_ntt_pqclean_vectors

./generate_ntt_pqclean_vectors > /path/to/numx/tests/vectors/ntt_pqclean_kat.h
```

Only needed if the seeds or vector count in `generate_ntt_pqclean_vectors.c`
change, or to independently re-verify the committed fixture.
