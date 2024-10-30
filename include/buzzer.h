#ifndef BUZZER_H
#define BUZZER_H

#include "globals.h"

/**
 * @brief Initialize TIM1 for PWM buzzer output
 */
void init_tim1_buzzer_pwm();

/**
 * @brief TIM1 interrupt handler for buzzer frequency modulation
 */
void TIM1_BRK_UP_TRG_COM_IRQHandler();

/**
 * @brief Turns buzzer on or off depending on SECURITY_STATE
 */
void set_buzzer();

#endif // BUZZER_H