#pragma once
#include "main.h"
#include <stdint.h>

void pollMainMenuButtons();
void pollParamMenuButtons();
void pollValueButtons();
uint8_t btn_is_down(GPIO_TypeDef *port, uint16_t pin);