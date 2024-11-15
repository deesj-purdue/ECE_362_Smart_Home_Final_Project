#include "fan.h"

void init_fan_peripherals()
{
    /* setup GPIOA */
    RCC -> AHBENR |= RCC_AHBENR_GPIOA; // temp
    // need to look into which pins are set up for pwm connection examples and H bbridge below
    //https://www.engineersgarage.com/l293d-pin-description-and-working/
    //https://deepbluembedded.com/stm32-dc-motor-speed-control-pwm-l293d-examples/
    GPIOA -> MODER |= (0b01 << 2 * 3) | (0b01 << 2 * 4);
    GPIOA->MODER |= 0b10 << GPIO_MODER_MODER7_Pos;   // set PA7 to Output
    GPIOA->AFR[1] |= 0b0100 << GPIO_AFRH_AFSEL8_Pos; // set PA7 to tim2 AF
    
    
}

void init_tim1_buzzer_pwm()
{
    /* setup GPIOA */
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    GPIOA->MODER |= 0b10 << GPIO_MODER_MODER8_Pos;   // set PA8 to Output
    GPIOA->AFR[1] |= 0b0010 << GPIO_AFRH_AFSEL8_Pos; // set PA8 to tim1 AF

    /* setup TIM1 */
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

    TIM1->BDTR |= TIM_BDTR_MOE; // enable main output
    TIM1->DIER |= TIM_DIER_UIE; // enable update interrupt

    TIM1->PSC = 48 - 1;                   // 48 MHz / 4800 = 10 kHz
    TIM1->ARR = MAX_BUZZER_FREQ - 1;      // 10 kHz / 10 = 1 kHz
    TIM1->CCR1 = MAX_BUZZER_FREQ / 2 - 1; // 50% duty cycle

    TIM1->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2; // set PWM mode 1 for OC1M
    TIM1->CCER |= TIM_CCER_CC1E;                        // enable OC1

    NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
    NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn, 0);

    // Purposely not enabling TIM1 here, will be enabled/disabled in set_buzzer
}

void TIM1_BRK_UP_TRG_COM_IRQHandler()
{
    TIM1->SR &= ~TIM_SR_UIF; // clear update interrupt flag
    TIM1->ARR = BUZZER_FREQ;
    if (BUZZER_FREQ < MIN_BUZZER_FREQ)
        BUZZER_FREQ = MAX_BUZZER_FREQ;
    else
        BUZZER_FREQ -= 2; // increment frequency
}

void set_buzzer()
{
    if (SECURITY_STATE == ALARM)
        TIM1->CR1 |= TIM_CR1_CEN; // enable TIM1, activates buzzer
    else
        TIM1->CR1 &= ~TIM_CR1_CEN; // disable TIM1, deactivates buzzer
}