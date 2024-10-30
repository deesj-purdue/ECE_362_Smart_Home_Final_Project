#ifndef GLOBALS_H
#define GLOBALS_H

#include "stm32f0xx.h"
#include <stdlib.h>
#include <math.h>

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

extern volatile float CURRENT_TEMPERATURE; // Celsius

extern volatile float TARGET_TEMPERATURE;  // Celsius

extern volatile int CORRECT_PASSWORD; // 4 digit pin

extern volatile int FAN_SPEED; // 0 - 100% speed range

extern volatile int BUZZER_FREQ; // modulating frequency for buzzer

extern volatile enum DoorState DOOR_STATE;                // OPEN, CLOSED
extern volatile enum SecurityState SECURITY_STATE;      // DISARMED, ARMED, PASSWORD, ALARM
extern volatile enum KeypadState KEYPAD_STATE; // TEMPERATURE_ENTRY, PASSWORD_ENTRY

#endif // GLOBALS_H