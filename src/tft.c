#include "tft.h"

/**
 * init_spi1_slow
    This function should configure SPI1 and the GPIOB MODER and AFR registers for pins PB3 (SCK), PB4 (MISO), and PB5 (MOSI). Configure it as follows:

    Set the baud rate divisor to the maximum value to make the SPI baud rate as low as possible.
    Set it to "Master Mode".
    Set the word (data) size to 8-bit.
    Configure "Software Slave Management" and "Internal Slave Select".
    Set the "FIFO reception threshold" bit in CR2 so that the SPI channel immediately releases a received 8-bit value.
    Enable the SPI channel.
 */
void init_spi1_slow()
{
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    // SPI1: PB3 (SCK), PB4 (MISO), PB5 (MOSI)
    GPIOB->MODER &= ~(GPIO_MODER_MODER3_Msk | GPIO_MODER_MODER4_Msk | GPIO_MODER_MODER5_Msk);
    GPIOB->MODER |= (GPIO_MODER_MODER3_1 | GPIO_MODER_MODER4_1 | GPIO_MODER_MODER5_1); // Set to Alternate Function mode

    GPIOB->AFR[0] &= ~(GPIO_AFRL_AFRL3_Msk | GPIO_AFRL_AFRL4_Msk | GPIO_AFRL_AFRL5_Msk);
    GPIOB->AFR[0] |= (0x0 << GPIO_AFRL_AFRL3_Pos) | (0x0 << GPIO_AFRL_AFRL4_Pos) | (0x0 << GPIO_AFRL_AFRL5_Pos); // Set AF0 for SPI1

    SPI1->CR1 |= SPI_CR1_BR;                                             // slowest baud rate
    SPI1->CR1 |= SPI_CR1_MSTR;                                           // master mode
    SPI1->CR2 = (SPI1->CR2 & ~SPI_CR2_DS_Msk) | (0x7 << SPI_CR2_DS_Pos); // 8-bit word size
    SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;                              // Software Slave Management, Internal Slave Select
    SPI1->CR2 |= SPI_CR2_FRXTH;                                          // FIFO reception threshold

    SPI1->CR1 |= SPI_CR1_SPE; // enable SPI
}

/**
 * enable_sdcard
    This function should set PB2 low to enable the SD card.
 */
void enable_sdcard()
{
    GPIOB->ODR &= ~GPIO_ODR_2; // Set PB2 low
}

/**
 * disable_sdcard
    This function should set PB2 high to disable the SD card.
 */
void disable_sdcard()
{
    GPIOB->ODR |= GPIO_ODR_2; // Set PB2 high
}

/**
 * init_sdcard_io
    Create a subroutine named init_sdcard_io that does the following:

    Calls init_spi1_slow().
    Configures PB2 as an output.
    Calls disable_sdcard().
 */
void init_sdcard_io()
{
    init_spi1_slow();

    // Set PB2 to output mode
    GPIOB->MODER &= ~GPIO_MODER_MODER2_Msk;
    GPIOB->MODER |= GPIO_MODER_MODER2_0;

    disable_sdcard();
}

/**
 * sdcard_io_high_speed
    This function is called after SPI1 is initialized. It should do the following:

    Disable the SPI1 channel.
    Set the SPI1 Baud Rate register so that the clock rate is 12 MHz. (You may need to set this lower if your SD card does not reliably work at this rate.)
    Re-enable the SPI1 channel.
 */
void sdcard_io_high_speed()
{
    SPI1->CR1 &= ~SPI_CR1_SPE; // disable SPI
    SPI1->CR1 &= ~SPI_CR1_BR;  // set baud rate to 12 MHz
    SPI1->CR1 |= SPI_CR1_SPE;  // enable SPI
}

/**
 * Write a function named init_lcd_spi() that does the following:

    Configure PB8, PB11, and PB14 as GPIO outputs. Don't forget the clock to GPIOB.
    Call init_spi1_slow() to configure SPI1.
    Call sdcard_io_high_speed() to make SPI1 fast.
    The LCD can work at a rate that is usually - higher than the SD card. If you use the MSP2202 for your project, consider putting it on a separate SPI channel than the SD card interface and running it at a 24 MHz baud rate.
 */
void init_lcd_spi()
{
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    // PB8, PB11, and PB14 outputs
    GPIOB->MODER &= ~(GPIO_MODER_MODER8_Msk | GPIO_MODER_MODER11_Msk | GPIO_MODER_MODER14_Msk);
    GPIOB->MODER |= (GPIO_MODER_MODER8_0 | GPIO_MODER_MODER11_0 | GPIO_MODER_MODER14_0);

    init_spi1_slow();
    sdcard_io_high_speed();
}