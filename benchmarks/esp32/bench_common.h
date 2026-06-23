#ifndef NUMX_BENCH_COMMON_H
#define NUMX_BENCH_COMMON_H

#include <stdio.h>
#include "esp_timer.h"

#define BENCH_N_BIG    10000
#define BENCH_N_MED     5000
#define BENCH_N_SMALL   1000
#define BENCH_N_TINY     100

static inline void bprint(const char *lbl, int64_t us, int n)
{
    int64_t ns = (us * 1000LL) / n;
    printf("| %-44s | %6d | %8lld us | %7lld ns |\n",
           lbl, n, (long long)us, (long long)ns);
}

static inline void bench_section(const char *target_md)
{
    printf("\n  >>> results/%s\n", target_md);
    printf("  | %-44s | N      | Total us | ns/call  |\n", "Function");
    printf("  |%-46s|--------|----------|----------|\n",
           "----------------------------------------------");
}

#define BENCH(lbl, n, body) do { \
    int64_t _t0 = esp_timer_get_time(); \
    for (int _i = 0; _i < (n); _i++) { body; } \
    int64_t _t1 = esp_timer_get_time(); \
    printf("  "); bprint((lbl), _t1 - _t0, (n)); \
} while (0)

#endif /* NUMX_BENCH_COMMON_H */
