#ifndef BUZZER_H
#define BUZZER_H

#include "globals.h"
#include "main.h"

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
 *
 * ALARM: on
 *
 * DISARMED, ARMED, PASSWORD: off
 */
void update_buzzer();

/**
 * @brief Initialize GPIOB for LED Indicator
 */
void init_led();

/**
 * @brief Turns LED on or off
 * @param led: LED number (0: red, 1: green, 2: blue)
 * @param state: 1 for on, 0 for off
 */
void set_led(int led, int state);

/**
 * @brief Updates LED states based on SECURITY_STATE
 *
 * DISARMED: all off
 *
 * ARMED: green on
 *
 * PASSWORD: blue on
 *
 * ALARM: red on
 */
void update_led();

#endif // BUZZER_H