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
#include "halleffect.h"
#include "lcd.h"
#include "fan.h"
#include "temperature.h"

volatile float CURRENT_TEMPERATURE = 75; // Celsius
volatile float TARGET_TEMPERATURE = 75;  // Celsius

volatile char CORRECT_PASSWORD[PASSWORD_LENGTH + 1] = "1234"; // 4 digit pin
volatile bool KEYPAD_TIMEOUT = false;                         // true if keypad entry entry timed out (breaks out of keypad entry)

volatile int FAN_SPEED = MIN_FAN_SPEED; // 0 - 100% speed range

volatile int BUZZER_FREQ = MAX_BUZZER_FREQ; // modulating frequency for buzzer

volatile enum DoorState DOOR_STATE = CLOSED;                // OPEN, CLOSED
volatile enum SecurityState SECURITY_STATE = DISARMED;      // DISARMED, ARMED, PASSWORD, ALARM
volatile enum KeypadState KEYPAD_STATE = TEMPERATURE_ENTRY; // TEMPERATURE_ENTRY, PASSWORD_ENTRY

volatile bool LCD_ACTIVE = false;

/**
 * PINS USED:
 * - PA0: Hall Effect Sensor
 * - PA1: Temperature sensor ADC
 * - PA8: Buzzer PWM
 *
 * - PA3: Red LED
 * - PA4: Green LED
 * - PA5: Blue LED
 *
 * - PC0-3: Keypad Rows
 * - PC4-7: Keypad Columns
 *
 * TIMERS USED:
 * - TIM1: Buzzer PWM
 * - TIM2: Temprature sensor
 * - TIM7: Keypad polling + Hall Effect
 * - TIM14: Keypad timeout
 *
 * PERIPHERALS USED:
 *
 */

void internal_clock();

void update_peripheral_states()
{
    update_buzzer();
    update_keypad_state();
    update_led();
}

void init_peripherals()
{
    init_tim1_buzzer_pwm();
    setup_adc1_temperature();
    init_tim2();
    init_halleffect();
    init_led();
    init_tim7_keypad();
    init_tim14_timer();
    init_tim3_fan_pwm();
    LCD_Setup();
}

void boot_sequence()
{
    LCD_Clear(BLACK);
    // LCD_DrawFillRectangle(50, 50, LCD_W - 50, LCD_H - 50, MAGENTA);
    LCD_DrawString(0, 0, WHITE, BLACK, "ECE 362 Smart Home", 20, 0);
    LCD_DrawString(0, 20, RED, BLACK, "Jackson Dees", 20, 0);
    LCD_DrawString(0, 40, YELLOW, BLACK, "Tristen Hood", 20, 0);
    LCD_DrawString(0, 60, GREEN, BLACK, "Quincy Tordill", 20, 0);
    LCD_DrawString(0, 80, BLUE, BLACK, "Grant Sylvester", 20, 0);

    SECURITY_STATE = ALARM;
    update_buzzer();

    for (int i = 0; i < 10; i++)
    {
        set_led(i % 3, 1);
        nano_wait(3000000);
        set_led(i % 3, 0);
    }

    SECURITY_STATE = DISARMED;
    update_buzzer();

    nano_wait(100000000);
    LCD_Clear(BLACK);
}

void update_security()
{
    if (SECURITY_STATE == ARMED && DOOR_STATE == OPEN)
    {
        SECURITY_STATE = PASSWORD;
        KEYPAD_TIMEOUT = true;
    }
}

void update_thermostat()
{
    /**
     * fan speed equal to a scaled value of temperature difference
     *
     * Room temperature is 20
     * Skin temperature is ~30 normally
     * Hot skin temperature can get up to 40
     *
     * With scaling factor 5:
     *
     * 20 - 20 = 0 * 3 = 0
     * 30 - 20 = 10 * 5 = 50
     * 40 - 20 = 20 * 5 = 100
     * This ranges about the whole fan speed range for our demo
     */

    int temp_diff = CURRENT_TEMPERATURE - TARGET_TEMPERATURE;
    int scaling_factor = 10;
    int fan_speed = temp_diff * scaling_factor > MAX_FAN_SPEED ? MAX_FAN_SPEED : temp_diff * scaling_factor;

    if (CURRENT_TEMPERATURE > TARGET_TEMPERATURE)
        FAN_SPEED = fan_speed;
    else
        FAN_SPEED = MIN_FAN_SPEED;
}

/**
 * Updates everything in this function on every interrupt
 */
void update_everything()
{
    update_thermostat();
    motor_on_off();
    update_peripheral_states();
    update_security();
    update_door_state();
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

    internal_clock();
    init_peripherals();
    boot_sequence();

    SECURITY_STATE = ARMED; // DEBUG initial state
    LCD_ACTIVE = true;

    for (;;)
    {
        switch (SECURITY_STATE)
        {
        case DISARMED:
            get_temperature_input();
            motor_on_off();
            break;
        case ARMED:
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
