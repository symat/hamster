#ifndef __HAMSTER_TIMER
#define __HAMSTER_TIMER

#include "kernel.h"

void timer_init();

uint32_t timer_millis();
uint32_t timer_micros();

#endif /* __HAMSTER_TIMER */