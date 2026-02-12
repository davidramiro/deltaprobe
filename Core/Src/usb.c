#include <stdint.h>
#include "main.h"
#include "usb.h"
#include "hid_device.h"
#include "usbd.h"
#include "usb_descriptors.h"

uint32_t startMouseAction() {
    HAL_TIM_Base_Stop_IT(&htim2);
    __HAL_TIM_SET_COUNTER(&htim2, 0);
    HAL_TIM_Base_Start_IT(&htim2);

    hid_mouse_report_t report = {
        .wheel = 0,
        .pan = 0
      };

    if (mainMenuIndex == CLICK) {
        report.buttons = 1;
    } else if (mainMenuIndex == MOVE) {
        report.x = 127,
        report.y = 127;
    }

    while (!tud_hid_ready()) {
        tud_task();
    }

    tud_hid_report(REPORT_ID_MOUSE, &report, sizeof(report));

    return TIM2->CNT;
}

void stopMouseAction() {

    hid_mouse_report_t report = {
        .wheel = 0,
        .pan = 0
      };

    if (mainMenuIndex == CLICK) {
        report.buttons = 0;
    } else if (mainMenuIndex == MOVE) {
        report.x = -127,
        report.y = -127;
    }

    while (!tud_hid_ready()) {
        tud_task();
    }

    tud_hid_report(REPORT_ID_MOUSE, &report, sizeof(report));
}