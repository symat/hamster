/*
   Hamster kernel functions:

   A single header to include in order to reach all the public
   kernel functions.
*/

#ifndef __CRICOM_KERNEL
#define __CRICOM_KERNEL


#define INLINE static __attribute__((always_inline)) inline


// Default clock: CLR_PER = CLK_MAIN/Prescaler = 20MHz / 2 = 10MHz
#define F_CPU (10000000UL)

#define __CRICOM_KERNEL_VERSION 1


#include <avr/io.h>
#include <avr/interrupt.h>


#include "usb.h"
#include "power.h"
#include "speaker.h"
#include "buttons.h"
#include "spi.h"
#include "video.h"
#include "flash.h"
#include "expander.h"
#include "timer.h"


void blink();

void init_kernel();


#endif