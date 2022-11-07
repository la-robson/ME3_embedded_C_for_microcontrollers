#ifndef _ColourLED_H
#define _ColourLED_H

#include <xc.h>

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

//Output for pins
#define LEDBlue LATAbits.LATA3
#define LEDGreen LATEbits.LATE7
#define LEDRed LATGbits.LATG0

void ColourLED_init(void);
void ColourLED_off(void);
void ColourLED_set(char c);

#endif