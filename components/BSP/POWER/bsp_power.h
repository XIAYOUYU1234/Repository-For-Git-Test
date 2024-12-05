#ifndef BSP_POWER_H
#define BSP_POWER_H

#include "driver/gpio.h"

enum GPIO_OUTPUT_STATE
{
    PowerOFF = 0,
    PowerON = 1
};

enum GPIO_INPUT_STATE
{
    PIN_RESET = 0,
    PIN_SET = 1
};

#define POWER_GPIO_PIN  GPIO_NUM_48
#define Sys_Power(x) x?gpio_set_level(POWER_GPIO_PIN, PIN_SET):gpio_set_level(POWER_GPIO_PIN, PIN_RESET)

void Power_Init(void);

#endif //BSP_POWER_H