#include <stdio.h>
#include "esp32_performance_tests.h"

void run_performance_tests(void)
{
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  numx PERFORMANCE BENCHMARK SUITE                           ║\n");
    printf("║  ESP32-S3 @ 240 MHz · float32 · ESP-IDF v5.5.2             ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    bench_phase1();
    bench_phase2();

    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  Benchmark complete.                                         ║\n");
    printf("║  Paste each >>> block into the matching results/*.md file.  ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
}
