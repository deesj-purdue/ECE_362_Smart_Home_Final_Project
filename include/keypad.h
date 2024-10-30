#ifndef KEYPAD_H
#define KEYPAD_H

#include "globals.h"

/**
 * @brief Updates the keypad state based on the current SECURITY_STATE and DOOR_STATE
 */
void update_keypad_state();

/**
 * @brief Checks if a digit is correct in the password
 * @param input: the digit to check
 * @param digit: the position of the digit in the password
 * @return 1 if correct, 0 if incorrect
 */
int check_password_digit(int digit, int position);


#endif // KEYPAD_H