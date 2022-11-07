#ifndef _interrupts_H
#define _interrupts_H

#include <xc.h>

#define _XTAL_FREQ 64000000

// incude necessary global variables
extern char day;      // create day/night variable (starts at midnight)
extern int time;      // create time variable to count 2 minute increments
extern int dawn;      // variable to record dawn time
extern int dusk;      // variable to record dusk time

void Interrupts_init(void);
void __interrupt(high_priority) HighISR();

#endif
