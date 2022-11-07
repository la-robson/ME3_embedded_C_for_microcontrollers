// CONFIG1L
#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)

// CONFIG3L
#pragma config WDTCPS = WDTCPS_31// WDT Period Select bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)

#include <xc.h>
#include "color.h"
#include "i2c.h"
#include "LEDs.h"
#include "dc_motor.h"
#include "ColourLED.h"
#include "Movement.h"
#include "ADC.h"
#include "interrupts.h"
#include "memory.h"
#include "timers.h"
#include "card.h"
#include "operations.h"

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz

//Interrupt variables
volatile char color_interrupt=1;
volatile char timer_interrupt=0;

// function main
void main(void)
{
    // set necessary variables 
    char card;                         // store card identity
    signed int stage_count = 0;        // track the number of stages 
    char forward = 1;                  // sets car state 'forward' or 'reverse'
    int PWMperiod = 199;               // set motor PWM period
    unsigned int high_interrupt_val;   // stores calibrated interrupt tolerance
    unsigned int low_interrupt_val;    // stores calibrated interrupt tolerances
    char mode;                         // stores mine difficulty mode
    
    // Initialise functions
    color_click_init();
    Interrupts_init();
    LEDs_init();
    dc_motor_init(PWMperiod);
    ColourLED_init();
    ADC_init();
    timer_initMemory();
    
    // declare necessary structures
    struct colour myColour;                 // declare colour structure
    struct DC_motor motorL, motorR; 		//declare two DC_motor structures
    struct memory myMemory;                 //declare time and card memory
    
    operations_motorSetup(&motorL,&motorR); // set initial values for motor

    // run calibration sequence
    operations_calibrationRoutine(&motorL,&motorR, &mode, &high_interrupt_val, &low_interrupt_val);
    
    // Starting movement 
    Movement_start_and_time(&motorL, &motorR,low_interrupt_val,high_interrupt_val);
    
    // main driving functionality 
    while(1)
    {
        if (color_interrupt==1) // if there is a proximity alert:
        {
            // stop car and record driving time in memory 
            Movement_stop_and_time(&motorL, &motorR, stage_count, &myMemory);
            
            // get card reading and assign to memory
            card = card_routine(&myColour, mode, &motorL, &motorR, stage_count, &myMemory);
            
            // execute appropriate movement 
            operations_executeCard(card, &motorL, &motorR,low_interrupt_val, high_interrupt_val, &forward);
            
            // iterate the stage count
            stage_count += 1;       
        } 
        
        //Initialising the 'reverse' movement
        if (forward==0) 
        {operations_reverseMovementSetup(&motorL, &motorR, myMemory.time[stage_count], &forward, &stage_count);}
        
        // execute reverse movement when timer interrupts 
        if ( timer_interrupt & (stage_count >= 0))
        {operations_reverseMovementExecute(&motorL, &motorR, myMemory.cards[stage_count], myMemory.time[stage_count], &stage_count);} 
        
        // if driving for 30s and fail to find card return to start 
        else if ( timer_interrupt & (forward == 1) )
        {operations_specialReverseMovementSetup(&motorL, &motorR, &myMemory, stage_count, &forward);}
        
        // if back to start stop and flash LED forever
        else if ((stage_count < 0) & timer_interrupt)
        {operations_end(&motorL, &motorR);}
        
        // check battery is above 20%, if below stop and flash LED forever
        operations_batteryCheck(&motorL, &motorR); 
    }       
}