#include "halleffect.h"
#include "globals.h"

#define BOXCAR_SIZE 20

static int door_state_buffer[BOXCAR_SIZE] = {0};
static int buffer_index = 0;

void init_halleffect(void)
{
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

  GPIOA->MODER &= 0xFFFFFFF0;
  GPIOA->MODER &= ~GPIO_MODER_MODER0;
  GPIOA->PUPDR |= 0x1;
}

void update_door_state(void)
{
  int current_state = (GPIOA->IDR & GPIO_IDR_0) ? CLOSED : OPEN;

  door_state_buffer[buffer_index] = current_state;
  buffer_index = (buffer_index + 1) % BOXCAR_SIZE;

  int sum = 0;
  for (int i = 0; i < BOXCAR_SIZE; i++)
  {
    sum += door_state_buffer[i];
  }
  int average_state = (sum >= (BOXCAR_SIZE / 2)) ? CLOSED : OPEN;

  DOOR_STATE = average_state;

  if (DOOR_STATE == OPEN && SECURITY_STATE == ARMED)
  {
    KEYPAD_TIMEOUT = true;
  }
}