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

## Test file structure

Tests are organized into four levels:

- **L1** — Known-answer tests: verified against analytical truth or a reference implementation.
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
