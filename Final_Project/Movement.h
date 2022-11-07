#ifndef _Movement_H
#define _Movement_H

#include <xc.h>
#include "dc_motor.h"
#include "memory.h"
#include "timers.h"

#define _XTAL_FREQ 64000000

extern volatile char color_interrupt;

void Movement_stop_and_time(struct DC_motor *mL, struct DC_motor *mR, char stage_count, struct memory *m);
void Movement_start_and_time(struct DC_motor *mL, struct DC_motor *mR, unsigned int lowT, unsigned int highT);
void Movement_reverseSetup(struct DC_motor *mL, struct DC_motor *mR, unsigned int time);
void Movement_back_up(struct DC_motor *mL, struct DC_motor *mR);
void Movement_WhichMovement(char colour, struct DC_motor *mL, struct DC_motor *mR, char forward);
void Movement_RedMovement(char forward, struct DC_motor *mL, struct DC_motor *mR);
void Movement_GreenMovement(char forward, struct DC_motor *mL, struct DC_motor *mR);
void Movement_BlueMovement(struct DC_motor *mL, struct DC_motor *mR);
void Movement_YellowMovement(char forward, struct DC_motor *mL, struct DC_motor *mR);
void Movement_PinkMovement(char forward, struct DC_motor *mL, struct DC_motor *mR);
void Movement_OrangeMovement(char forward, struct DC_motor *mL, struct DC_motor *mR);
void Movement_LightBlueMovement(char forward, struct DC_motor *mL, struct DC_motor *mR);


#endif
