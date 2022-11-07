#ifndef _TIMERS_H
#define _TIMERS_H

#include <xc.h>

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

void timer_initMemory(void);
void timer_calculator(unsigned int time);

#endif