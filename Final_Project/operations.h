#ifndef _OPERATIONS_H
#define _OPERATIONS_H

#include <xc.h>
#include "operations.h"
#include "dc_motor.h"
#include "Movement.h"
#include "LEDs.h"
#include "color.h"
#include "card.h"
#include "interrupts.h"
#include "memory.h"
#include "ADC.h"

#define _XTAL_FREQ 64000000
#define initialTurningCircle 30 //Approximate value needed to turn the car 45 degrees (from trial and error)
#define maxTime 65535


void operations_motorSetup(struct DC_motor *mL, struct DC_motor *mR);

void operations_calibrationRoutine(struct DC_motor *mL, struct DC_motor *mR,
        char *mode, unsigned int *high_interrupt_val, unsigned int *low_interrupt_val);

void operations_executeCard(char card, struct DC_motor *mL, struct DC_motor *mR, 
        unsigned int low_interrupt_val, unsigned int high_interrupt_val, char *forward);

void operations_reverseMovementSetup(struct DC_motor *mL, struct DC_motor *mR, 
        unsigned int curr_time, char *forward, int *stage_count );

void operations_reverseMovementExecute(struct DC_motor *mL, struct DC_motor *mR,
        char curr_card, char curr_time, int *stage_count);

void operations_specialReverseMovementSetup(struct DC_motor *mL, struct DC_motor *mR,
        struct memory *m, int stage_count, char *forward);

void operations_end(struct DC_motor *mL, struct DC_motor *mR);

void operations_batteryCheck(struct DC_motor *mL, struct DC_motor *mR);

#endif
