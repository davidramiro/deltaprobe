#pragma once
#include "main.h"
#include <stdint.h>

void poll_main_menu_buttons();
void poll_param_menu_buttons();
void poll_value_buttons();
uint8_t btn_is_down(GPIO_TypeDef *port, uint16_t pin);