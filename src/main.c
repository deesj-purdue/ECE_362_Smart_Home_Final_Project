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
  DISARMED, // Door open state will NOT set off alarm
  ARMED, // Door open WILL set off alarm
  PASSWORD_ENTRY, // Door has just been opened, correct password sends to DISARMED, incorrect password sends to ALARM
  ALARM // buzzer turned on, will not disable unless correct password is entered
};

void init_tim1_buzzer_pwm()
{
  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

  TIM1->PSC = 4799; // 48 MHz / 4800 = 10 kHz
  TIM1->ARR = 9;    // 10 kHz / 10 = 1 kHz
  TIM1->CCR1 = 499; // 50% duty cycle

  TIM1->CR1 |= TIM_CR1_CEN;
}

int main()
{
  for (;;)
  {
    asm("wfi");
  }
}