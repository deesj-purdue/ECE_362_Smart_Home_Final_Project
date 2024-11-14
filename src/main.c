/**
 ******************************************************************************
 * @file    main.c
 * @author  Jackson Dees,
 * @date    Oct 28, 2024
 * @brief   ECE 362 Smart Home
 ******************************************************************************
 */
#include "globals.h"
#include "helpers.h"
#include "buzzer.h"
#include "keypad.h"

volatile float CURRENT_TEMPERATURE = 0; // Celsius

volatile float TARGET_TEMPERATURE = 0; // Celsius

volatile int CORRECT_PASSWORD = 1234; // 4 digit pin

volatile int FAN_SPEED = MIN_FAN_SPEED; // 0 - 100% speed range

volatile int BUZZER_FREQ = MAX_BUZZER_FREQ; // modulating frequency for buzzer

volatile enum DoorState DOOR_STATE = CLOSED;                // OPEN, CLOSED
volatile enum SecurityState SECURITY_STATE = DISARMED;      // DISARMED, ARMED, PASSWORD, ALARM
volatile enum KeypadState KEYPAD_STATE = TEMPERATURE_ENTRY; // TEMPERATURE_ENTRY, PASSWORD_ENTRY

/**
 * PINS USED:
 * - PA8: Buzzer PWM
 * 
 * TIMERS USED:
 * - TIM1: Buzzer PWM
 * 
 * PERIPHERALS USED:
 * 
 */

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
    init_tim7_keypad();
    SECURITY_STATE = ALARM;
    set_buzzer();
    nano_wait(1000000000); // 1 second
    SECURITY_STATE = DISARMED;
    set_buzzer();
    // get_password_string();

    for (;;)
        asm("wfi");
}