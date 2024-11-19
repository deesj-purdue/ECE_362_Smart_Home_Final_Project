#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include "globals.h"
#include "main.h"
#include "lcd.h"

/**
 * @brief Initialize ADC1 to input from the Temperature sensor
 */
void setup_adc1_temperature(void);

/**
 * @brief Initialize TIM2 for temperature sensor
 */
void TIM2_IRQHandler(void);

/**
 * @brief Initialize TIM2 for temperature sensor
 */
void init_tim2(void);

#endif