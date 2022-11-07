#ifndef _CARD_H
#define _CARD_H

#include <xc.h>
#include "color.h"
#include "memory.h"

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  
#define tol 10          // tolerance of colour sensors


// declare colour structure for each card (for calibration and identification)
struct colour black, white; 
// numbers are average normalised rgbc readings taken in tutorial room 749
struct colour red = {{103,117,94,100},{91,99,68,64},{94,88,148,91},{100,100,100,100}};
struct colour green = {{98,80,52,68},{109,100,121,135},{94,103,100,108},{100,100,100,100}};
struct colour blue = {{58,127,101,45},{340,100,100,135},{231,100,96,157},{100,100,100,100}};
struct colour yellow = {{101,96,82,742},{93,101,93,642},{96,95,109,558},{100,100,100,100}};
struct colour pink = {{92,167,106,990},{125,71,91,587},{112,91,72,696},{100,100,100,100}};
struct colour orange = {{101,156,143,299},{76,85,71,159},{93,83,83,190},{100,100,100,100}};
struct colour lblue = {{93,102,93,183},{114,98,109,220},{108,105,100,221},{100,100,100,100}};



char card_mode(void);
void card_normalise(struct colour *col);
char card_identify(struct colour *c, char mode);
char card_routine(struct colour *col, char mode, struct DC_motor *mL, struct DC_motor *mR, char stage_count, struct memory *m);


#endif