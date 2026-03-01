#include "display.h"
#include "display_assets.h"

#include <stdio.h>
#include <string.h>

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

void drawStartupScreen() {
  u8g2_ClearBuffer(&u8g2);

  u8g2_SetFont(&u8g2, DP_FONT_LARGE);
  u8g2_DrawStr(&u8g2, 0, 16, "deltaprobe");

  u8g2_SetFont(&u8g2, DP_FONT_SMALL);
  u8g2_DrawStr(&u8g2, 60, 26, "davidramiro");

  drawMainMenuInline();

  u8g2_SendBuffer(&u8g2);
}

uint32_t map(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
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

  u8g2_DrawRFrame(&u8g2, 0, 117, 128, 10, 3);
  u8g2_DrawRBox(&u8g2, 2, 119, map(cur_adc_val, min_adc_val, max_adc_val, 6, 124), 6, 2);
}

void drawMainMenuInline() {
  if (mainMenuIndex == LATENCY) {
    u8g2_DrawXBMP(&u8g2, 2, 31, 126, 42, menu_selection_2x_bitmap);
    u8g2_DrawXBM(&u8g2, 29, 57, 3, 5, left_bitmap);
    u8g2_DrawXBM(&u8g2, 115, 57, 3, 5, right_bitmap);
  } else {
    const uint8_t selectorY = (uint8_t)(55 + mainMenuIndex * 24);
    u8g2_DrawXBMP(&u8g2, 2, selectorY, 126, 21, menu_selection_bitmap);
  }

  u8g2_SetFont(&u8g2, DP_FONT_MEDIUM);
  u8g2_DrawStr(&u8g2, 31, 49, "LATENCY TEST");

  u8g2_SetFont(&u8g2, DP_FONT_SMALL);

  if (mainModeIndex == CLICK) {
    u8g2_DrawStr(&u8g2, 38, 63, " CLICK MODE ");
    u8g2_DrawXBMP(&u8g2, 8, 44, 15, 16, click_bitmap);
  } else if (mainModeIndex == MOVE) {
    u8g2_DrawStr(&u8g2, 35, 63, "  MOVE MODE  ");
    u8g2_DrawXBMP(&u8g2, 10, 44, 11, 16, move_bitmap);
  } else if (mainModeIndex == EXTERNAL) {
    u8g2_DrawStr(&u8g2, 35, 63, "EXTERNAL MODE");
    u8g2_DrawXBMP(&u8g2, 8, 44, 16, 16, external_bitmap);
  }

  u8g2_DrawXBMP(&u8g2, 8, 81, 17, 16, jiggler_bitmap);
  u8g2_DrawXBMP(&u8g2, 8, 105, 16, 16, cogwheel_bitmap);

  u8g2_DrawStr(&u8g2, 53, 93, "JIGGLER");
  u8g2_DrawStr(&u8g2, 46, 117, "PARAMETERS");
}

void drawParamsMenu(uint8_t index) {
  u8g2_ClearBuffer(&u8g2);

  const uint8_t selectorY = (uint8_t)(1 + index * 24);
  u8g2_DrawXBMP(&u8g2, 1, selectorY, 126, 20, menu_selection_bitmap);

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

void drawError(char *head, char *l1, char *l2) {

  u8g2_SetDrawColor(&u8g2, 2);
  u8g2_DrawBox(&u8g2, 2, 45, 123, 39);

  u8g2_SetDrawColor(&u8g2, 1);

  u8g2_DrawXBM(&u8g2, 1, 44, 126, 42, menu_selection_2x_bitmap);

  u8g2_SetFont(&u8g2, DP_FONT_MEDIUM);

  uint8_t w = u8g2_GetStrWidth(&u8g2, head);
  u8g2_DrawStr(&u8g2, 69 - (w / 2), 59, head);

  u8g2_DrawXBM(&u8g2, 7, 57, 16, 16, warning_bitmap);

  u8g2_SetFont(&u8g2, DP_FONT_SMALL);
  w = u8g2_GetStrWidth(&u8g2, l1);
  u8g2_DrawStr(&u8g2, 69 - (w / 2), 70, l1);
  w = u8g2_GetStrWidth(&u8g2, l2);
  u8g2_DrawStr(&u8g2, 69 - (w / 2), 80, l2);

  u8g2_SendBuffer(&u8g2);

  HAL_Delay(ERROR_SCREEN_MS);
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

  uint8_t w = 16 * progress;

  if (progress == 7) {
    w = 118;
  }

  u8g2_DrawXBM(&u8g2, 2, 52, 128, 21, menu_selection_bitmap);
  u8g2_DrawRBox(&u8g2, 5, 54, w, 16, 2);

  u8g2_SetFont(&u8g2, DP_FONT_MEDIUM);
  u8g2_DrawStr(&u8g2, 18, 27, "WRITING FLASH");

  u8g2_DrawStr(&u8g2, 10, 109, "DO NOT TURN OFF");

  u8g2_SendBuffer(&u8g2);
}