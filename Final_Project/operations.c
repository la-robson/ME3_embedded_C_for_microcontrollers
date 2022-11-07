#include <xc.h>
#include "operations.h"
#include "dc_motor.h"
#include "memory.h"

/************************************
 * Function to set all necessary initial motor values
************************************/
void operations_motorSetup(struct DC_motor *mL, struct DC_motor *mR)
{
    int PWMperiod = 199;
    // left motor on RE2 (PWM A) and RE4 (PWM B)
    mL->power=0; 						//zero power to start
    mL->direction=1; 					//set default motor direction, forward
    mL->dutyHighByte=(unsigned char *)(&PWM6DCH);	//store address of PWM duty high byte
    mL->dir_LAT=(unsigned char *)(&LATE); 		    //store address of LAT LATEbits.LATE2
    mL->dir_pin=4; 						//pin RE4 controls direction
    mL->PWMperiod=PWMperiod; 			//store PWMperiod for motor
    mL->turningCircle=initialTurningCircle;         //Approximate value needed to turn the car 45 degrees (from trial and error)
    
    // right motor on RC7 (PWM C) and RG6 (PWM D)
    mR->power=0; 						//zero power to start
    mR->direction=1; 					//set default motor , forward
    mR->dutyHighByte=(unsigned char *)(&PWM7DCH);	//store address of PWM duty high byte
    mR->dir_LAT=(unsigned char *)(&LATG); 		    //store address of LAT LATCbits.LATC7
    mR->dir_pin=6; 						//pin RG6 controls direction
    mR->PWMperiod=PWMperiod; 			//store PWMperiod for motor
    mR->turningCircle=initialTurningCircle;         //Approximate value needed to turn the car 45 degrees (from trial and error)
    
    // set  motor PWM
    dc_motor_setPWM(mL);
    dc_motor_setPWM(mR);
};

/************************************
 * Function to run all calibration routines
************************************/
void operations_calibrationRoutine(struct DC_motor *mL, struct DC_motor *mR,
        char *mode, unsigned int *high_interrupt_val, unsigned int *low_interrupt_val)
{
    dc_motor_movemoventCalibration(mL, mR); //Movement Calibration Routine
    color_cal_init();                       //Colour Calibration Routine
    *mode = card_mode();                    //Select easy/hard mode 
    interrupt_calibration(*high_interrupt_val, *low_interrupt_val);  // set high and low interrupt tolerance
};

/************************************
 * Function to execute appropriate movement in response to card
************************************/
void operations_executeCard(char card, struct DC_motor *mL, struct DC_motor *mR, 
        unsigned int low_interrupt_val, unsigned int high_interrupt_val, char *forward)
{ 
    // check for failed reading, white or black card
    if (card==0 | card>=8 )
    {
        LEDs_flash();           // alert that car is going back to start
        *forward = 0;           // set to reverse mode
        color_interrupt = 0;    // leave the while loop
    }
    // success! execute card instruction, increase count and restart car
    else 
    {
        char tmpFwd = 1;                // temp to set forward
        Movement_back_up(mL, mR);       // back up from card
        Movement_WhichMovement(card, mL, mR, tmpFwd);       // execute movement
        // restart car, timer and interrupt
        Movement_start_and_time(mL, mR, low_interrupt_val,high_interrupt_val);
    }
};

/************************************
 * Function to set up reverse movement interrupts and execute the first step
************************************/
void operations_reverseMovementSetup(struct DC_motor *mL, struct DC_motor *mR, 
        unsigned int curr_time, char *forward, int *stage_count )
{
    *forward = 1; //only want to go in to this routine once, can 'pretend' to be in forward mode once this if is entered      
    *stage_count -= 1;                            // increment stage_count
    timer0_interrupt();                           // Initialising timer interrupt
    Movement_back_up(mL, mR);                     // back away from wall
    Movement_BlueMovement(mL, mR);                // Turn around      
    Movement_reverseSetup(mL, mR, curr_time);     // Does the first movement  
};

/************************************
 * Function to execute reverse movement and reset interrupts
************************************/
void operations_reverseMovementExecute(struct DC_motor *mL, struct DC_motor *mR,
        char curr_card, char curr_time, int *stage_count)
{
    *stage_count -= 1;                             // increment stage count
    dc_motor_stop(mL, mR);                         // Stopping car 
    timer_interrupt = 0;                           // reset timer interrupt
    Movement_WhichMovement(curr_card, mL, mR, 0);  // Movement in reverse            
    Movement_reverseSetup( mL, mR, curr_time);     // Starts car in straight line and times it  
       
}

/************************************
 * Function to execute reverse movement and reset interrupts
 * for the special case that the car drives for 30s without finding a card
************************************/
void operations_specialReverseMovementSetup(struct DC_motor *mL, struct DC_motor *mR,
        struct memory *m, int stage_count, char *forward)
{
    memory_card(m, 0, stage_count);         // read failed card into memory
    memory_time(m, maxTime, stage_count);   // store drive time in memory 
    dc_motor_stop(mL, mR);                  // stop car
    LEDs_flash();                           // alert that car is going back to start
    *forward = 0;                           // set to reverse mode
}

/************************************
 * Function to stop the car and flash the celebration lights 
 * when it gets back to start
************************************/
void operations_end(struct DC_motor *mL, struct DC_motor *mR)
{
    dc_motor_stop(mL, mR);          // stop car
    T0CON0bits.T0EN=0;              // stop timer
    while (1) {LEDs_flash();}       // flash celebration lights forever
}

/************************************
 * Function to stop the car and flash the hazards 
 * if battery is below 20%
************************************/
void operations_batteryCheck(struct DC_motor *mL, struct DC_motor *mR)
{
    if (ADC_getval()) //if less than 20 % hazard lights
    {
        T0CON0bits.T0EN=0;              // turn off timer
        dc_motor_stop(mL, mR);          // stop car
        while (1) {LEDs_hazards();}     // flash hazards
    }
}