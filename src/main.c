/**
 ******************************************************************************
 * @file    main.c
 * @author  Jackson Dees,
 * @date    Oct 28, 2024
 * @brief   ECE 362 Smart Home
 ******************************************************************************
 */

#include "stm32f0xx.h"
#include <stdlib.h>
#include <math.h>

/* GLOBAL VARIABLES */

void init_tim1_buzzer_pwm();
void set_buzzer(enum SECURITY_STATE state);
int check_password_digit(int digit, int position);

#define PASSWORD_LENGTH 4
#define PASSWORD_TIMEOUT_S 5           // 5 seconds
#define PASSWORD_TIMEOUT_MS 5000       // 5 seconds
#define PASSWORD_TIMEOUT_NS 5000000000 // 5 seconds

#define MIN_FAN_SPEED 0
#define MAX_FAN_SPEED 100

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

volatile float CURRENT_TEMPERATURE = 0; // Celsius
volatile float TARGET_TEMPERATURE = 0;  // Celsius

volatile int CORRECT_PASSWORD = 1234; // 4 digit pin

volatile int FAN_SPEED = 0; // 0 - 100% speed range

volatile enum DOOR_STATE DOOR = CLOSED;           // door state
volatile enum SECURITY_STATE SECURITY = DISARMED; // security state

/* PINS USED */
/**
 * PA8: tim1 for buzzer
 *
 * whichever pins are used for the keypad
 */

/**
 * @brief Initialize TIM1 for PWM buzzer output
 */
void init_tim1_buzzer_pwm()
{
    /* setup GPIOA */
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    GPIOA->MODER |= 0b10 << GPIO_MODER_MODER8_Pos;   // set PA8 to Output
    GPIOA->AFR[1] |= 0b0010 << GPIO_AFRH_AFSEL8_Pos; // set PA8 to tim1 AF

    /* setup TIM1 */
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

    TIM1->BDTR |= TIM_BDTR_MOE;

    TIM1->PSC = 4800 - 1; // 48 MHz / 4800 = 10 kHz
    TIM1->ARR = 10 - 1;   // 10 kHz / 10 = 1 kHz
    TIM1->CCR1 = 500 - 1; // 50% duty cycle

    TIM1->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2; // set PWM mode 1 for OC1M
    TIM1->CCER |= TIM_CCER_CC1E;                        // enable OC1

    // Purposely not enabling TIM1 here, will be enabled/disabled in set_buzzer
}

/**
 * @brief Turns buzzer on or off depending on SECURITY_STATE
 * @param state: the state of the security system
 *
 * may or may not change this to a bool input instead of state
 */
void set_buzzer(enum SECURITY_STATE state)
{
    if (state == ALARM)
        TIM1->CR1 |= TIM_CR1_CEN; // enable TIM1, activates buzzer
    else
        TIM1->CR1 &= ~TIM_CR1_CEN; // disable TIM1, deactivates buzzer
}

/**
 * @brief Checks if a digit is correct in the password
 * @param input: the digit to check
 * @param digit: the position of the digit in the password
 * @return 1 if correct, 0 if incorrect
 */
int check_password_digit(int digit, int position)
{
    return (CORRECT_PASSWORD / (int)pow(10, PASSWORD_LENGTH - position - 1)) % 10 == digit;
}

int main()
{
    /**
     * Temperature Control Flow
     * 1. Read temperature sensor in timer interrupt
     * 2. Compare current temperature to target temperature
     * 3. Adjust fan speed to reach target temperature
     *
     * Security Control Flow
     * 1. Door sensor interrupt triggers
     * 2. Check door state against security state
     * 3. Table of security states:
     * |                | Open                   | Closed                 |
     * |----------------|------------------------|------------------------|
     * | DISARMED       | do nothing             | do nothing             |
     * | ARMED          | goto PASSWORD_ENTRY    | do nothing             |
     * | PASSWORD_ENTRY | correct: goto DISARMED | correct: goto DISARMED |
     * |                | incorrect: goto ALARM  | incorrect: goto ALARM  |
     * |                | timeout: goto ALARM    | timeout: goto ALARM    |
     * | ALARM          | wait for password      | wait for password      |
     *
     * when in PASSWORD_ENTRY, activate keypad for password entry instead of temperature control and start timeout timer
     */

    /* init all peripherals */
    init_tim1_buzzer_pwm();

    for (;;)
        asm("wfi");
}