#include "stm32f0xx.h"
#include <stdio.h>
#include "fan.h"



void init_tim3_fan_pwm()
{
    /* setup GPIOA */
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    // Tim3 setup
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;


    GPIOA -> MODER |= 0b10 << GPIO_MODER_MODER6_Pos;   // set PA6 to AF
    GPIOA -> AFR[0] &= ~(0xf << GPIO_AFRL_AFSEL6_Pos); // clcear  PA6 AF
    GPIOA -> AFR[0] |= (0x1 << GPIO_AFRL_AFSEL6_Pos); // set PA6 to tim1 AF

    

    // TIM3->BDTR |= TIM_BDTR_MOE; // enable main output
    // TIM3->DIER |= TIM_DIER_UIE; // enable update interrupt

    TIM3->PSC = 48000 - 1;                   // 48 MHz / 4800 = 10 kHz
    TIM3->ARR = 10 - 1;      // 10 kHz / 10 = 1000 Hz
    

    TIM3->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2; // set PWM mode 1 for OC1M
    TIM3 -> CCER |= TIM_CCER_CC1E; 
    // TIM3 -> CCMR1 |= TIM_CCMR1_OC1PE;
    // TIM3 -> SR &= ~TIM_SR_UIF;
    // TIM3 -> CR1 |= TIM_CR1_ARPE;                       // enable OC1

    TIM3->CCR1 =  500; // 50% duty cycle

}

void motor_on_off()
{
    init_tim3_fan_pwm();
    // if (CURRENT_TEMPERATURE >= 27)
    // {
        TIM3 -> CR1 |= TIM_CR1_CEN;
    // }
    // else{
    //     TIM3 -> CR1 &= ~TIM_CR1_CEN;
    // }

}