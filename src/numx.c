/**
 * @file numx.c
 * @brief Library-wide constants for numx.
 *
 * Exports version numbers as link-time constants so downstream code can
 * query the library version at runtime without including numx_types.h.
 * Module source files are added alongside this file as each algorithm is
 * implemented (see CMakeLists.txt).
 */

#include "numx/numx_types.h"

const int numx_version_major = NUMX_VERSION_MAJOR;
const int numx_version_minor = NUMX_VERSION_MINOR;
const int numx_version_patch = NUMX_VERSION_PATCH;
