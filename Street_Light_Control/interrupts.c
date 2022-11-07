#include <xc.h>
#include <stdio.h>
#include "interrupts.h"


/************************************
 * Function to turn on interrupts and set if priority is used
 * Note you also need to enable peripheral interrupts in the INTCON register to use CM1IE.
************************************/
void Interrupts_init(void)
{    
    INTCONbits.IPEN=1;  // define priority level
    
    // set priority level for comaprator and timer
    IPR2bits.C1IP=1;     
    IPR0bits.TMR0IP=1;
    
    // clear flag for comparator and timer 
    PIR2bits.C1IF=0;
    PIR0bits.TMR0IF=0;
    
    INTCONbits.GIE=1; 	// turn on high priority interrupts globally
    INTCONbits.GIEL=1;  // turn on low priority interrupts globally
    
    //enable interrupt source C1IE
    PIE2bits.C1IE=1;
    PIE0bits.TMR0IE=1;
}

/************************************
 * High priority interrupt service routine
 * Make sure all enabled interrupts are checked and flags cleared
************************************/
void __interrupt(high_priority) HighISR()
{
	if(PIR2bits.C1IF){                          // check the comparator interrupt flag
        day = !day;                             // toggle the day/night status
        if (day==1) {dawn=time;} else {dusk=time;} // record dawn/dusk 
        PIR2bits.C1IF=0; 						// clear the interrupt flag
	}
    
    if(PIR0bits.TMR0IF){                        // check the timer interrupt flag
        time += 1;                              // increment the time counter
        // initialise registers for 1 second cycles 
        TMR0H=0b00001011;                       //write High reg first, update happens when low reg is written to
        TMR0L=0b10111010;
        PIR0bits.TMR0IF=0; 						// clear the interrupt flag
    }

}