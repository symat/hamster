/*
   Video kernel module: 

   Basic low-level functions to refresh the screen, and/or send the data 
   through USB to an external device.

   display: 128x64

   We are using an SSD1306 LCD driver chip in 4-wire SPI mode. As we don't have
   separate leg for DC, we can not use HW mode ?
   or we can actually use an SW_A/B line as output?

   the T_cycle_min is 300ns, meaning we can go below 3.33MHz with the clock. 
   We need pre-scaler=4 and can not enable the double speed mode for HW SPI. 


*/

#ifndef __HAMSTER_KERNEL_VIDEO
#define __HAMSTER_KERNEL_VIDEO

#include "kernel.h"



/* initialize the display before first use */
void video_init();


/* needs to be called before any video function below.
   One can not use other SPI IO target, nor can read button values after this. */
void video_draw_start();

/* needs to be called after any video function below.
   Only after calling this function can one use 
   other SPI IO target, nor can read button values. */
void video_draw_end();


/* drawing a screen based on a full frame buffer 
  Expecting that video_draw_start was called */
void video_draw_full_screen(uint8_t* buffer);


/* turning the display on or off. 
  Expecting that video_draw_start was called */
void video_display_on_off(uint8_t is_on);

/* enabling or disabling the invertion of the colors on the screen 
  Expecting that video_draw_start was called */
void video_invert_display_on_off(uint8_t is_inverted);


/*
Configuring the visible part of the display, where pixel data will be written.
Pages: 0..7, Columns: 0..127
Expecting that video_draw_start was called
*/
void video_set_canvas(uint8_t page_start, uint8_t page_end, uint8_t column_start, uint8_t column_end);

/*
This is a very lightweight way of printing out numbers for debugging.
You can print a barcode into each page (0..8), showing a uint16 value,
represented by black (0) and white (1) boxes, Most significant bit on the left.
Expecting that video_draw_start was called  */
void video_binary_barcode(uint8_t page, uint16_t number);

/*
Fill the current canvas with black (0x00) or white (0xFF) color.
Expecting that video_draw_start was called  */
void video_fill_screen(uint8_t color);

void dc_to_data();
void dc_to_command();

#define SSD1306_LCDWIDTH				128
#define SSD1306_LCDHEIGHT				64
#define SSD1306_SETCONTRAST				0x81
#define SSD1306_DISPLAYALLON_RESUME		0xA4
#define SSD1306_DISPLAYALLON			0xA5
#define SSD1306_NORMALDISPLAY			0xA6
#define SSD1306_INVERTDISPLAY			0xA7
#define SSD1306_DISPLAYOFF				0xAE
#define SSD1306_DISPLAYON				0xAF
#define SSD1306_SETDISPLAYOFFSET		0xD3
#define SSD1306_SETCOMPINS				0xDA
#define SSD1306_SETVCOMDETECT			0xDB
#define SSD1306_SETDISPLAYCLOCKDIV		0xD5
#define SSD1306_SETPRECHARGE			0xD9
#define SSD1306_SETMULTIPLEX			0xA8
#define SSD1306_SETLOWCOLUMN			0x00
#define SSD1306_SETHIGHCOLUMN			0x10
#define SSD1306_SETSTARTLINE			0x40
#define SSD1306_MEMORYMODE				0x20
#define SSD1306_COLUMNADDR				0x21
#define SSD1306_PAGEADDR				0x22
#define SSD1306_COMSCANINC				0xC0
#define SSD1306_COMSCANDEC				0xC8
#define SSD1306_SEGREMAP				0xA0
#define SSD1306_CHARGEPUMP				0x8D
#define SSD1306_EXTERNALVCC				0x1
#define SSD1306_SWITCHCAPVCC			0x2


#endif