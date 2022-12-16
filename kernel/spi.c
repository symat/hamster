#include "spi.h"

	
/* initializing SPI (still disabled) in non-buffered mode, with CLK/4 */
void spi_init() {
 //   SPI0.CTRLA = 0x20;   // SPI0.CTRLA { DORD=MSb, MASTER=Master, CLK2X=No, PRESC=DIV4, ENABLED=No }
 //   SPI0.CTRLB = 0x00;   // SPI0.CTRLB { BUFEN=No, BUFWR=0, SSD=Enable, MODE=0 }

SPI0.CTRLA |= SPI_MASTER_bm;						// Select master mode
	SPI0.CTRLA |= SPI_PRESC_DIV16_gc;				// Select Prescaler of 16
	SPI0.CTRLA |= SPI_CLK2X_bm;						// Enable double clock speed
	SPI0.CTRLB |= SPI_SSD_bm;						// Disable slave select
//	SPI0.CTRLB |= SPI_MODE_1_gc;					// Select SPI Mode 1
    SPI0.CTRLA |= SPI_ENABLE_bm;					// Enable SPI

}




/* Sending a single byte through HW SPI.
   Expecting that SPI already enabled in non-buffered mode and chip select done */
void spi_send_single_byte(uint8_t data) {
	SPI0.DATA = data;
	while(!(SPI0.INTFLAGS & SPI_IF_bm)) {}		//Wait for transfer
}	


/* Sending multiple bytes through HW SPI in buffered mode.
   Expecting that SPI already enabled and chip select done */
void spi_send_data(uint8_t *buffer, uint16_t length) {	
    SPI0.CTRLB |=  SPI_BUFEN_bm;  // enable buffered mode
    for (uint16_t i = 0 ; i < length; i++) {					
        SPI0.DATA = buffer[i];
        while(!(SPI0.INTFLAGS & SPI_DREIF_bm)) {} // wait until buffer gets empty
    }
    while(!(SPI0.INTFLAGS & SPI_TXCIF_bm)) {} // wait until all data transfer finished
    SPI0.INTFLAGS |= SPI0.INTFLAGS & SPI_TXCIF_bm; // clear transmit flag
    SPI0.CTRLB &= ~ SPI_BUFEN_bm; // disable buffered mode
}
