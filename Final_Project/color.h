#ifndef _color_H
#define _color_H

#include <xc.h>
#include "dc_motor.h"
#include "memory.h"

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  
#define tol 10          // tolerance of colour sensors

// definition of colour structure
// array format contains readings when the following LEDs are on
// [redLED, greenLED, blueLED, clearLED]
struct colour { 
    unsigned int r[4];
    unsigned int g[4];
    unsigned int b[4];
    unsigned int c[4];
};

void color_click_init(void);
void color_writetoaddr(char address, char value);
unsigned int color_read_Red(void);
unsigned int color_read_Green(void);
unsigned int color_read_Blue(void);
unsigned int color_read_Clear(void);
void color_cal_init(void);
void color_cal(struct colour *c);
void color_read(struct colour *c);



#endif