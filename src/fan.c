#include "stm32f0xx.h"
#include <stdio.h>
#include "fan.h"

void init_tim3_fan_pwm()
{
    /* setup GPIOA */
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    // Tim3 setup
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    TIM3->CR1 &= ~TIM_CR1_CEN; // disable TIM3

    GPIOA->MODER |= 0b10 << GPIO_MODER_MODER6_Pos;   // set PA6 to AF
    GPIOA->AFR[0] &= ~(0xf << GPIO_AFRL_AFSEL6_Pos); // clear PA6 AF
    GPIOA->AFR[0] |= (0x1 << GPIO_AFRL_AFSEL6_Pos);  // set PA6 to TIM3 AF

    TIM3->PSC = 4800 - 1; // 48 MHz / 4800 = 10 kHz
    TIM3->ARR = 10 - 1;   // 10 kHz / 10 = 1000 Hz

    TIM3->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2; // set PWM mode 1 for OC1M
    TIM3->CCMR1 |= TIM_CCMR1_OC1PE;                     // enable preload register on TIM3_CCR1
    TIM3->CCER |= TIM_CCER_CC1E;                        // enable capture/compare 1 output

    // TIM3->CCR1 = 500; // 50% duty cycle

    TIM3->CR1 |= TIM_CR1_CEN; // enable TIM3
}

void motor_on_off()
{
    // init_tim3_fan_pwm();
    printf("Current temperature: %f\n", CURRENT_TEMPERATURE);
    if (CURRENT_TEMPERATURE >= TARGET_TEMPERATURE)
    {
        TIM3->CCR1 = (CURRENT_TEMPERATURE - TARGET_TEMPERATURE - 3) * 30;
    }
    else
    {
        nano_wait(10000);
        TIM3->CCR1 &= 0;
        nano_wait(10000);
        TIM3->CCR1 &= 0;
        nano_wait(10000);
        TIM3->CCR1 = 0;
        nano_wait(10000);
        TIM3->CCR1 = 0;
    }
}