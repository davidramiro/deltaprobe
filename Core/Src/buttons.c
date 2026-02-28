#include "main.h"

volatile uint8_t btn_up_pressed = 0;
volatile uint8_t btn_down_pressed = 0;
volatile uint8_t btn_left_pressed = 0;
volatile uint8_t btn_right_pressed = 0;
volatile uint8_t btn_center_pressed = 0;

void pollMainMenuButtons() {
  if (btn_up_pressed && mainMenuIndex > CLICK) {
    mainMenuIndex--;
    btn_up_pressed = 0;
  }

  if (btn_down_pressed && mainMenuIndex < PARAMS) {
    mainMenuIndex++;
    btn_down_pressed = 0;
  }
}

void pollParamMenuButtons() {
  if (btn_up_pressed && paramMenuIndex > CYCLES) {
    paramMenuIndex--;
    btn_up_pressed = 0;
  }
  if (btn_down_pressed && paramMenuIndex < EXIT) {
    paramMenuIndex++;
    btn_down_pressed = 0;
  }
}

void pollValueButtons() {
  if (btn_left_pressed) {
    if (paramMenuIndex == CYCLES) {
      num_cycles--;
    } else if (paramMenuIndex == THRESHOLD) {
      if (sensor_threshold == 0) {
        sensor_threshold = 4096;
        return;
      }
      sensor_threshold--;
    }
  }

  if (btn_right_pressed) {
    if (paramMenuIndex == CYCLES) {
      num_cycles++;
    } else if (paramMenuIndex == THRESHOLD) {
      if (sensor_threshold == 4096) {
        sensor_threshold = 0;
        return;
      }
      sensor_threshold++;
    }
  }
}

uint8_t btn_is_down(GPIO_TypeDef *port, uint16_t pin) {
  return (HAL_GPIO_ReadPin(port, pin) == GPIO_PIN_RESET) ? 1u : 0u;
}
