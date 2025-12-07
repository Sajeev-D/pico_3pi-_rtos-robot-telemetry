#include "performance_profiler.h"
#include "hardware/timer.h"  // RP2040 timer
#include <stdio.h>
#include <math.h>

void profiler_init(void) {
    // No initialization needed - timer is always running
    // Just verify it's working
    uint64_t test = time_us_64();
    (void)test;  // Timer is ready
}

uint32_t profiler_start(void) {
    // Return current microsecond count (lower 32 bits is enough for our measurements)
    return (uint32_t)time_us_64();
}

uint32_t profiler_end(uint32_t start_us) {
    uint32_t end_us = (uint32_t)time_us_64();
    return end_us - start_us;  // Returns elapsed microseconds
}

void profiler_update_metrics(PerformanceMetrics *metrics, uint32_t cycles) {
    // First sample? Initialize min/max
    if (metrics->sample_count == 0) {
        metrics->min_cycles = cycles;
        metrics->max_cycles = cycles;
    } else {
        // Update min/max
        if (cycles < metrics->min_cycles) {
            metrics->min_cycles = cycles;
        }
        if (cycles > metrics->max_cycles) {
            metrics->max_cycles = cycles;
        }
    }

    // Update totals
    metrics->total_cycles += cycles;
    metrics->sample_count++;

    // Calculate average
    metrics->avg_cycles = (float)metrics->total_cycles / metrics->sample_count;
}

void profiler_print_metrics(const char *label, const PerformanceMetrics *metrics) {
    printf("\n=== %s ===\n", label);
    printf("Samples:      %lu\n", metrics->sample_count);
    printf("Min time:     %lu us\n", metrics->min_cycles);
    printf("Max time:     %lu us\n", metrics->max_cycles);
    printf("Avg time:     %.2f us\n", metrics->avg_cycles);
    printf("Consistency:  %.2f us spread (max-min)\n", 
           (float)(metrics->max_cycles - metrics->min_cycles));
}