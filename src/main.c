/**
 ******************************************************************************
 * @file    main.c
 * @author  Jackson Dees,
 * @date    Oct 21, 2024
 * @brief   ECE 362 Smart Home
 ******************************************************************************
 */

#include "stm32f0xx.h"
#include <stdlib.h>

/* GLOBAL VARIABLES */

float CURRENT_TEMPERATURE = 0; // Celsius
float TARGET_TEMPERATURE = 0;  // Celsius
int CORRECT_PASSWORD = 1234;   // 4 digit pin
int FAN_SPEED = 0;             // 0 - 100% speed range
enum DOOR_STATE
{
  OPEN,
  CLOSED
};
enum SECURITY_STATE
{
  DISARMED,       // Door open state will NOT set off alarm
  ARMED,          // Door open WILL set off alarm
  PASSWORD_ENTRY, // Door has just been opened, correct password sends to DISARMED, incorrect password sends to ALARM
  ALARM           // buzzer turned on, will not disable unless correct password is entered
};

/* PINS USED */
/**
 * PA8: tim1 for buzzer
 */

void init_tim1_buzzer_pwm()
{
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

  GPIOA->MODER |= 0b10 << GPIO_MODER_MODER8_Pos;   // set PA8 to Output
  GPIOA->AFR[1] |= 0b0010 << GPIO_AFRH_AFSEL8_Pos; // set PA8 to tim1 AF

  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

  TIM1->BDTR |= TIM_BDTR_MOE;
  TIM1->PSC = 4799; // 48 MHz / 4800 = 10 kHz
  TIM1->ARR = 9;    // 10 kHz / 10 = 1 kHz
  TIM1->CCR1 = 499; // 50% duty cycle

  TIM1->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
  TIM1->CCER |= TIM_CCER_CC1E;

  TIM1->CR1 |= TIM_CR1_CEN;
}

int main()
{
  for (;;)
  {
    asm("wfi");
  }
}