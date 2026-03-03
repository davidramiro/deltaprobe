#pragma once
#include "u8g2.h"
#include <stdint.h>

#define DP_W 128
#define DP_H 128
#define DP_FONT_XSMALL u8g2_font_4x6_tr
#define DP_FONT_SMALL u8g2_font_6x10_tr
#define DP_FONT_LARGE u8g2_font_logisoso16_tr
#define DP_FONT_MEDIUM u8g2_font_NokiaLargeBold_tr
#define ERROR_SCREEN_MS 5000

void render_splash_screen(void);
void render_startup_screen();
void draw_sensor_bar();
void render_measurement(uint32_t baseline, uint32_t new_value, uint32_t latency);
void render_statistics(uint32_t latencies_us[], float mean_ms, float sd_ms);
void draw_main_menu();
void render_params_menu(uint8_t index);
void draw_error_overlay(const char *head, const char *l1, const char *l2);
void draw_latency_graph(uint32_t latencies_us[], float mean_ms);
void render_jiggler_screen(uint8_t countdown);
void render_flash_screen(uint8_t progress);
uint32_t get_max(const uint32_t latencies_us[]);
uint32_t get_min(const uint32_t latencies_us[]);