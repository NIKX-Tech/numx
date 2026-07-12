# Contributing to numx

> Read this fully before writing a single line of code.

---

## Non-negotiable rules

Every file, every function, every commit must obey all ten rules. No exceptions.

1. **Pure C99 only.** No GCC extensions (`__attribute__`, etc.) unless wrapped in `#ifdef`.
2. **Zero dynamic memory allocation.** No `malloc`, `calloc`, `realloc`, `free` — ever.
3. **No external dependencies.** Not even `<math.h>` in core modules — provide own primitives.
4. **No global mutable state.** All functions must be reentrant and thread-safe by design.
5. **Full Doxygen header on every public function.** See template below.
6. **Every module needs a test file and a benchmark file.** No exceptions.
7. **Errors returned as status codes.** Never silently ignore a failure.
8. **Never use `float` or `double` directly in headers.** Use `numx_real_t`.
9. **Provide fixed-point and floating-point variants** where performance matters.
10. **NULL-check every pointer parameter** and return `NUMX_ERR_NULL_PTR`.

---

## Adding a new module

Each new algorithm requires exactly four deliverables — all four or none:

| Deliverable | Path |
|---|---|
| Public header | `include/numx/<module>.h` |
| Implementation | `src/<module>.c` |
| Unity test suite | `tests/test_<module>.c` |
| Algorithm doc page | `docs/algorithms/<name>.md` |

Then register in `CMakeLists.txt` (three places: sources, test sources, bench sources)
and in `tests/test_runner.c` and `benchmarks/bench_runner.c`.

---

## Function contract template

```c
/**
 * @brief  One-line summary.
 *
 * 2–4 sentences: what it computes, which algorithm, precision notes.
 *
 * Mathematical definition:
 *   result = integral from a to b of f(x) dx
 *
 * Reference: Author, Title, Venue, Year. Full citation:
 * docs/algorithms/[module].md#references.
 *
 * @param[in]  param1  Description, valid range.
 * @param[out] result  Pointer to store result. Must not be NULL.
 *
 * @return NUMX_OK on success.
 *         NUMX_ERR_NULL_PTR   if any pointer argument is NULL.
 *         NUMX_ERR_INVALID_ARG if [condition].
 *         NUMX_ERR_NO_CONVERGE if [condition].
 *
 * @note   Important usage note or precision tradeoff.
 *
 * @example
 *   numx_real_t result;
 *   numx_status_t s = numx_function(arg1, &result);
 *   if (s != NUMX_OK) { / handle error / }
 */
numx_status_t numx_function(numx_real_t param1, numx_real_t *result);
```

---

## Citing sources

Every function implementing a published algorithm cites that source inline in its
docstring (short form, per the template above); the full bibliographic entry lives in
the module's `docs/algorithms/[name].md#references`, not duplicated in the code. This
applies equally to a function you wrote from a textbook and to a fix contributed by
someone outside the core team (e.g. a community-submitted correctness or performance
fix): the docstring should say what it's based on either way. `docs/algorithms/*.md`
formula sections carry the same `[n]` marker inline, next to the formula, not only in
a bottom References list, so a reader doesn't have to hunt for which citation backs
which equation.

Modules with an available external reference implementation or published dataset
should include a `validation/reference/<module>/` directory: a small generator script
(any language/tool) that produces a frozen test-vector fixture from that external
source, checked into `tests/vectors/`, with a README documenting provenance (source,
pinned commit/version, regeneration steps). This is validation tooling only; it is
never a build or runtime dependency of the library itself. See
`validation/reference/ntt/` for the reference instance of this pattern.

---

## Test file structure

Tests are organized into four levels:

- **L1** — Known-answer tests: verified against analytical truth, or a reference
  implementation. For crypto/security-critical modules, "reference implementation"
  means an external, independently-citable one (published test vectors, or an
  established third-party implementation), not code written for this project. An
  internally-written reference (e.g. a naive O(n^2) implementation checked against an
  optimized one) is still valid for general numerical modules, but is self-consistency,
  not independent proof, and is not sufficient on its own for crypto modules.
- **L2** — Property tests: mathematical invariants that must always hold.
- **L3** — Edge cases: zero, negative, extremes, near-singular, empty input.
- **L4** — Error handling: null pointers and invalid args return the correct error code.

Each `tests/test_<module>.c` must expose `void numx_test_<module>(void)` (no `main`).
`tests/test_runner.c` owns `main()` and calls every suite.

---

## Commit checklist

Before opening a PR:

- [ ] `cmake -B build && cmake --build build --parallel` passes with zero warnings
- [ ] `ctest --test-dir build --output-on-failure` passes with zero failures
- [ ] All four deliverables present (header, impl, tests, docs page)
- [ ] No `malloc`/`free` anywhere in new code (`grep -r malloc src/`)
- [ ] No raw `float`/`double` in any public header
- [ ] Every public function has a Doxygen comment
- [ ] `NUMX_ERR_NULL_PTR` returned for every NULL pointer argument

---

## On AI-assisted development

numx is built with Claude Code as a tool, used under direction and reviewed at every
step, not run unsupervised. Concretely: implementation scaffolding, multi-platform
validation runs, sanitizer testing, and documentation are commonly done with AI
assistance. Architecture decisions (zero allocation, no external dependencies, module
boundaries), the underlying math, and validation methodology are human-directed, and
every change is reviewed before it ships, including catching AI's own mistakes along
the way. If you're evaluating this project and that changes your assessment of it,
that's a reasonable reaction to have, this note exists so you don't have to go dig for
the answer.

The prompt pattern below is the actual template used when adding a new module.

## Prompt pattern for Claude Code

When implementing a new algorithm, use this exact prompt:

```
Implement [P#.## — numx_function_name] for the numx library.

Context:
- Language: C99 strictly, no dynamic allocation, no external dependencies
- All types from include/numx/numx_types.h (numx_real_t, numx_status_t, etc.)
- Follow the function contract template in CONTRIBUTING.md exactly

Deliverables (all four required):
1. include/numx/[module].h
2. src/[module].c
3. tests/test_[module].c  (expose void numx_test_<module>(void), no main)
4. docs/algorithms/[name].md

Algorithm: [algorithm name]
Mathematical definition: [formula in plain ASCII]
Reference: [paper or textbook]
Known test vectors: [input → expected output pairs for L1 tests]

Do not use malloc, free, or any OS calls.
Do not introduce new dependencies.
All array sizes must be compile-time constants or passed as parameters.
```
