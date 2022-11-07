#include <xc.h>
#include "Movement.h"
#include "interrupts.h"
#include "ColourLED.h"
#include "timers.h"
#include "memory.h"

/************************************
 *Function stops the car, and writes to memory the time, and stops the timer
************************************/
void Movement_stop_and_time(struct DC_motor *mL, struct DC_motor *mR, char stage_count, struct memory *m){
    unsigned int curr_time;
    unsigned int temp1 = TMR0H;
    unsigned int temp2 = TMR0L;
    
    curr_time = temp2 | (temp1 << 8); //Writes time of stop to curr_time
    dc_motor_stop(mL, mR);            //Stops the car    
    memory_time(m, curr_time, stage_count);
    T0CON0bits.T0EN = 0; //Turns timer off          
}

/************************************
* Function starts the movement of buggy, restarts and resets the timer, and resets the colour sense interrupt
************************************/
void Movement_start_and_time(struct DC_motor *mL, struct DC_motor *mR, unsigned int lowT, unsigned int highT){
    TMR0L = 0;
    TMR0H = 0;
    ColourLED_set(3);   //turning back on colour sense LED
    __delay_ms(200);
    T0CON0bits.T0EN=1;  //turns timer back on
    dc_motor_ForwardAccelerate(mL,mR);
    color_interrupt = 0;  
    //Resets or setups up I2C_interrupt
    I2C_interrupt_clear();
    I2C_interrupt_init(lowT, highT);
    I2C_interrupt_clear();
}

/************************************
* Function accelerates car and turns on timer and sets to correct time so it stops when required
************************************/
void Movement_reverseSetup(struct DC_motor *mL, struct DC_motor *mR, unsigned int time)
{    
    dc_motor_ForwardAccelerate(mL, mR);
    T0CON0bits.T0EN = 1;
    timer_calculator(time); //Inputs the correct initial value in to the timer
}

/************************************
* Function backs up car to ensure cardboard / car does not get stopped by it
************************************/
void Movement_back_up(struct DC_motor *mL, struct DC_motor *mR){
     dc_motor_BackwardAccelerate(mL,mR);
     __delay_ms(100);
     dc_motor_stop(mL,mR);
     __delay_ms(300); //stop momentum in to turns
}


/************************************
* Function takes in a colour as a number and calls a function to do the required movement
************************************/
void Movement_WhichMovement(char colour, struct DC_motor *mL, struct DC_motor *mR, char forward){
    if (colour == 1){Movement_RedMovement(forward, mL,mR);} 
    else if (colour == 2){Movement_GreenMovement(forward,mL,mR);} 
    else if (colour == 3){Movement_BlueMovement(mL,mR);} 
    else if (colour == 4){Movement_YellowMovement(forward,mL,mR);} 
    else if (colour == 5){Movement_PinkMovement(forward,mL,mR);} 
    else if (colour == 6){Movement_OrangeMovement(forward,mL,mR);} 
    else if (colour == 7){Movement_LightBlueMovement(forward,mL,mR);}
}

/************************************
* Red: Turn Right 90
************************************/
void Movement_RedMovement(char forward, struct DC_motor *mL, struct DC_motor *mR)
{
    if (forward){dc_motor_turnRight90(mL,mR);} 
    else if (!forward){dc_motor_turnLeft90(mL,mR);}
}

/************************************
* Green: Turn Left 90
************************************/
void Movement_GreenMovement(char forward, struct DC_motor *mL, struct DC_motor *mR)
{
    if (forward){dc_motor_turnLeft90(mL,mR);} 
    else if (!forward){dc_motor_turnRight90(mL,mR);}
}

/************************************
* Blue: Turn 180 (don't need to use forward, as reverse of 180 is 180)
************************************/
void Movement_BlueMovement(struct DC_motor *mL, struct DC_motor *mR){
    dc_motor_turnRight90(mL,mR);
    dc_motor_turnRight90(mL,mR);
}

/************************************
* Yellow: Reverse 1 square and turn right 90
************************************/
void Movement_YellowMovement(char forward, struct DC_motor *mL, struct DC_motor *mR)
{
    if (forward)
    {
        dc_motor_BackwardAccelerate(mL,mR);
        __delay_ms(250); 
        dc_motor_stop(mL,mR);
        dc_motor_turnRight90(mL,mR);
    } 
    else if (!forward)
    {
        dc_motor_turnLeft90(mL,mR);
        dc_motor_ForwardAccelerate(mL,mR);
        __delay_ms(250); 
        dc_motor_stop(mL,mR);
    }
}

/************************************
*Pink: Reverse 1 square and turn left 90
************************************/
void Movement_PinkMovement(char forward, struct DC_motor *mL, struct DC_motor *mR){
    if (forward)
    {
        dc_motor_BackwardAccelerate(mL,mR);
        __delay_ms(250); 
        dc_motor_stop(mL,mR);
        dc_motor_turnLeft90(mL,mR);
    } 
    else if (!forward)
    {
        dc_motor_turnRight90(mL,mR);
        dc_motor_ForwardAccelerate(mL,mR);
        __delay_ms(250); 
        dc_motor_stop(mL,mR);
    }
}

/************************************
*Orange: Turn Right 135
************************************/
void Movement_OrangeMovement(char forward, struct DC_motor *mL, struct DC_motor *mR)
{
    if (forward){dc_motor_turnRight90(mL,mR); dc_motor_turnRight45(mL,mR);} 
    else if (!forward){dc_motor_turnLeft90(mL,mR); dc_motor_turnLeft45(mL,mR);}
}

/************************************
* Light blue: Turn Left 135
************************************/
void Movement_LightBlueMovement(char forward, struct DC_motor *mL, struct DC_motor *mR)
{
    if (forward){dc_motor_turnLeft90(mL,mR); dc_motor_turnLeft45(mL,mR);} 
    else if (!forward){dc_motor_turnRight90(mL,mR); dc_motor_turnRight45(mL,mR);}   
}