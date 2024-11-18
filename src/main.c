/**
 ******************************************************************************
 * @file    main.c
 * @author  Jackson Dees, Tristen Hood, Quincy Tordill, Grant Sylvester
 * @date    Oct 28, 2024
 * @brief   ECE 362 Smart Home
 ******************************************************************************
 */
#include "globals.h"
#include "helpers.h"
#include "buzzer.h"
#include "keypad.h"
#include "fan.h"

volatile float CURRENT_TEMPERATURE = 0; // Celsius

volatile float TARGET_TEMPERATURE = 0; // Celsius

volatile char CORRECT_PASSWORD[PASSWORD_LENGTH + 1] = "1234"; // 4 digit pin

volatile int FAN_SPEED = MIN_FAN_SPEED; // 0 - 100% speed range

volatile int BUZZER_FREQ = MAX_BUZZER_FREQ; // modulating frequency for buzzer

volatile enum DoorState DOOR_STATE = CLOSED;                // OPEN, CLOSED
volatile enum SecurityState SECURITY_STATE = DISARMED;      // DISARMED, ARMED, PASSWORD, ALARM
volatile enum KeypadState KEYPAD_STATE = TEMPERATURE_ENTRY; // TEMPERATURE_ENTRY, PASSWORD_ENTRY

/**
 * PINS USED:
 * - PA8: Buzzer PWM
 *
 * - PB5: Red LED
 * - PB6: Green LED
 * - PB7: Blue LED
 *
 * - PC0-3: Keypad Rows
 * - PC4-7: Keypad Columns
 *
 * TIMERS USED:
 * - TIM1: Buzzer PWM
 * - TIM7: Keypad polling
 * - TIM14: Keypad timeout
 *
 * PERIPHERALS USED:
 *
 */

void update_peripheral_states()
{
    update_buzzer();
    update_keypad_state();
    update_led();
}

void init_peripherals()
{
    init_tim1_buzzer_pwm();
    init_led();
    init_tim7_keypad();
    init_tim14_timer();
}

void boot_sequence()
{
    SECURITY_STATE = ALARM;
    update_buzzer();

    for (int i = 0; i < 10; i++)
    {
        set_led(i % 3, 1);
        nano_wait(10000000);
        set_led(i % 3, 0);
    }

    SECURITY_STATE = DISARMED;
    update_buzzer();
}

void fan_control()
{

    motor_on_off();
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

    init_peripherals();
    boot_sequence();
    

    SECURITY_STATE = PASSWORD; // DEBUG initial state

    for (;;)
    {
        update_peripheral_states();

        switch (SECURITY_STATE)
        {
        case DISARMED:
            get_temperature_input();
            fan_control();
            break;
        case ARMED:
            // add door sensor interrupt
            get_temperature_input();
            break;
        case PASSWORD:
            start_password_timeout();
            get_password_string();
            if (check_password())
                SECURITY_STATE = DISARMED;
            else
                SECURITY_STATE = ALARM;
            break;
        case ALARM:
            get_password_string();
            if (check_password())
                SECURITY_STATE = DISARMED;
            break;
        }
    }
    for (;;)
        asm("wfi");
}


