#include "halleffect.h"
#include "globals.h"

void init_halleffect(void){
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

  GPIOA->MODER &= 0xFFFFFFF0;
  GPIOA->MODER &= ~GPIO_MODER_MODER0;
  GPIOA->PUPDR |= 0x1;
}

void update_door_state(void){
    DOOR_STATE = (GPIOA->IDR & GPIO_IDR_0) ? OPEN: CLOSED;
}