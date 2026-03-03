#pragma once
#include <stdint.h>

#define MS_FACTOR 1000.0f
#define MEASUREMENT_DELAY 503

uint32_t read_single_ADC();
uint32_t get_3sample_average_ADC();
int8_t measure(uint32_t latencies_us[]);
void compute_latency_stats(const uint32_t latencies_us[], float *mean_ms, float *sd_ms);