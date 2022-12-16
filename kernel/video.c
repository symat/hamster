#include "kernel.h"

#include <util/delay.h>

#define PORTA_DC_MASK PIN7_bm          // INPUT_SW_B line is used, PA7
#define PORTA_RESET_MASK PIN5_bm       // temporarily using battery-level pin as reset

uint8_t display_sleep = 0;
uint8_t current_page_start = 0;
uint8_t current_page_end = 0;
uint8_t current_column_start = 0;
uint8_t current_column_end = 0;


/* needs to be called before any video function below.
   One can not use other SPI IO target, nor can read button values after this. */
void video_draw_start() {
	spi_all_io_disable();
    PORTA.DIRSET = PORTA_DC_MASK;        // DC PIN is set to output
    spi_enable_io_display();
}

/* needs to be called after any video function below.
   Only after calling this function can one use 
   other SPI IO target, nor can read button values. */
void video_draw_end() {
	spi_all_io_disable();
    PORTA.DIRCLR = PORTA_DC_MASK;       // DC PIN is set to input (default)
}


void dc_to_data() {
	PORTA.OUTSET = PORTA_DC_MASK;
}

void dc_to_command() {
	PORTA.OUTCLR = PORTA_DC_MASK;
}



/* initialize the display before first use */
void video_init() {

    // reset the display
	PORTA.OUTCLR = PORTA_RESET_MASK;
  _delay_ms(100);
	PORTA.OUTSET = PORTA_RESET_MASK;


    video_draw_start();

   // Init sequence for 128x64 OLED module

    dc_to_command();

    spi_send_single_byte(SSD1306_DISPLAYOFF); // Set display OFF		
 
    spi_send_single_byte(SSD1306_SETDISPLAYCLOCKDIV); // Set Display Clock Divide Ratio / OSC Frequency
    spi_send_single_byte(0x80); // Display Clock Divide Ratio / OSC Frequency 
                                // the suggested ratio 0x80 (this is the default)
 
    spi_send_single_byte(SSD1306_SETMULTIPLEX); // Set Multiplex Ratio
    spi_send_single_byte(0x3F); // Multiplex Ratio for 128x64 (64-1)
                                // 0b00111111 = 0x3F is the default
 
    spi_send_single_byte(SSD1306_SETDISPLAYOFFSET); // Set Display Offset
    spi_send_single_byte(0x00); // Display Offset (no vertical offset)
 
    spi_send_single_byte(SSD1306_SETSTARTLINE); // Set Display Start Line
                                                // position the start line to line #0 (40~7F)
 

     spi_send_single_byte(SSD1306_MEMORYMODE);                    // 0x20
    //spi_send_single_byte(0x01);								 // 0x00 vertical addressing
    spi_send_single_byte(0x00);									 // 0x00 horizontal addressing

    spi_send_single_byte(SSD1306_CHARGEPUMP); // Set Charge Pump
    spi_send_single_byte(0x14); // Charge Pump (0x10 External, 0x14 Internal DC/DC)
 
    // normal, no horizontal flip: (default) 
    // from right to left
    // spi_send_single_byte(SSD1306_SEGREMAP);				
    // spi_send_single_byte(SSD1306_COMSCANINC);			

    // for horizontal flip: 
    // from left to right
    spi_send_single_byte(SSD1306_SEGREMAP | 0x01);		
    spi_send_single_byte(SSD1306_COMSCANDEC);	

 
    spi_send_single_byte(SSD1306_SETCOMPINS);  // Set COM Hardware Configuration
    spi_send_single_byte(0x12);                // default is 0x12: disable COM Left/Right remap
                                               // Alternative COM pin configuration

    spi_send_single_byte(SSD1306_SETCONTRAST);  // Set Contrast
    spi_send_single_byte(0x9F);                 // 00-FF, default is 7F
    
    spi_send_single_byte(SSD1306_SETPRECHARGE); // Set Pre-Charge Period - how much DCLK is considered invalid in phase 1 and 2
    spi_send_single_byte(0x22);                 // 0x22: (default) External, 0xF1: Internal
    
    spi_send_single_byte(SSD1306_SETVCOMDETECT);  // V COMH deselect level
    spi_send_single_byte(0x20);                   // 0x00 = 0.65 VCC, 0x20 = 0.77 VCC (default), 0x30 = 0.83 VCC
 
    spi_send_single_byte(SSD1306_DISPLAYALLON_RESUME); // Set all pixels OFF

    spi_send_single_byte(SSD1306_NORMALDISPLAY); // Set display not inverted
                                                 // for inverted, use: SSD1306_INVERTDISPLAY

    spi_send_single_byte(SSD1306_DISPLAYON); // Set display On


    video_set_canvas(0x00, 0x07, 0x00, 0x7F);
    video_fill_screen(0x00);

    video_draw_end();
}


/*
Configuring the visible part of the display, where pixel data will be written.
Pages: 0..7, Columns: 0..127
The video ram write pointer jumps to the start column of the start page. 
Expecting that video_draw_start was called
*/
void video_set_canvas(uint8_t page_start, uint8_t page_end, uint8_t column_start, uint8_t column_end) {

    current_page_start = page_start;
    current_page_end = page_end;
    current_column_start = column_start;
    current_column_end = column_end;
 
    dc_to_command();
 
    spi_send_single_byte(SSD1306_PAGEADDR);
    spi_send_single_byte(page_start);   // page start addr (0)
    spi_send_single_byte(page_end);   // page end addr (7)

    spi_send_single_byte(SSD1306_COLUMNADDR);
    spi_send_single_byte(column_start);   // column start addr (0)
    spi_send_single_byte(column_end);   // column end addr (127)


    spi_send_single_byte(SSD1306_DISPLAYON);
}


/* Drawing the entire canvas, based on a full frame buffer 
  Expecting that video_draw_start was called */
void video_draw_full_canvas(uint8_t* buffer) {
    dc_to_data();

    uint16_t buffer_size = (current_page_end - current_page_start + 1) * (current_column_end - current_column_start + 1);

    // TODO: change to single operation, when we will have attiny1624
    //spi_send_data(buffer, buffer_size); 

    spi_send_data(buffer, buffer_size / 2);
    spi_send_data(buffer, buffer_size / 2);
}


void video_invert_display_on_off(uint8_t is_inverted) {
    dc_to_command();
    spi_send_single_byte(is_inverted ? SSD1306_INVERTDISPLAY : SSD1306_NORMALDISPLAY);
}

/* turning the display on or off. 
  Expecting that video_draw_start was called */
void video_display_on_off(uint8_t is_on) {
	if (is_on && display_sleep) {
		video_init();
		display_sleep = 0;									//Reset OLED to wake it back up
	} 
    
    if(!is_on && !display_sleep) {
        dc_to_command();
		spi_send_single_byte(SSD1306_DISPLAYOFF);           //Sleep the OLED
		display_sleep = 1;
	}
}


/*
This is a very lightweight way of printing out numbers for debugging.
You can print a barcode into each page (0..8), showing a uint16 value,
represented by black (0) and white (1) boxes, Most significant bit on the left.
Expecting that video_draw_start was called  */
void video_binary_barcode(uint8_t page, uint16_t number) {
    
  uint8_t tmp_page_start = current_page_start;
  uint8_t tmp_page_end = current_page_end;
  uint8_t tmp_column_start = current_column_start;
  uint8_t tmp_column_end = current_column_end;
  video_set_canvas(page, page, 0x00, 0x7F);

  dc_to_data();
  for(uint8_t seg = 0; seg<16; seg++) {
    uint16_t color = (number << seg) & 0x8000;
    spi_send_single_byte(color ?  0x00 : 0xff);
    for(uint8_t col = 0; col<6; col++) {
      spi_send_single_byte(color ?  0xff : 0x00);
    }
    spi_send_single_byte(color ?  0x00 : 0xff);
  }
  video_set_canvas(tmp_page_start, tmp_page_end, tmp_column_start, tmp_column_end);
}


/*
Fill the current canvas with black (0x00) or white (0xFF) color.
Expecting that video_draw_start was called  */
void video_fill_screen(uint8_t color) {
  dc_to_data();
  for(uint8_t page = current_page_start; page<=current_page_end; page++) {
     for(uint8_t col = current_column_start; col<=current_column_end; col++) {
      spi_send_single_byte(color);
    }
  }
}



