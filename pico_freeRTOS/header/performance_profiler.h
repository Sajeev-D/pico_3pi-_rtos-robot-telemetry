#ifndef PERFORMANCE_PROFILER_H
#define PERFORMANCE_PROFILER_H

#include <stdint.h>

// Performance metrics structure
typedef struct {
    uint32_t min_cycles;
    uint32_t max_cycles;
    uint32_t total_cycles;
    uint32_t sample_count;
    float avg_cycles;
} PerformanceMetrics;


void profiler_init(void);

/**
 * Start a performance measurement
 * Returns: Starting cycle count
 */
uint32_t profiler_start(void);

/**
 * End a performance measurement
 * @param start_cycles: Value returned from profiler_start()
 * Returns: Elapsed cycles
 */
uint32_t profiler_end(uint32_t start_cycles);

/**
 * Update metrics with a new measurement
 */
void profiler_update_metrics(PerformanceMetrics *metrics, uint32_t cycles);

/**
 * Print metrics over USB
 */
void profiler_print_metrics(const char *label, const PerformanceMetrics *metrics);

#endif