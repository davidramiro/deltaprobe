#include <stdint.h>
#include <stdlib.h>

#include "display.h"
#include "main.h"
#include "measure.h"
#include "usb.h"
#include "usbd.h"

#include <math.h>

#include "../../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal.h"

uint32_t read_single_ADC() {
  uint32_t adc_val = 0;
  HAL_ADC_Start(&hadc1);
  if (HAL_ADC_PollForConversion(&hadc1, 10) != HAL_TIMEOUT) {
    adc_val = HAL_ADC_GetValue(&hadc1);
  }
  HAL_ADC_Stop(&hadc1);

  return adc_val;
}

uint32_t get_3sample_average_ADC() {
  uint32_t adc_val = 0;
  for (int i = 0; i < 3; i++) {
    adc_val += read_single_ADC();
  }
  return adc_val / 3;
}

/**
 * @brief Sends a HID event, measures latency in microseconds until ADC value
 * changes beyond threshold.
 * @param latencies_us Array to store the measured latencies in microseconds.
 * @return int8_t Error code
 */
int8_t measure(uint32_t latencies_us[]) {
  const uint32_t baseline = read_single_ADC();
  
  render_measurement(baseline, -1, -1);

  HAL_TIM_Base_Stop_IT(&htim2);
  __HAL_TIM_SET_COUNTER(&htim2, 0);
  HAL_TIM_Base_Start_IT(&htim2);

  if (latency_mode_selector == EXTERNAL) {
    HAL_GPIO_WritePin(EXT_TRIGGER_GPIO_Port, EXT_TRIGGER_Pin, GPIO_PIN_SET);
  } else {
    const int8_t error = start_mouse_action();
    if (error) {
      return error;
    }
  }

  const uint32_t start = TIM2->CNT;

  while (1) {
    tud_task();

    const int32_t delta = read_single_ADC() - baseline;

    if (abs(delta) > sensor_threshold) {
      const uint32_t latency = (uint32_t)__HAL_TIM_GET_COUNTER(&htim2) - start;
      if (latency_mode_selector == EXTERNAL) {
        HAL_GPIO_WritePin(EXT_TRIGGER_GPIO_Port, EXT_TRIGGER_Pin,
                          GPIO_PIN_RESET);
      } else {
        const int8_t error = stop_mouse_action();
        if (error) {
          return error;
        }
      }

      if (cycle_index < num_cycles) {
        latencies_us[cycle_index] = latency;
      }

      render_measurement(baseline, baseline + delta, latency);

      HAL_Delay(MEASUREMENT_DELAY);

      break;
    }
  }

  return 0;
}

/**
 * @brief Calculates the mean and standard deviation of latency measurements and
 * converts them to milliseconds.
 * @param latencies_us Array containing latency measurements in microseconds.
 * @param mean_ms Pointer to a float where the mean latency in milliseconds will
 * be stored.
 * @param sd_ms Pointer to a float where the standard deviation in milliseconds
 * will be stored.
 */
void compute_latency_stats(const uint32_t latencies_us[], float *mean_ms, float *sd_ms) {
  float sum_us = 0.0f;
  float variance_us = 0.0f;

  if (num_cycles == 1) {
    *mean_ms = latencies_us[0] / MS_FACTOR;
    *sd_ms = 0.0f;
    return;
  }

  // calculate mean
  for (int i = 0; i < num_cycles; i++) {
    sum_us += (float)latencies_us[i];
  }
  const float mean_us = sum_us / (float)num_cycles;

  // calculate sample standard deviation
  for (int i = 0; i < num_cycles; i++) {
    const float diff_us = (float)latencies_us[i] - mean_us;
    variance_us += diff_us * diff_us;
  }
  const float sd_us = sqrtf(variance_us / (float)(num_cycles - 1));

  *mean_ms = mean_us / MS_FACTOR;
  *sd_ms = sd_us / MS_FACTOR;
}
