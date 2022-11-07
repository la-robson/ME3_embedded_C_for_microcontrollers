#include <xc.h>
#include <stdlib.h>
#include "timers.h"

/************************************
 * Init timer for memory
************************************/
void timer_initMemory(void)
{
    T0CON1bits.T0CS=0b010; // Fosc/4
    T0CON1bits.T0ASYNC=1; // see datasheet errata - needed to ensure correct operation when Fosc/4 used as clock source
    T0CON1bits.T0CKPS=0b1101; // Resets 16 bits every 33.55 seconds, 8192 pre scaler
    T0CON0bits.T016BIT=1;	//16 bit mode	

    TMR0H=0b0;           //write High reg first (only for the 16 bits), update happens when low reg is written to
    TMR0L=0b0;           //Lower bits (for both 16 and 8 bits timer)
    T0CON0bits.T0EN=0;	//don't start the timer
}

/************************************
 *Interrupt occurs when overflow occur therefore need to subtract time taken from (2^16-1) 
 * and set that value to the timer 
************************************/
void timer_calculator(unsigned int time)
{
    unsigned int start_time;
    start_time = 65535 - time;
    TMR0H = (start_time >> 8);             //Removes lower 8 bits 
    TMR0L = (start_time & 0b0000000011111111);     //Removes top 8 bits     
}