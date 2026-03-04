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
void poll_main_menu_buttons() {
  if (btn_up_pressed && main_menu_selector > LATENCY) {
    main_menu_selector--;
    btn_up_pressed = 0;
  }

  if (btn_down_pressed && main_menu_selector < PARAMS) {
    main_menu_selector++;
    btn_down_pressed = 0;
  }

  if (btn_left_pressed && main_menu_selector == LATENCY) {
    if (latency_mode_selector == CLICK) {
      latency_mode_selector = EXTERNAL;
      btn_left_pressed = 0;
      return;
    }
    latency_mode_selector--;
    btn_left_pressed = 0;
  }

  if (btn_right_pressed && main_menu_selector == LATENCY) {
    if (latency_mode_selector == EXTERNAL) {
      latency_mode_selector = CLICK;
      btn_right_pressed = 0;
      return;
    }
    latency_mode_selector++;
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
void poll_param_menu_buttons() {
  if (btn_up_pressed && params_menu_selector > CYCLES) {
    params_menu_selector--;
    btn_up_pressed = 0;
  }
  if (btn_down_pressed && params_menu_selector < EXIT) {
    params_menu_selector++;
    btn_down_pressed = 0;
  }
}

/**
 * @brief Adjusts configuration values based on button inputs.
 *
 * Checks the current menu index to determine which variable to modify.
 * For the CYCLES option, it increments or decrements the cycle count.
 * For the THRESHOLD option, it adjusts the sensor threshold value.
 * Handles boundary wrapping for the threshold (0 and 4095).
 */
void poll_value_buttons() {
  if (btn_left_pressed) {
    if (params_menu_selector == CYCLES) {
      num_cycles--;
    } else if (params_menu_selector == THRESHOLD) {
      if (sensor_threshold == 0) {
        sensor_threshold = 4095;
        return;
      }
      sensor_threshold--;
    } else if (params_menu_selector == SENSOR) {
      if (adc_channel == 4) {
        adc_channel = 1;
      } else if (adc_channel == 1) {
        adc_channel = 4;
      }
      update_ADC_channel();
      btn_left_pressed = 0;
    }
  }

  if (btn_right_pressed) {
    if (params_menu_selector == CYCLES) {
      num_cycles++;
    } else if (params_menu_selector == THRESHOLD) {
      if (sensor_threshold == 4095) {
        sensor_threshold = 0;
        return;
      }
      sensor_threshold++;
    } else if (params_menu_selector == SENSOR) {
      if (adc_channel == 4) {
        adc_channel = 1;
      } else if (adc_channel == 1) {
        adc_channel = 4;
      }
      update_ADC_channel();
      btn_right_pressed = 0;
    }
  }
}

/**
 * @brief Checks if the specified GPIO pin is in the low state.
 * @param port The GPIO port to read.
 * @param pin The pin number within the port.
 * @return 1 if the pin is low, 0 if the pin is high.
 */
uint8_t btn_is_down(GPIO_TypeDef *port, const uint16_t pin) {
  return HAL_GPIO_ReadPin(port, pin) == GPIO_PIN_RESET ? 1u : 0u;
}
