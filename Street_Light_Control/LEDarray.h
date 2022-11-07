#ifndef _LEDarray_H
#define _LEDarray_H

#include <xc.h>

#define _XTAL_FREQ 64000000

extern char day;

//function prototypes
void RH3led_init(void);
void LEDarray_init(void);
void LEDarray_disp_bin(unsigned int number);
void LEDarray_streetlight(char *hours, char *noonCheck);

#endif
