#include "keypad.h"

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

int check_password_digit(int digit, int position)
{
    return (CORRECT_PASSWORD / (int)pow(10, PASSWORD_LENGTH - position - 1)) % 10 == digit;
}
