#ifndef MAIN_H
#define MAIN_H

#include "stm32f0xx.h"

void internal_clock(void);
void update_peripheral_states(void);
void init_peripherals(void);
void boot_sequence(void);
void update_security(void);
void update_thermostat(void);
void update_everything(void);

#endif // MAIN_H