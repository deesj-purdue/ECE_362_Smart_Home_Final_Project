#ifndef HALLEFFECT_H
#define HALLEFFECT_H

#include "globals.h"

/**
 * @brief Initialize GPIOx for hall effect sensor
 */
void init_halleffect(void);
/**
 * @brief Update the Current Door State 1 = Open, 0 = Closed
 */
void update_door_state(void);

#endif