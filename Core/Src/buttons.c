#include "main.h"

volatile uint8_t btn_up_pressed = 0;
volatile uint8_t btn_down_pressed = 0;
volatile uint8_t btn_left_pressed = 0;
volatile uint8_t btn_right_pressed = 0;
volatile uint8_t btn_center_pressed = 0;

/**
 * @brief Handles main menu navigation.
 * Decrements the menu index if the up button is pressed and the index is
 * greater than CLICK. Increments the menu index if the down button is pressed
 * and the index is less than PARAMS. Clears the button flags after processing.
 */
void pollMainMenuButtons() {
  if (btn_up_pressed && mainMenuIndex > CLICK) {
    mainMenuIndex--;
    btn_up_pressed = 0;
  }

  if (btn_down_pressed && mainMenuIndex < PARAMS) {
    mainMenuIndex++;
    btn_down_pressed = 0;
  }

  if (btn_left_pressed && mainMenuIndex == LATENCY) {
    if (mainModeIndex == CLICK) {
      mainModeIndex = EXTERNAL;
      btn_left_pressed = 0;
      return;
    }
    mainModeIndex--;
    btn_left_pressed = 0;
  }

  if (btn_right_pressed && mainMenuIndex == LATENCY) {
    if (mainModeIndex == EXTERNAL) {
      mainModeIndex = CLICK;
      btn_right_pressed = 0;
      return;
    }
    mainModeIndex++;
    btn_right_pressed = 0;
  }
}

/**
 * @brief Handles navigation within the parameter menu.
 * @details Decrements the menu index if the up button is pressed and the index
 * is greater than CYCLES. Increments the menu index if the down button is
 * pressed and the index is less than EXIT. Clears the button flags after
 * processing.
 */
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

/**
 * @brief Adjusts configuration values based on button inputs.
 *
 * Checks the current menu index to determine which variable to modify.
 * For the CYCLES option, it increments or decrements the cycle count.
 * For the THRESHOLD option, it adjusts the sensor threshold value.
 * Handles boundary wrapping for the threshold (0 and 4096).
 */
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
    } else if (paramMenuIndex == SENSOR) {
      if (adc_channel == 4) {
        adc_channel = 1;
        HAL_Delay(ADDITIONAL_DEBOUNCE_MS);
        updateADCChannel();
        return;
      } else if (adc_channel == 1) {
        adc_channel = 4;
        HAL_Delay(ADDITIONAL_DEBOUNCE_MS);
        updateADCChannel();
        return;
      }
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
    } else if (paramMenuIndex == SENSOR) {
      if (adc_channel == 4) {
        adc_channel = 1;
        HAL_Delay(ADDITIONAL_DEBOUNCE_MS);
        updateADCChannel();
        return;
      } else if (adc_channel == 1) {
        adc_channel = 4;
        HAL_Delay(ADDITIONAL_DEBOUNCE_MS);
        updateADCChannel();
        return;
      }
    }
  }
}

/**
 * @brief Checks if the specified GPIO pin is in the low state.
 * @param port The GPIO port to read.
 * @param pin The pin number within the port.
 * @return 1 if the pin is low, 0 if the pin is high.
 */
uint8_t btn_is_down(GPIO_TypeDef *port, uint16_t pin) {
  return (HAL_GPIO_ReadPin(port, pin) == GPIO_PIN_RESET) ? 1u : 0u;
}
