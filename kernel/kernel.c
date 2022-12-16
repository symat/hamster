/* This is the main entry point of the kernel, this is where the main function can be found. */

#include "kernel.h"

#include <util/delay.h>


// factory default FUSES, read from a new chip, code generated using MPLAB X IDE 
FUSES = {
 .WDTCFG = 0x00, // WDTCFG {PERIOD=OFF, WINDOW=OFF}
 .BODCFG = 0x00, // BODCFG {SLEEP=DIS, ACTIVE=DIS, SAMPFREQ=1KHZ, LVL=BODLEVEL0}
 .OSCCFG = 0x02, // OSCCFG {FREQSEL=20MHZ, OSC_LOCK=CLEAR}
 .SYSCFG0 = 0xD4, // SYSCFG0 {EESAVE=EEPROM_ERASED_DURING_CHIP_ERASE, RSTPINCFG=UPDI, TOUTDIS=NVM_WRITE_BLOCK_DISABLED, CRCSRC=NOCRC}
 .SYSCFG1 = 0x07, // SYSCFG1 {SUT=64MS}
 .APPEND = 0x00, // APPEND {APPEND=User range:  0x0 - 0xFF}
 .BOOTEND = 0x00, // BOOTEND {BOOTEND=User range:  0x0 - 0xFF}
};
LOCKBITS = 0xC5; // {LB=NOLOCK}


INLINE void init_clock() {
    // set 10 MHz clock
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, 0x00);  // MCLKCTRLA {CLKOUT=0, CLKSEL=OSC20M}
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 0x01);  // MCLKCTRLB {PDIV=2, PEN=1}

    // wait until internal oscillator is stable
    while(!(CLKCTRL.MCLKSTATUS & 0x10)) {}
}

void init_ports() {
    spi_all_io_disable();
    PORTB.DIR = 0x03; // set PB0 and PB1 to be output (SPI IO address lines)
 //   PORTA.DIR = PIN1_bm | PIN3_bm;  // SPI_MOSI, SPI_SCK set to output
    PORTA.DIR = PIN1_bm | PIN3_bm  | PIN5_bm ;  // for now, also add PA5 (battery-level) for using as display reset
}

void blink() {
    
    spi_all_io_disable();
    _delay_ms(200);

    spi_enable_io_flash();
    _delay_ms(200);

}


void init_kernel() {
    init_clock();
    init_ports();
}