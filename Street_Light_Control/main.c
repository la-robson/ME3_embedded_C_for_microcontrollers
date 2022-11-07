// CONFIG1L
#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)

// CONFIG3L
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)

#include <xc.h>
#include <stdio.h>
#include "LEDarray.h"
#include "interrupts.h"
#include "comparator.h"
#include "timers.h"
#include "LCD.h"

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

/****************************************************************************
 ******************* Street light control system ****************************
 * 
 * Street light (LED RH3) will turn on when it is dark (detected by LDR)
 * except between 1-5am to reduce energy wastage. 
 * The current time is displayed in hours in binary on the LED array, it is 
 * also displayed along with the date on the LCD array to aid testing. 
 * The control system can function in testing mode where an hour lasts 2s 
 * or it operational mode where an hour lasts the full 3600s. 
 * 
 * Change the mode by changing the hourFactor variable below. 
 * 
*****************************************************************************/


// create necessary global variables
int hourFactor=2;   // number of seconds in an hour (test=2, real=3600)
char day=0;         // day/night variable (starts at midnight)
int time=0;        // time variable to count 1 second increments
int dawn;           // variable to record dawn time
int dusk;           // variable to record dusk time


void main(void) {
        
    // initiate interrupts, comparator, timer, LEDs and LCD screen
    Interrupts_init(); 
    Comp1_init();
    Timer0_init();
    RH3led_init();
    LEDarray_init();
    LCD_Init();
    
    // define time/date variables and set initial values 
    // starting at midnight on Friday Jan 1st 2021
    char hours = 0;       // variable to count the hours
    int date = 1;         // variable to track the date
    char dayOfWeek = 5;   // variable to keep track of the day
    char leapYear = 1;    // variable tracking leap years (leap year when =4)
    dawn = 8*hourFactor;            // initial dawn time 
    dusk = 16*hourFactor;           // initial dusk time 
 
    // define checker variables ensuring variables only get updated once
    char savings = 1;     // variable for if winter daylight savings have been added
    char hourCheck = 0;   // variable to ensure hour is only updated once 
    char noonCheck = 0;             // variable tracking if noon has been checked
    
    // main operating code
    while (1) {
        
        time_to_hour(&hours, &hourCheck); // increment if an hour has passed    
        LEDarray_disp_bin(hours);   // display the time in binary
        
        // set street light
        // off during day, or if time is between 1-5am
        // on during night, excluding 1-5am
        LEDarray_streetlight(&hours, &noonCheck);
        
        // update time/date at midnight        
        date = update_date(&hours, date, &dayOfWeek); 
        
        // check and implement daylight savings
        daylight_savings (&hours, date, dayOfWeek, &savings);
             
        // check for time drifting and correct if necessary
        time_drifting (&noonCheck, savings);
        
        // check if the year needs updating and update if necessary 
        date = year_update (&leapYear, date);
        
        // display useful info on LCD 
        LCD_dispInfo (hours, date); 
        
        
    }
}