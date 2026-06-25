/**
 * @file numx.h
 * @brief Master include for the numx library.
 *
 * Include this single header to access the full numx API.
 * Individual module headers may also be included directly.
 */

#ifndef NUMX_H
#define NUMX_H

#include "numx_config.h"
#include "numx_types.h"

/* ── Module headers (uncommented as each module is implemented) ────── */

/* Phase 1 — Foundation */
#include "linalg.h"
#include "stats.h"
#include "roots.h"
#include "integrate.h"
#include "differentiate.h"
#include "interpolate.h"
#include "poly.h"
#include "ode.h"

/* Phase 2 — Signal Processing */
#include "signal.h"
#include "fft.h"

/* Phase 3 — Unicorn Algorithms */
#include "autodiff.h"
#include "compressed_sensing.h"
#include "sketch.h"
#include "ntt.h"

/* ── Library-wide version symbols ──────────────────────────────────── */

/**
 * @brief Version components exported as link-time constants.
 *
 * Use these when the compile-time macros from numx_types.h are not
 * accessible (e.g., when linking against a pre-built numx archive).
 */
extern const int numx_version_major;
extern const int numx_version_minor;
extern const int numx_version_patch;

#endif /* NUMX_H */
