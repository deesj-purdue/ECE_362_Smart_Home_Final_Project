#ifndef FAN_H
#define FAN_H

#include "globals.h"

/**
 * @brief Initialize TIM3 for PWM Motor output
 */
void init_tim3_fan_pwm();

/**
 * @brief Initialize TIM3 for PWM Motor output
 */
void motor_on_off();

#endif