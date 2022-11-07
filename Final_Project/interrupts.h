#ifndef _interrupts_H
#define _interrupts_H

#include <xc.h>

#define _XTAL_FREQ 64000000

//Interrupt variable
extern volatile char color_interrupt;
extern volatile char timer_interrupt;

void Interrupts_init(void);
unsigned int interrupt_set(void);
void I2C_interrupt_init(unsigned int lowT, unsigned int highT);
void I2C_interrupt_clear(void);
void interrupt_calibration(unsigned int *high_interrupt_val, unsigned int *low_interrupt_val);
void timer0_interrupt(void);
void __interrupt(high_priority) HighISR();

#endif
