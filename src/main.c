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

#define PASSWORD_LENGTH 4
#define PASSWORD_TIMEOUT_S 5     // 5 seconds
#define PASSWORD_TIMEOUT_MS 5000 // 5 seconds

#define MIN_FAN_SPEED 0
#define MAX_FAN_SPEED 100

#define MAX_BUZZER_FREQ 1000
#define MIN_BUZZER_FREQ 500

enum DoorState
{
    OPEN,
    CLOSED
};
enum SecurityState
{
    DISARMED, // Door open state will NOT set off alarm
    ARMED,    // Door open WILL set off alarm
    PASSWORD, // Door has just been opened, correct password sends to DISARMED, incorrect password sends to ALARM
    ALARM     // buzzer turned on, will not disable unless correct password is entered
};

enum KeypadState
{
    TEMPERATURE_ENTRY,
    PASSWORD_ENTRY
};

volatile float CURRENT_TEMPERATURE = 0; // Celsius
volatile float TARGET_TEMPERATURE = 0;  // Celsius

volatile int CORRECT_PASSWORD = 1234; // 4 digit pin

volatile int FAN_SPEED = MIN_FAN_SPEED; // 0 - 100% speed range

volatile int BUZZER_FREQ = MAX_BUZZER_FREQ; // modulating frequency for buzzer

volatile enum DoorState DOOR_STATE = CLOSED;                // OPEN, CLOSED
volatile enum SecurityState SECURITY_STATE = DISARMED;      // DISARMED, ARMED, PASSWORD, ALARM
volatile enum KeypadState KEYPAD_STATE = TEMPERATURE_ENTRY; // TEMPERATURE_ENTRY, PASSWORD_ENTRY

void nano_wait(unsigned int n);
void init_tim1_buzzer_pwm();
void set_buzzer();
int check_password_digit(int digit, int position);

/* PINS USED */
/**
 * PA8: tim1 for buzzer
 *
 * whichever pins are used for the keypad
 */

void nano_wait(unsigned int n)
{
    asm("        mov r0,%0\n"
        "repeat: sub r0,#83\n"
        "        bgt repeat\n" : : "r"(n) : "r0", "cc");
}

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

    TIM1->BDTR |= TIM_BDTR_MOE; // enable main output
    TIM1->DIER |= TIM_DIER_UIE; // enable update interrupt

    TIM1->PSC = 48 - 1;                   // 48 MHz / 4800 = 10 kHz
    TIM1->ARR = MAX_BUZZER_FREQ - 1;      // 10 kHz / 10 = 1 kHz
    TIM1->CCR1 = MAX_BUZZER_FREQ / 2 - 1; // 50% duty cycle

    TIM1->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2; // set PWM mode 1 for OC1M
    TIM1->CCER |= TIM_CCER_CC1E;                        // enable OC1

    NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);

    // Purposely not enabling TIM1 here, will be enabled/disabled in set_buzzer
}

/**
 * @brief TIM1 interrupt handler for buzzer frequency modulation
 */
void TIM1_BRK_UP_TRG_COM_IRQHandler()
{
    TIM1->SR &= ~TIM_SR_UIF; // clear update interrupt flag
    TIM1->ARR = BUZZER_FREQ;
    if (BUZZER_FREQ < MIN_BUZZER_FREQ)
        BUZZER_FREQ = MAX_BUZZER_FREQ;
    else
        BUZZER_FREQ -= 1; // increment frequency
}

/**
 * @brief Turns buzzer on or off depending on SECURITY_STATE
 */
void set_buzzer()
{
    if (SECURITY_STATE == ALARM)
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

/**
 * @brief Updates the keypad state based on the current SECURITY_STATE and DOOR_STATE
 */
void update_keypad_state()
{
    switch (SECURITY_STATE)
    {
    case DISARMED:
        KEYPAD_STATE = TEMPERATURE_ENTRY;
        break;
    case ARMED:
        if (DOOR_STATE == OPEN)
            KEYPAD_STATE = PASSWORD_ENTRY;
        else
            KEYPAD_STATE = TEMPERATURE_ENTRY;
        break;
    case PASSWORD:
        KEYPAD_STATE = PASSWORD_ENTRY;
        break;
    case ALARM:
        KEYPAD_STATE = PASSWORD_ENTRY;
        break;
    }
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
     * | PASSWORD       | correct: goto DISARMED | correct: goto DISARMED |
     * |                | incorrect: goto ALARM  | incorrect: goto ALARM  |
     * |                | timeout: goto ALARM    | timeout: goto ALARM    |
     * | ALARM          | wait for password      | wait for password      |
     *
     * when in PASSWORD_ENTRY, activate keypad for password entry instead of temperature control and start timeout timer
     */

    /* init all peripherals */
    init_tim1_buzzer_pwm();
    SECURITY_STATE = ALARM;
    set_buzzer();
    nano_wait(1000000000); // 1 second
    SECURITY_STATE = DISARMED;
    set_buzzer();

    for (;;)
        asm("wfi");
}