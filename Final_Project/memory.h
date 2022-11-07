#ifndef _MEMORY_H
#define _MEMORY_H

#include <xc.h>

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

// create structure to store route 
struct memory {
    unsigned int time[20];
    unsigned int cards[20];
};

void memory_card(struct memory *m, char card, char stage_count);
void memory_time(struct memory *m, unsigned int curr_time, char stage_count);

#endif