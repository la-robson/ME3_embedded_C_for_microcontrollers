#ifndef _LEDs_H
#define _LEDs_H

#include <xc.h>

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

#define HLAMP LATHbits.LATH1
#define MBEAM LATDbits.LATD3
#define BRAKE LATDbits.LATD4
#define TURNR LATHbits.LATH0
#define TURNL LATFbits.LATF0


void LEDs_init(void);
void LEDs_flash(void);
void LEDs_hazards(void);
char LEDs_holdOrPress(void);


#endif