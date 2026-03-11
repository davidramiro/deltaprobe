#include "display.h"
#include "display_assets.h"

#include <stdio.h>
#include <string.h>

#include "main.h"

void render_splash_screen(void) {
  u8g2_ClearBuffer(&u8g2);

  u8g2_DrawXBMP(&u8g2, 30, 6, 64, 64, logo_bitmap);
  u8g2_SetFont(&u8g2, DP_FONT_LARGE);
  u8g2_DrawStr(&u8g2, 18, DP_H - 19, "deltaprobe");
  u8g2_SetFont(&u8g2, DP_FONT_MEDIUM);
  u8g2_DrawStr(&u8g2, 30, DP_H - 4, "davidramiro");

  u8g2_SendBuffer(&u8g2);
}

void render_startup_screen() {
  u8g2_ClearBuffer(&u8g2);

  u8g2_SetFont(&u8g2, DP_FONT_LARGE);
  u8g2_DrawStr(&u8g2, 0, 16, "deltaprobe");

  u8g2_SetFont(&u8g2, DP_FONT_SMALL);
  u8g2_DrawStr(&u8g2, 60, 26, "davidramiro");

  draw_main_menu();

  u8g2_SendBuffer(&u8g2);
}

uint32_t map(const uint32_t x, const uint32_t in_min, const uint32_t in_max,
             const uint32_t out_min, const uint32_t out_max) {
  if (in_min == in_max) {
    // avoid division by zero
    return out_max;
  }
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void draw_sensor_bar(void) {
  char min_buf[6];
  char max_buf[6];
  char cur_buf[6];

  snprintf(min_buf, sizeof(min_buf), "%lu", (unsigned long)min_adc_val);
  snprintf(max_buf, sizeof(max_buf), "%lu", (unsigned long)max_adc_val);
  snprintf(cur_buf, sizeof(cur_buf), "%lu", (unsigned long)cur_adc_val);

  u8g2_SetFont(&u8g2, DP_FONT_SMALL);
  u8g2_DrawStr(&u8g2, 0, DP_H - 18, min_buf);
  u8g2_DrawStr(&u8g2, DP_W / 2 - 12, DP_H - 18, cur_buf);
  u8g2_DrawStr(&u8g2, DP_W - 24, DP_H - 18, max_buf);

  u8g2_DrawRFrame(&u8g2, 0, 117, 128, 10, 3);
  u8g2_DrawRBox(&u8g2, 2, 119,
                map(cur_adc_val, min_adc_val, max_adc_val, 6, 124), 6, 2);
}

void draw_main_menu() {
  if (main_menu_selector == LATENCY) {
    u8g2_DrawXBMP(&u8g2, 2, 31, 126, 42, menu_item_2x_bitmap);
    u8g2_DrawXBM(&u8g2, 29, 57, 3, 5, left_bitmap);
    u8g2_DrawXBM(&u8g2, 115, 57, 3, 5, right_bitmap);
  } else {
    const uint8_t selectorY = (uint8_t)(55 + main_menu_selector * 24);
    u8g2_DrawXBMP(&u8g2, 2, selectorY, 126, 21, menu_item_bitmap);
  }

  u8g2_SetFont(&u8g2, DP_FONT_MEDIUM);
  u8g2_DrawStr(&u8g2, 31, 49, "LATENCY TEST");

  u8g2_SetFont(&u8g2, DP_FONT_SMALL);

  if (latency_mode_selector == CLICK) {
    u8g2_DrawStr(&u8g2, 38, 63, " CLICK MODE ");
    u8g2_DrawXBMP(&u8g2, 8, 44, 15, 16, crosshair_bitmap);
  } else if (latency_mode_selector == MOVE) {
    u8g2_DrawStr(&u8g2, 35, 63, "  MOVE MODE  ");
    u8g2_DrawXBMP(&u8g2, 10, 44, 11, 16, pointer_bitmap);
  } else if (latency_mode_selector == EXTERNAL) {
    u8g2_DrawStr(&u8g2, 35, 63, "EXTERNAL MODE");
    u8g2_DrawXBMP(&u8g2, 8, 44, 16, 16, bulb_bitmap);
  }

  u8g2_DrawXBMP(&u8g2, 8, 81, 17, 16, jiggler_bitmap);
  u8g2_DrawXBMP(&u8g2, 8, 105, 16, 16, cogwheel_bitmap);

  u8g2_DrawStr(&u8g2, 53, 93, "JIGGLER");
  u8g2_DrawStr(&u8g2, 46, 117, "PARAMETERS");
}

void render_params_menu(const uint8_t index) {
  u8g2_ClearBuffer(&u8g2);

  const uint8_t selectorY = (uint8_t)(1 + index * 24);
  u8g2_DrawXBMP(&u8g2, 1, selectorY, 126, 20, menu_item_bitmap);

  char value_buf[12];
  u8g2_SetFont(&u8g2, DP_FONT_SMALL);
  snprintf(value_buf, sizeof(value_buf), "%d", (int)num_cycles);
  int str_width = u8g2_GetStrWidth(&u8g2, value_buf);
  u8g2_DrawStr(&u8g2, 110 - str_width / 2, 14, value_buf);
  snprintf(value_buf, sizeof(value_buf), "%d", (int)sensor_threshold);
  str_width = u8g2_GetStrWidth(&u8g2, value_buf);
  u8g2_DrawStr(&u8g2, 110 - str_width / 2, 14 + 24, value_buf);

  if (adc_channel == 1) {
    u8g2_DrawStr(&u8g2, 101, 14 + 48, "INT");
  } else if (adc_channel == 4) {
    u8g2_DrawStr(&u8g2, 101, 14 + 48, "EXT");
  }

  if (index != 3) {
    u8g2_DrawXBMP(&u8g2, 27, selectorY + 7, 3, 5, left_bitmap);
    u8g2_DrawXBMP(&u8g2, 89, selectorY + 7, 3, 5, right_bitmap);
  }

  u8g2_DrawXBMP(&u8g2, 3, 3, 16, 16, cycles_bitmap);
  u8g2_DrawXBMP(&u8g2, 3, 3 + 24, 15, 16, threshold_bitmap);
  u8g2_DrawXBMP(&u8g2, 3, 3 + 48, 15, 16, sensor_bitmap);
  u8g2_DrawXBMP(&u8g2, 3, 3 + 72, 15, 14, exit_bitmap);

  u8g2_DrawStr(&u8g2, 42, 14, "CYCLES");
  u8g2_DrawStr(&u8g2, 33, 14 + 24, "THRESHOLD");
  u8g2_DrawStr(&u8g2, 42, 14 + 48, "SENSOR");
  u8g2_DrawStr(&u8g2, 42, 14 + 72, "SAVE & EXIT");

  draw_sensor_bar();

  u8g2_SendBuffer(&u8g2);
}

void render_measurement(const uint32_t baseline, const uint32_t new_value,
                        const uint32_t latency) {
  u8g2_ClearBuffer(&u8g2);

  // top bar filling with cycles
  u8g2_DrawRFrame(&u8g2, 0, 2, 128, 10, 3);
  u8g2_DrawRBox(&u8g2, 2, 4, map(cycle_index + 1, 1, num_cycles, 6, 124), 6, 2);

  // cycle n / num_cycles box
  u8g2_SetFont(&u8g2, DP_FONT_MEDIUM);
  u8g2_DrawStr(&u8g2, 45, 25, "CYCLE");

  u8g2_DrawRFrame(&u8g2, 0, 28, 128, 19, 3);
  u8g2_DrawTriangle(&u8g2, 73, 28, 91, 46, 55, 46);
  u8g2_DrawBox(&u8g2, 74, 29, 53, 17);

  char cycle_buf[6];
  snprintf(cycle_buf, sizeof(cycle_buf), "%d", cycle_index + 1);
  uint8_t w = u8g2_GetStrWidth(&u8g2, cycle_buf);
  u8g2_DrawStr(&u8g2, 30 - w / 2, 43, cycle_buf);

  char num_cycles_buf[6];
  snprintf(num_cycles_buf, sizeof(num_cycles_buf), "%d", num_cycles);
  w = u8g2_GetStrWidth(&u8g2, num_cycles_buf);
  u8g2_SetDrawColor(&u8g2, 0);
  u8g2_DrawStr(&u8g2, 100 - w / 2, 43, num_cycles_buf);
  u8g2_SetDrawColor(&u8g2, 1);

  // sensor baseline / new box
  u8g2_DrawStr(&u8g2, 42, 62, "SENSOR");

  u8g2_DrawRFrame(&u8g2, 0, 65, 128, 19, 3);
  u8g2_DrawTriangle(&u8g2, 58, 65, 67, 74, 57, 83);
  u8g2_DrawBox(&u8g2, 1, 66, 57, 17);

  u8g2_SetDrawColor(&u8g2, 0);
  char baseline_buf[12];
  snprintf(baseline_buf, sizeof(baseline_buf), "%lu", (unsigned long)baseline);
  w = u8g2_GetStrWidth(&u8g2, baseline_buf);
  u8g2_DrawStr(&u8g2, 30 - w / 2, 80, baseline_buf);

  u8g2_SetDrawColor(&u8g2, 1);
  if (new_value != (uint32_t)-1) {
    char new_buf[12];
    snprintf(new_buf, sizeof(new_buf), "%lu", (unsigned long)new_value);
    w = u8g2_GetStrWidth(&u8g2, new_buf);
    u8g2_DrawStr(&u8g2, 100 - w / 2, 80, new_buf);
  }

  // latency box
  u8g2_SetDrawColor(&u8g2, 1);
  u8g2_DrawRBox(&u8g2, 0, 102, 128, 26, 3);
  u8g2_SetFont(&u8g2, DP_FONT_MEDIUM);
  u8g2_DrawStr(&u8g2, 40, 99, "LATENCY");
  

  if (latency != (uint32_t)-1) {
    u8g2_SetDrawColor(&u8g2, 0);
    u8g2_SetFont(&u8g2, DP_FONT_LARGE);
    char latency_buf[16];
    snprintf(latency_buf, sizeof(latency_buf), "%.3f ms",
             (float)latency / 1000.0f);
    w = u8g2_GetStrWidth(&u8g2, latency_buf);
    u8g2_DrawStr(&u8g2, 64 - w / 2, 123, latency_buf);
    u8g2_SetDrawColor(&u8g2, 1);
  }

  u8g2_SendBuffer(&u8g2);
}

uint32_t get_min(const uint32_t latencies_us[]) {
  uint32_t min = latencies_us[0];
  for (uint8_t i = 1; i < num_cycles; i++) {
    const uint32_t v = latencies_us[i];
    if (v < min)
      min = v;
  }
  return min;
}

uint32_t get_max(const uint32_t latencies_us[]) {
  uint32_t max = latencies_us[0];
  for (uint8_t i = 1; i < num_cycles; i++) {
    const uint32_t v = latencies_us[i];
    if (v > max)
      max = v;
  }
  return max;
}

void draw_latency_graph(uint32_t latencies_us[], float mean_ms) {
  u8g2_DrawLine(&u8g2, 125, 110, 19, 110);
  u8g2_DrawLine(&u8g2, 20, 55, 20, 111);
  u8g2_DrawPixel(&u8g2, 19, 56);
  u8g2_DrawPixel(&u8g2, 19, 81);
  u8g2_DrawPixel(&u8g2, 19, 107);

  const uint32_t min = get_min(latencies_us) / 1000;
  const uint32_t max = get_max(latencies_us) / 1000;

  u8g2_SetFont(&u8g2, DP_FONT_XSMALL);
  char max_buf[8];
  snprintf(max_buf, sizeof(max_buf), "%lu", max);
  u8g2_DrawStr(&u8g2, 18 - u8g2_GetStrWidth(&u8g2, max_buf), 59, max_buf);
  char min_buf[8];
  snprintf(min_buf, sizeof(min_buf), "%lu", min);
  u8g2_DrawStr(&u8g2, 18 - u8g2_GetStrWidth(&u8g2, min_buf), 110, min_buf);
  char med_buf[8];
  snprintf(med_buf, sizeof(med_buf), "%lu", (max + min) / 2);
  u8g2_DrawStr(&u8g2, 18 - u8g2_GetStrWidth(&u8g2, med_buf), 84, med_buf);

  const uint32_t range = max - min;

  const float pixel_per_value = range == 0 ? 0.0f : 51.0f / (float)range;
  const float pixel_per_cycle =
      num_cycles == 0 ? 0.0f : 108.0f / (float)num_cycles;

  uint8_t mean_y = (uint8_t)(107 - (int)((mean_ms - min) * pixel_per_value));
  for (int i = 23; i < 122; i += 3) {
    u8g2_DrawPixel(&u8g2, i, mean_y);
  }

  uint8_t prevX = 0;
  uint8_t prevY = 0;

  for (uint8_t i = 0; i < num_cycles; i++) {
    const uint8_t x = (uint8_t)(23 + i * pixel_per_cycle);
    const uint32_t v_ms = latencies_us[i] / 1000;
    uint8_t y = (uint8_t)(107 - (int)((v_ms - min) * pixel_per_value));

    if (prevX != 0) {
      u8g2_DrawLine(&u8g2, prevX, prevY, x, y);
    }

    prevX = x;
    prevY = y;
  }
}

void render_statistics(uint32_t latencies_us[], const float mean_ms,
                       const float sd_ms) {
  u8g2_ClearBuffer(&u8g2);

  u8g2_DrawRFrame(&u8g2, 1, 1, 126, 51, 3);

  u8g2_SetFont(&u8g2, DP_FONT_XSMALL);
  u8g2_DrawStr(&u8g2, 5, 9, "AVERAGE");
  char n_buf[8];
  snprintf(n_buf, sizeof(n_buf), "n: %d", num_cycles);
  u8g2_DrawStr(&u8g2, 5, 16, n_buf);

  u8g2_SetFont(&u8g2, DP_FONT_MEDIUM);
  char mean_buf[16];
  snprintf(mean_buf, sizeof(mean_buf), "%.3f ms", mean_ms);
  u8g2_DrawStr(&u8g2, 120 - u8g2_GetStrWidth(&u8g2, mean_buf), 15, mean_buf);

  u8g2_DrawLine(&u8g2, 6, 18, 120, 18);

  u8g2_SetFont(&u8g2, DP_FONT_SMALL);
  u8g2_DrawStr(&u8g2, 6, 28, "JITTER");
  u8g2_DrawStr(&u8g2, 6, 38, "MIN");
  u8g2_DrawStr(&u8g2, 6, 48, "MAX");

  char jitter_buf[16];
  snprintf(jitter_buf, sizeof(jitter_buf), "%.3f ms", sd_ms);
  u8g2_DrawStr(&u8g2, 120 - u8g2_GetStrWidth(&u8g2, jitter_buf), 28,
               jitter_buf);
  char min_buf[16];
  snprintf(min_buf, sizeof(min_buf), "%.3f ms",
           (float)get_min(latencies_us) / 1000.0f);
  u8g2_DrawStr(&u8g2, 120 - u8g2_GetStrWidth(&u8g2, min_buf), 38, min_buf);
  char max_buf[16];
  snprintf(max_buf, sizeof(max_buf), "%.3f ms",
           (float)get_max(latencies_us) / 1000.0f);
  u8g2_DrawStr(&u8g2, 120 - u8g2_GetStrWidth(&u8g2, max_buf), 48, max_buf);

  draw_latency_graph(latencies_us, mean_ms);

  u8g2_DrawXBM(&u8g2, 18, 113, 93, 14, menu_item_sm_bitmap);

  u8g2_DrawXBM(&u8g2, 32, 117, 7, 5, exit_sm_bitmap);

  u8g2_SetFont(&u8g2, DP_FONT_SMALL);
  u8g2_DrawStr(&u8g2, 46, 123, "MAIN MENU");

  u8g2_SendBuffer(&u8g2);
}

void draw_error_overlay(const char *head, const char *l1, const char *l2) {

  u8g2_SetDrawColor(&u8g2, 2);
  u8g2_DrawBox(&u8g2, 2, 45, 123, 39);

  u8g2_SetDrawColor(&u8g2, 1);

  u8g2_DrawXBM(&u8g2, 1, 44, 126, 42, menu_item_2x_bitmap);

  u8g2_SetFont(&u8g2, DP_FONT_MEDIUM);

  uint8_t w = u8g2_GetStrWidth(&u8g2, head);
  u8g2_DrawStr(&u8g2, 69 - w / 2, 59, head);

  u8g2_DrawXBM(&u8g2, 7, 57, 16, 16, warning_bitmap);

  u8g2_SetFont(&u8g2, DP_FONT_SMALL);
  w = u8g2_GetStrWidth(&u8g2, l1);
  u8g2_DrawStr(&u8g2, 69 - w / 2, 70, l1);
  w = u8g2_GetStrWidth(&u8g2, l2);
  u8g2_DrawStr(&u8g2, 69 - w / 2, 80, l2);

  u8g2_SendBuffer(&u8g2);

  HAL_Delay(ERROR_SCREEN_MS);
}

void render_jiggler_screen(const uint8_t countdown) {
  u8g2_ClearBuffer(&u8g2);

  u8g2_SetFont(&u8g2, DP_FONT_MEDIUM);
  u8g2_DrawStr(&u8g2, 0, 20, "JIGGLER ACTIVE");

  u8g2_DrawXBMP(&u8g2, 1, 105, 128, 21, menu_item_bitmap);
  u8g2_DrawXBMP(&u8g2, 8, 108, 15, 14, exit_bitmap);

  if (countdown % 2 == 0) {
    u8g2_DrawXBM(&u8g2, 37, 25, 64, 64, jiggler_full_bitmap_01);
  } else {
    u8g2_DrawXBM(&u8g2, 29, 25, 64, 64, jiggler_full_bitmap_02);
  }

  char buf[23];
  snprintf(buf, sizeof(buf), "Sending input in %d s", countdown);
  u8g2_SetFont(&u8g2, DP_FONT_SMALL);
  u8g2_DrawStr(&u8g2, 1, 99, buf);
  u8g2_DrawStr(&u8g2, 41, 119, "MAIN MENU");

  u8g2_SendBuffer(&u8g2);
}

void render_flash_screen(const uint8_t progress) {
  u8g2_ClearBuffer(&u8g2);

  uint8_t w = 16 * progress;

  if (progress == 7) {
    w = 118;
  }

  u8g2_DrawXBM(&u8g2, 2, 52, 128, 21, menu_item_bitmap);
  u8g2_DrawRBox(&u8g2, 5, 54, w, 16, 2);

  u8g2_SetFont(&u8g2, DP_FONT_MEDIUM);
  u8g2_DrawStr(&u8g2, 18, 27, "WRITING FLASH");

  u8g2_DrawStr(&u8g2, 10, 109, "DO NOT TURN OFF");

  u8g2_SendBuffer(&u8g2);
}