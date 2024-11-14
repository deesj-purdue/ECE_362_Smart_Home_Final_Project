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

extern volatile uint8_t col;
extern volatile char* keymap;
extern volatile uint8_t hist[16];
extern volatile char queue[2];  // A two-entry queue of button press/release events.
extern volatile int qin;        // Which queue entry is next for input
extern volatile int qout;       // Which queue entry is next for output
extern volatile char password_entry_str[5];

#endif // GLOBALS_H