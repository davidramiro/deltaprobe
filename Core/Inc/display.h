#pragma once
#include "u8g2.h"
#include <stdint.h>

#define DP_W 128
#define DP_H 128
#define DP_FONT_SMALL u8g2_font_6x10_tr
#define DP_FONT_LARGE u8g2_font_logisoso16_tr
#define DP_FONT_MEDIUM u8g2_font_NokiaLargeBold_tr
#define ERROR_SCREEN_MS 5000

void drawSplashScreen(void);
void drawStartupScreen();
void drawSensorBarInline();
void drawMeasurement(uint32_t baseline, uint32_t new, uint32_t latency);
void drawAverage(uint32_t latencies_us[], float mean_ms, float sd_ms);
void drawMainMenuInline();
void drawParamsMenu(uint8_t index);
void drawError(char *head, char *l1, char *l2);
void drawGraphInline(uint32_t latencies_us[]);
void drawJigglerScreen(uint8_t countdown);
void drawFlashScreen(uint8_t progress);
uint32_t getMax(uint32_t latencies_us[]);
uint32_t getMin(uint32_t latencies_us[]);