#include "sleep.h"
#include "main.h"

volatile uint8_t led_interrupt_counter = 0;
volatile uint16_t standby_interrupt_counter = 0;
volatile uint8_t sleep_requested = 0;
volatile uint8_t wakeup_requested = 0;
volatile uint8_t display_sleeping = 0;

void handleMCUSleep() {
  if (sleep_requested) {
    sleep_requested = 0;
    u8g2_SetPowerSave(&u8g2, 1);

    display_sleeping = 1;

    HAL_TIM_Base_Stop_IT(&htim3);
    HAL_TIM_Base_Stop_IT(&htim4);
    HAL_SuspendTick();

    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

    HAL_ResumeTick();
    HAL_TIM_Base_Start_IT(&htim3);
    HAL_TIM_Base_Start_IT(&htim4);

    u8g2_SetPowerSave(&u8g2, 0);
    display_sleeping = 0;
    wakeup_requested = 0;
  }
}

void handleDisplaySleep() {
  if (display_sleeping) {
    HAL_GPIO_WritePin(INF_LED_GPIO_Port, INF_LED_Pin,
                      led_interrupt_counter % 5 == 0);
  }
  if (sleep_requested) {
    if (!display_sleeping) {
      u8g2_SetPowerSave(&u8g2, 1);
      display_sleeping = 1;
    }
    sleep_requested = 0;
  }
  if (wakeup_requested) {
    if (display_sleeping) {
      u8g2_SetPowerSave(&u8g2, 0);
      display_sleeping = 0;
      HAL_GPIO_WritePin(INF_LED_GPIO_Port, INF_LED_Pin, GPIO_PIN_SET);
    }
  }
  wakeup_requested = 0;
}