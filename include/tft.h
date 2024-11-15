#ifndef TFT_H
#define TFT_H

#include "globals.h"

#include "lcd_files/commands.h"
#include "lcd_files/diskio.h"
#include "lcd_files/ff.h"
#include "lcd_files/ffconf.h"
#include "lcd_files/fifo.h"
#include "lcd_files/lcd.h"
#include "lcd_files/tty.h"

void init_spi1_slow(void);
void enable_sdcard(void);
void disable_sdcard(void);
void init_sdcard_io(void);
void sdcard_io_high_speed(void);
void init_lcd_spi(void);

#endif // TFT_H