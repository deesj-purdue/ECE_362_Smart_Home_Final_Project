#include "temperature.h"

void setup_adc1_temperature(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER |= 0xC;                    //initialize GPIOA 

    RCC->APB2ENR |= RCC_APB2ENR_ADCEN;
    RCC->CR2 |= RCC_CR2_HSI14ON;            //enable clock
    while(!(RCC->CR2 & RCC_CR2_HSI14RDY));  //wait until ready

    ADC1->CR |= ADC_CR_ADEN;                //Enable ADC
    while(!(ADC1->ISR & ADC_ISR_ADRDY));    //Wait un ADC is ready

    ADC1->CHSELR = 0;                       //clear ADC
    ADC1->CHSELR |= (1<<1);                 //Set ADC to channel 1
    while(!(ADC1->ISR & ADC_ISR_ADRDY));    //Wait until setup
}

void TIM2_IRQHandler(){

    TIM2->SR &= ~(TIM_SR_UIF); 

    ADC1->CR |= ADC_CR_ADSTART;
    while(!(ADC1->ISR & ADC_ISR_EOC));

    CURRENT_TEMPERATURE = ((ADC1->DR) * 3.3) / 4096.0 / 0.01;
}

void init_tim2(void) {
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

  TIM2->PSC = 47999;
  TIM2->ARR = 999;
  
  TIM2->DIER |= TIM_DIER_UIE;

  NVIC->ISER[0] |= 1 << TIM2_IRQn;

  TIM2->CR1 |= TIM_CR1_CEN;
}