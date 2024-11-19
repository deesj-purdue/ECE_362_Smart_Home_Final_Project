#ifndef _LCD_H
#define _LCD_H

#include "globals.h"

typedef struct
{
    uint16_t width;
    uint16_t height;
    uint16_t id;
    uint8_t  dir;
    uint16_t  wramcmd;
    uint16_t  setxcmd;
    uint16_t  setycmd;
    void (*reset)(int);
    void (*select)(int);
    void (*reg_select)(int);
} lcd_dev_t;

extern lcd_dev_t lcddev;

// Rotation:
// 0: rotate 0
// 1: rotate: 90
// 2: rotate: 180
// 3: rotate 270
#define USE_HORIZONTAL       0

// The dimensions of the display.
#define LCD_W 240
#define LCD_H 320

// Some popular colors
#define WHITE       0xFFFF
#define BLACK       0x0000
#define BLUE        0x001F
#define YELLOW      0XFFE0
#define RED         0xF800
#define MAGENTA     0xF81F
#define GREEN       0x07E0
#define CYAN        0x7FFF

/**
 * @brief Initialize SPI1 for LCD display communication
 */
void init_lcd_spi();

/**
 * @brief 
 */
void LCD_Setup(void);

/**
 * 
 */
void LCD_Init(void (*reset)(int), void (*select)(int), void (*reg_select)(int));
void LCD_Clear(uint16_t Color);
void LCD_DrawPoint(uint16_t x,uint16_t y,uint16_t c);
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t c);
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t c);
void LCD_DrawFillRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t c);
void LCD_DrawChar(uint16_t x,uint16_t y,uint16_t fc, uint16_t bc, char num, uint8_t size, uint8_t mode);
void LCD_DrawString(uint16_t x,uint16_t y, uint16_t fc, uint16_t bg, const char *p, uint8_t size, uint8_t mode);

void update_display();

#endif