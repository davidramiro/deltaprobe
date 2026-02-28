#include "display.h"
#include "display_assets.h"

#include <stdio.h>

#include "main.h"

void drawSplashScreen(void) {
  u8g2_ClearBuffer(&u8g2);

  u8g2_DrawXBMP(&u8g2, 30, 6, 64, 64, logo_bitmap_64);
  u8g2_SetFont(&u8g2, DP_FONT_LARGE);
  u8g2_DrawStr(&u8g2, 18, DP_H - 19, "deltaprobe");
  u8g2_SetFont(&u8g2, DP_FONT_MEDIUM);
  u8g2_DrawStr(&u8g2, 30, DP_H - 4, "davidramiro");

  u8g2_SendBuffer(&u8g2);
}

void drawStartupScreen(uint8_t mode) {
  u8g2_ClearBuffer(&u8g2);

  u8g2_SetFont(&u8g2, DP_FONT_LARGE);
  u8g2_DrawStr(&u8g2, 0, 16, "deltaprobe");

  u8g2_SetFont(&u8g2, DP_FONT_SMALL);
  u8g2_DrawStr(&u8g2, 60, 26, "davidramiro");

  drawMainMenuInline(mode);

  u8g2_SendBuffer(&u8g2);
}

void drawSensorBarInline(void) {
  char min_buf[6];
  char max_buf[6];
  char cur_buf[6];

  snprintf(min_buf, sizeof(min_buf), "%lu", (unsigned long)min_adc_val);
  snprintf(max_buf, sizeof(max_buf), "%lu", (unsigned long)max_adc_val);
  snprintf(cur_buf, sizeof(cur_buf), "%lu", (unsigned long)cur_adc_val);

  u8g2_SetFont(&u8g2, DP_FONT_SMALL);
  u8g2_DrawStr(&u8g2, 0, DP_H - 18, min_buf);
  u8g2_DrawStr(&u8g2, (uint8_t)(DP_W / 2 - 12), DP_H - 18, cur_buf);
  u8g2_DrawStr(&u8g2, (uint8_t)(DP_W - 24), DP_H - 18, max_buf);

  float bar_units;
  if (max_adc_val == min_adc_val) {
    bar_units = 124 / 2.0f;
  } else {
    bar_units = 124 / (float)(max_adc_val - min_adc_val);
  }

  float bar_w_f = (float)cur_adc_val * bar_units;
  if (bar_w_f < 0.0f)
    bar_w_f = 0.0f;
  if (bar_w_f > (float)124)
    bar_w_f = (float)124;

  u8g2_DrawRFrame(&u8g2, 0, 117, 128, 10, 3);
  u8g2_DrawRBox(&u8g2, 2, 119, (uint8_t)bar_w_f, 6, 2);
}

void drawMainMenuInline(uint8_t index) {
  const uint8_t selectorY = (uint8_t)(31 + index * 24);

  u8g2_DrawXBMP(&u8g2, 2, selectorY, 128, 21, menu_selection_bitmap);

  u8g2_DrawXBMP(&u8g2, 9, 33, 15, 16, click_bitmap);
  u8g2_DrawXBMP(&u8g2, 12, 57, 11, 16, move_bitmap);
  u8g2_DrawXBMP(&u8g2, 8, 81, 17, 16, jiggler_bitmap);
  u8g2_DrawXBMP(&u8g2, 8, 105, 16, 16, cogwheel_bitmap);

  u8g2_SetFont(&u8g2, DP_FONT_SMALL);
  u8g2_DrawStr(&u8g2, 40, 45, "CLICK MODE");
  u8g2_DrawStr(&u8g2, 40, 69, "MOVE MODE");
  u8g2_DrawStr(&u8g2, 43, 93, "JIGGLER");
  u8g2_DrawStr(&u8g2, 40, 117, "PARAMETERS");
}

void drawParamsMenu(uint8_t index) {
  u8g2_ClearBuffer(&u8g2);

  const uint8_t selectorY = (uint8_t)(30 + index * 24);
  u8g2_DrawXBMP(&u8g2, 2, selectorY, 128, 21, menu_selection_bitmap);

  char value_buf[12];
  u8g2_SetFont(&u8g2, DP_FONT_LARGE);
  if (index == 0) {
    snprintf(value_buf, sizeof(value_buf), "%d", (int)num_cycles);
    u8g2_DrawStr(&u8g2, 52, 20, value_buf);
  } else if (index == 1) {
    snprintf(value_buf, sizeof(value_buf), "%d", (int)sensor_threshold);
    u8g2_DrawStr(&u8g2, 52, 20, value_buf);
  }

  if (index != 2) {
    u8g2_DrawXBMP(&u8g2, 102, 0, 21, 24, right_bitmap);

    u8g2_DrawXBMP(&u8g2, 8, 0, 21, 24, left_bitmap);
  }

  u8g2_DrawXBMP(&u8g2, 8, 32, 16, 16, cycles_bitmap);
  u8g2_DrawXBMP(&u8g2, 8, 56, 15, 16, threshold_bitmap);
  u8g2_DrawXBMP(&u8g2, 8, 80, 15, 14, exit_bitmap);

  u8g2_SetFont(&u8g2, DP_FONT_SMALL);
  u8g2_DrawStr(&u8g2, 40, 44, "  CYCLES  ");
  u8g2_DrawStr(&u8g2, 43, 68, "THRESHOLD");
  u8g2_DrawStr(&u8g2, 37, 92, "SAVE & EXIT");

  drawSensorBarInline();

  u8g2_SendBuffer(&u8g2);
}

void drawMeasurement(uint32_t baseline, uint32_t new_value, uint32_t latency) {
  u8g2_ClearBuffer(&u8g2);

  u8g2_DrawLine(&u8g2, 0, 36, 127, 36);

  u8g2_SetFont(&u8g2, DP_FONT_SMALL);
  u8g2_DrawStr(&u8g2, 1, 9, "CYCLE");

  u8g2_SetFont(&u8g2, DP_FONT_LARGE);
  char cycle_buf[16];
  snprintf(cycle_buf, sizeof(cycle_buf), "%d / %d", (int)(cycle_index + 1),
           (int)num_cycles);
  u8g2_DrawStr(&u8g2, 35, 20, cycle_buf);

  u8g2_DrawLine(&u8g2, 0, 72, 127, 72);

  u8g2_SetFont(&u8g2, DP_FONT_SMALL);
  u8g2_DrawStr(&u8g2, 1, 46, "BRIGHTNESS");

  u8g2_SetFont(&u8g2, DP_FONT_LARGE);
  char baseline_buf[12];
  snprintf(baseline_buf, sizeof(baseline_buf), "%lu", (unsigned long)baseline);
  u8g2_DrawStr(&u8g2, 3, 64, baseline_buf);

  if (new_value != (uint32_t)-1) {
    char new_buf[12];
    snprintf(new_buf, sizeof(new_buf), "%lu", (unsigned long)new_value);
    u8g2_DrawStr(&u8g2, 73, 64, new_buf);

    u8g2_DrawXBMP(&u8g2, 45, 50, 17, 10, arrow_bitmap);
  }

  u8g2_SetFont(&u8g2, DP_FONT_SMALL);
  u8g2_DrawStr(&u8g2, 1, 82, "LATENCY");

  if (latency != (uint32_t)-1) {
    u8g2_SetFont(&u8g2, DP_FONT_LARGE);
    u8g2_DrawStr(&u8g2, 93, 102, "ms");

    char latency_buf[16];
    snprintf(latency_buf, sizeof(latency_buf), "%.3f",
             (float)latency / 1000.0f);
    u8g2_DrawStr(&u8g2, 2, 102, latency_buf);
  }

  u8g2_SendBuffer(&u8g2);
}

uint32_t getMin(uint32_t latencies_us[]) {
  uint32_t min = latencies_us[0] / 1000;
  for (uint8_t i = 1; i < num_cycles; i++) {
    uint32_t v = latencies_us[i] / 1000;
    if (v < min)
      min = v;
  }
  return min;
}

uint32_t getMax(uint32_t latencies_us[]) {
  uint32_t max = latencies_us[0] / 1000;
  for (uint8_t i = 1; i < num_cycles; i++) {
    uint32_t v = latencies_us[i] / 1000;
    if (v > max)
      max = v;
  }
  return max;
}

void drawGraphInline(uint32_t latencies_us[]) {
  u8g2_DrawLine(&u8g2, 4, 60, 4, 96);
  u8g2_DrawLine(&u8g2, 2, 94, 122, 94);
  u8g2_DrawLine(&u8g2, 4, 60, 5, 61);
  u8g2_DrawLine(&u8g2, 122, 94, 121, 95);
  u8g2_DrawLine(&u8g2, 4, 60, 3, 61);
  u8g2_DrawLine(&u8g2, 122, 94, 121, 93);

  const uint32_t min = getMin(latencies_us);
  const uint32_t max = getMax(latencies_us);
  const uint32_t range = max - min;

  const float pixel_per_value = (range == 0) ? 0.0f : (30.0f / (float)range);
  const float pixel_per_cycle =
      (num_cycles == 0) ? 0.0f : (120.0f / (float)num_cycles);

  uint8_t prevX = 0;
  uint8_t prevY = 0;

  for (uint8_t i = 0; i < num_cycles; i++) {
    uint8_t x = (uint8_t)((i + 1) * pixel_per_cycle);
    uint32_t v_ms = latencies_us[i] / 1000;
    uint8_t y = (uint8_t)(93 - (int)((v_ms - min) * pixel_per_value));

    u8g2_DrawXBMP(&u8g2, x - 1, y - 1, 3, 3, graph_point_bitmap);

    if (prevX != 0) {
      u8g2_DrawLine(&u8g2, prevX, prevY, x, y);
    }

    prevX = x;
    prevY = y;
  }
}

void drawAverage(uint32_t latencies_us[], float mean_ms, float sd_ms) {
  u8g2_ClearBuffer(&u8g2);

  u8g2_SetFont(&u8g2, DP_FONT_SMALL);
  u8g2_DrawStr(&u8g2, 2, 10, "MEAN");
  u8g2_DrawStr(&u8g2, 2, 38, "STD DEV");
  u8g2_DrawStr(&u8g2, 114, 24, "ms");
  u8g2_DrawStr(&u8g2, 114, 54, "ms");

  u8g2_SetFont(&u8g2, DP_FONT_LARGE);
  char mean_buf[16];
  snprintf(mean_buf, sizeof(mean_buf), "%.3f", mean_ms);
  u8g2_DrawStr(&u8g2, 52, 18, mean_buf);

  char sd_buf[16];
  snprintf(sd_buf, sizeof(sd_buf), "%.3f", sd_ms);
  u8g2_DrawStr(&u8g2, 52, 46, sd_buf);

  u8g2_DrawXBMP(&u8g2, 1, 105, 128, 21, menu_selection_bitmap);
  u8g2_DrawXBMP(&u8g2, 8, 108, 15, 14, exit_bitmap);

  u8g2_SetFont(&u8g2, DP_FONT_SMALL);
  u8g2_DrawStr(&u8g2, 41, 120, "MAIN MENU");

  drawGraphInline(latencies_us);

  u8g2_SendBuffer(&u8g2);
}

void drawError(char *error) {
  u8g2_ClearBuffer(&u8g2);

  u8g2_SetFont(&u8g2, DP_FONT_LARGE);
  u8g2_DrawStr(&u8g2, 10, 50, error);

  u8g2_SendBuffer(&u8g2);
}

void drawJigglerScreen(uint8_t countdown) {
  u8g2_ClearBuffer(&u8g2);

  u8g2_SetFont(&u8g2, DP_FONT_MEDIUM);
  u8g2_DrawStr(&u8g2, 0, 20, "JIGGLER ACTIVE");

  u8g2_DrawXBMP(&u8g2, 1, 105, 128, 21, menu_selection_bitmap);
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

void drawFlashScreen(const uint8_t progress) {
  u8g2_ClearBuffer(&u8g2);

  uint8_t w = 19 * progress;

  if (progress == 6) {
    w = 118;
  }

  u8g2_DrawXBM(&u8g2, 2, 52, 128, 21, menu_selection_bitmap);
  u8g2_DrawRBox(&u8g2, 5, 54, w, 16, 2);

  u8g2_SetFont(&u8g2, DP_FONT_MEDIUM);
  u8g2_DrawStr(&u8g2, 18, 27, "WRITING FLASH");

  u8g2_DrawStr(&u8g2, 10, 109, "DO NOT TURN OFF");

  u8g2_SendBuffer(&u8g2);
}