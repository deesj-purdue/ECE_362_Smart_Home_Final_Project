#ifndef KEYPAD_H
#define KEYPAD_H

#include "globals.h"

/**
 * @brief Updates the keypad state based on the current SECURITY_STATE
 *
 * DISARMED, ARMED: Temperature Entry
 *
 * PASSWORD, ALARM: Password Entry
 */
void update_keypad_state();

/**
 * @brief Checks if the input password is correct
 */
bool check_password();

/**
 * @brief Initialize TIM7 for keypad reading
 */
void init_tim7_keypad();

/**
 * @brief TIM1 interrupt handler for reading column of keypad
 */
void TIM7_IRQHandler();

// Keypad Helpers
/**
 * @brief Reads the rows of the currently active column
 * @return 4 bits representing rows (MSB is top row)
 */
int read_rows();

/**
 * @brief Gets password string from user through keypad (updates global password_entry_str)
 */
void get_password_string();

/**
 * @brief Gets temperature integer from user through keypad (updates global TARGET_TEMPERATURE)
 */
void get_temperature_input();

/**
 * @brief pushes keypress event into event queue
 * @param n: keypress event, press/release (1/0) flag followed by ascii value for key
 */
void push_queue(int n);

/**
 * @brief pops keypress event out of event queue
 * @return keypress event, press/release (1/0) flag followed by ascii value for key
 */
char pop_queue();

/**
 * @brief shifts current row values into the history bytes for the associated key in the hist[] array
 * @param col: global col variable, the current column being scanned
 * @param rows: 4 bit value describing the status of the rows scanned in the column
 */
void update_history(int col, int rows);

/**
 * @brief waits for nonzero event in the event queue, then returns it
 * @return key event char (not the correct key if it has the flag enabled, get_keypress is required to handle that)
 */
char get_key_event();

/**
 * @brief Waits for a keypress (not release) and returns appropriate key
 * @return char representing the key that was pressed
 */
char get_keypress();

/**
 * @brief Drives current column in col to high voltage
 * @param col: current column (global variable)
 */
void drive_column(uint8_t col);

/**
 * @brief Initializes TIM2 for keypad timeout
 */
void init_tim14_timer();

/**
 * @brief Starts the keypad timeout timer
 */
void start_password_timeout();

/**
 * @brief Stops the keypad timeout timer
 */
void stop_password_timeout();

#endif // KEYPAD_H