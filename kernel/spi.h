/*
   SPI kernel module: 

 */

#ifndef __HAMSTER_KERNEL_SPI
#define __HAMSTER_KERNEL_SPI

#include "kernel.h"


#define IO_ADDR_DISPLAY 0x01
#define IO_ADDR_FLASH 0x02
#define IO_ADDR_EXPANDER 0x00



/* initializing SPI (also enable it) in non-buffered mode, with CLK/4 */
void spi_init();



/* enable SPI */
INLINE void spi_enable() {
    SPI0.CTRLA |=  SPI_ENABLE_bm;  
}

/* disable SPI */
INLINE void spi_disable() {
    SPI0.CTRLA &= ~ SPI_ENABLE_bm; 
}

INLINE void spi_all_io_disable() {
    PORTB.OUT = 0x03;
}

INLINE void spi_enable_io_display() {
    PORTB.OUT = IO_ADDR_DISPLAY;
}

INLINE void spi_enable_io_flash() {
    PORTB.OUT = IO_ADDR_FLASH;
}

INLINE void spi_enable_io_expander() {
    PORTB.OUT = IO_ADDR_EXPANDER;
}


/* Sending a single byte through HW SPI.
   Expecting that SPI already enabled in non-buffered mode and chip select done */
void spi_send_single_byte(uint8_t data);


/* Sending multiple bytes through HW SPI in buffered mode.
   Expecting that SPI already enabled and chip select done */
void spi_send_data(uint8_t *buffer, uint16_t length);



#endif

