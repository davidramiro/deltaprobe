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

/**
 * @brief Triggers a mouse action based on the current menu index.
 * @details To be used as the first action within a latency measurement. 
 * Depending on `mainMenuIndex`, either clicks mouse1 or moves by 127px x, 127px y. It waits for the USB HID interface to be ready.
 * If a timeout occurs (6 seconds), it triggers an error state by flashing the error LED and displaying a message.
 * @return int8_t Error code
 */
int8_t startMouseAction() {
  hid_mouse_report_t report = {.wheel = 0, .pan = 0};

  if (mainModeIndex == CLICK) {
    report.buttons = 1;
  } else if (mainModeIndex == MOVE) {
    report.x = 127, report.y = 127;
  }

  while (!tud_hid_ready()) {
    tud_task();
    if (TIM2->CNT > 6000000) {
      HAL_GPIO_WritePin(ERR_LED_GPIO_Port, ERR_LED_Pin, GPIO_PIN_SET);
      drawError("USB timeout. Connected?");
      HAL_Delay(2000);
      HAL_GPIO_WritePin(ERR_LED_GPIO_Port, ERR_LED_Pin, GPIO_PIN_RESET);
      return 1;
    }
  }

  tud_hid_report(REPORT_ID_MOUSE, &report, sizeof(report));

  return TIM2->CNT;
}

/**
 * @brief Generates a random mouse movement report and sends it via USB HID.
 *
 * @details The mouse report contains random X and Y coordinates ranging from -3 to 3 pixels.
 * The function waits for the USB HID interface to become ready. If a timeout occurs
 * (6 seconds), it triggers an error state by turning on the error LED,
 * displaying a message, and delaying before returning.
 * Upon successful transmission, it flashes the info LED to indicate completion.
 */
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

/**
 * @brief Resets mouse state by sending HID input
 *
 * @details To be used after measurement with @ref startMouseAction is finished. 
 * Depending on `mainMenuIndex`, sets `buttons` to 0 for a releasing or `x` and `y` to -127 for a move back to
 * the original position. It waits for the USB HID interface to be ready.
 * If a timeout occurs (6 seconds), it triggers an error state by flashing the error LED and displaying a message.
 * Finally, it sends the report.
 * @return int8_t Error code
 */
int8_t stopMouseAction() {
  hid_mouse_report_t report = {.wheel = 0, .pan = 0};

  if (mainModeIndex == CLICK) {
    report.buttons = 0;
  } else if (mainModeIndex == MOVE) {
    report.x = -127, report.y = -127;
  }

  while (!tud_hid_ready()) {
    tud_task();
    if (TIM2->CNT > 6000000) {
      HAL_GPIO_WritePin(ERR_LED_GPIO_Port, ERR_LED_Pin, GPIO_PIN_SET);
      drawError("USB timeout. Connected?");
      HAL_Delay(2000);
      HAL_GPIO_WritePin(ERR_LED_GPIO_Port, ERR_LED_Pin, GPIO_PIN_RESET);
      return 1;
    }
  }

  tud_hid_report(REPORT_ID_MOUSE, &report, sizeof(report));
}