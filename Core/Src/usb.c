#include "usb.h"
#include "main.h"
#include <stdint.h>
#include <stdlib.h>

#include "display.h"
#include "hid_device.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_tim.h"
#include "usb_descriptors.h"
#include "usbd.h"

uint32_t startMouseAction() {
  HAL_TIM_Base_Stop_IT(&htim2);
  __HAL_TIM_SET_COUNTER(&htim2, 0);
  HAL_TIM_Base_Start_IT(&htim2);

  hid_mouse_report_t report = {.wheel = 0, .pan = 0};

  if (mainMenuIndex == CLICK) {
    report.buttons = 1;
  } else if (mainMenuIndex == MOVE) {
    report.x = 127, report.y = 127;
  }

  while (!tud_hid_ready()) {
    tud_task();
    if (TIM2->CNT > 6000000) {
      HAL_GPIO_WritePin(ERR_LED_GPIO_Port, ERR_LED_Pin, GPIO_PIN_SET);
      drawError("USB timeout. Connected?");
      HAL_Delay(2000);
      HAL_GPIO_WritePin(ERR_LED_GPIO_Port, ERR_LED_Pin, GPIO_PIN_RESET);
      return 0;
    }
  }

  tud_hid_report(REPORT_ID_MOUSE, &report, sizeof(report));

  return TIM2->CNT;
}

void randomMouseMove() {
  hid_mouse_report_t report = {
      .wheel = 0,
      .pan = 0,
      .x = (rand() % 7) - 3,
      .y = (rand() % 7) - 3,
  };

  while (!tud_hid_ready()) {
    tud_task();
    if (TIM2->CNT > 6000000) {
      HAL_GPIO_WritePin(ERR_LED_GPIO_Port, ERR_LED_Pin, GPIO_PIN_SET);
      drawError("USB timeout. Connected?");
      HAL_Delay(2000);
      HAL_GPIO_WritePin(ERR_LED_GPIO_Port, ERR_LED_Pin, GPIO_PIN_RESET);
      return;
    }
  }

  tud_hid_report(REPORT_ID_MOUSE, &report, sizeof(report));

  HAL_GPIO_WritePin(INF_LED_GPIO_Port, INF_LED_Pin, GPIO_PIN_SET);
  HAL_Delay(50);
  HAL_GPIO_WritePin(INF_LED_GPIO_Port, INF_LED_Pin, GPIO_PIN_RESET);
}

void stopMouseAction() {

  hid_mouse_report_t report = {.wheel = 0, .pan = 0};

  if (mainMenuIndex == CLICK) {
    report.buttons = 0;
  } else if (mainMenuIndex == MOVE) {
    report.x = -127, report.y = -127;
  }

  while (!tud_hid_ready()) {
    tud_task();
    if (TIM2->CNT > 6000000) {
      HAL_GPIO_WritePin(ERR_LED_GPIO_Port, ERR_LED_Pin, GPIO_PIN_SET);
      drawError("USB timeout. Connected?");
      HAL_Delay(2000);
      HAL_GPIO_WritePin(ERR_LED_GPIO_Port, ERR_LED_Pin, GPIO_PIN_RESET);
      return;
    }
  }

  tud_hid_report(REPORT_ID_MOUSE, &report, sizeof(report));
}