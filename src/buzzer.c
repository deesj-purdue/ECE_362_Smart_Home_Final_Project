#include "buzzer.h"

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

    // Purposely not enabling TIM1 here, will be enabled/disabled in update_buzzer
}

void TIM1_BRK_UP_TRG_COM_IRQHandler()
{
    TIM1->SR &= ~TIM_SR_UIF; // clear update interrupt flag
    TIM1->ARR = BUZZER_FREQ;
    if (BUZZER_FREQ < MIN_BUZZER_FREQ)
        BUZZER_FREQ = MAX_BUZZER_FREQ;
    else
        BUZZER_FREQ -= 1; // increment frequency
}

void update_buzzer()
{
    if (SECURITY_STATE == ALARM)
        TIM1->CR1 |= TIM_CR1_CEN; // enable TIM1, activates buzzer
    else
        TIM1->CR1 &= ~TIM_CR1_CEN; // disable TIM1, deactivates buzzer
}

void init_led()
{
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    GPIOA->MODER |= 0b01 << GPIO_MODER_MODER3_Pos | 0b01 << GPIO_MODER_MODER4_Pos | 0b01 << GPIO_MODER_MODER5_Pos; // set PB5, 6, 7 to output
    GPIOA->PUPDR |= 0b10 << GPIO_PUPDR_PUPDR3_Pos | 0b10 << GPIO_PUPDR_PUPDR4_Pos | 0b10 << GPIO_PUPDR_PUPDR5_Pos; // pull down PB5, 6, 7

    GPIOA->ODR |= 0b111 << 3; // turn off LEDs
}

void set_led(int led, int state)
{
    if (state)
        GPIOA->ODR &= ~(1 << (led + 3)); // turn on LED
    else
        GPIOA->ODR |= 1 << (led + 3); // turn off LED
}

void update_led()
{
    switch (SECURITY_STATE)
    {
    case DISARMED:
        set_led(0, 0);
        set_led(1, 0);
        set_led(2, 0);
        break;
    case ARMED:
        set_led(0, 0);
        set_led(1, 1);
        set_led(2, 0);
        break;
    case PASSWORD:
        set_led(0, 0);
        set_led(1, 0);
        set_led(2, 1);
        break;
    case ALARM:
        set_led(0, 1);
        set_led(1, 0);
        set_led(2, 0);
        break;
    }
}