#include "kernel/kernel.h"

#include <util/delay.h>



// TODO: change to single, 1024 buffer, when we will have attiny1624
uint8_t frame_buffer[512]; // cheating... on ATTiny824 we have only 1KB RAM


void set_binary_number(uint16_t number) {
    uint16_t x = 0;
    // page 1: white
    for(uint8_t seg = 0; seg<16; seg++) {
        for(uint8_t col = 0; col<8; col++) {
            frame_buffer[x++] = 0xff;
        }
    }
    // page 2, 3: bar code
    for(uint8_t page = 0; page<2; page++) {
        for(uint8_t seg = 0; seg<16; seg++) {
            uint16_t color = (number << seg) & 0x8000;
            frame_buffer[x++] = color ?  0x00 : 0xff;
            for(uint8_t col = 0; col<6; col++) {
                frame_buffer[x++] = color ?  0xff : 0x00;
            }
            frame_buffer[x++] = color ?  0x00 : 0xff;
        }
    }
    // page 4: white
    for(uint8_t seg = 0; seg<16; seg++) {
        for(uint8_t col = 0; col<8; col++) {
            frame_buffer[x++] = 0xff;
        }
    }
     


}

int main(void) { 
    init_kernel();
 //   blink();
    spi_init();
  //  blink();
    video_init();
 //   blink();
    timer_init();
 //   blink();
    sei();	//Enable interrupts (needed e.g. for the timer)


    uint32_t aaa = 0;

    while (1) {
        blink();

        //aaa++;

        aaa = (uint16_t) timer_millis();
        //set_binary_number( (uint16_t) aaa);


        video_draw_start();
        //video_draw_full_screen(frame_buffer);
       // video_invert_display_on_off(1);
       
       //video_binary_barcode(0x00, aaa);
       //video_binary_barcode(0x02, aaa);
       video_binary_barcode(0x04, aaa);
       //video_binary_barcode(0x06, aaa);

        _delay_ms(500);
       //  video_invert_display_on_off(0);
        video_draw_end();
  }
 
    return(0); 
}
