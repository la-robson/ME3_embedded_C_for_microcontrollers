#include <xc.h>
#include "LEDarray.h"

/************************************
/ RH3led_init
/ Function used to initialise the RH3 LED
************************************/
void RH3led_init(void)
{
    LATHbits.LATH3=0;   //initial output state for LED
    TRISHbits.TRISH3=0; //set TRIS value for LED pin (output)
}
    
    
/************************************
/ LEDarray_init
/ Function used to initialise pins to drive the LEDarray
************************************/
void LEDarray_init(void)
{
    //set initial output LAT values (they may have random values when powered on)
    LATGbits.LATG0=0; 
    LATGbits.LATG1=0;
    LATAbits.LATA2=0;
    LATFbits.LATF6=0;
    LATAbits.LATA4=0;
    LATAbits.LATA5=0;
    LATFbits.LATF0=0;
    LATBbits.LATB0=0;
    LATBbits.LATB1=0;
    
    //set up TRIS registers for pins connected to LED array
    TRISGbits.TRISG0=0;
    TRISGbits.TRISG1=0;
    TRISAbits.TRISA2=0;
    TRISFbits.TRISF6=0;
    TRISAbits.TRISA4=0;
    TRISAbits.TRISA5=0;
    TRISFbits.TRISF0=0;
    TRISBbits.TRISB0=0;
    TRISBbits.TRISB1=0;
	
}  

/************************************
/ LEDarray_disp_bin
/ Function used to display a number on the LED array in binary
************************************/
void LEDarray_disp_bin(unsigned int number)
{
	//some code to turn on/off the pins connected to the LED array
	//if statements and bit masks can be used to determine if a particular pin should be on/off
	//see Readme.md for examples
    
    // Check in turn each bit and set the corresponding LED state
    if (number & 0b000000001) {LATGbits.LATG0=1;} else {LATGbits.LATG0=0;}
    if (number & 0b000000010) {LATGbits.LATG1=1;} else {LATGbits.LATG1=0;}
    if (number & 0b000000100) {LATAbits.LATA2=1;} else {LATAbits.LATA2=0;} 
    if (number & 0b000001000) {LATFbits.LATF6=1;} else {LATFbits.LATF6=0;}
    if (number & 0b000010000) {LATAbits.LATA4=1;} else {LATAbits.LATA4=0;}
    if (number & 0b000100000) {LATAbits.LATA5=1;} else {LATAbits.LATA5=0;}
    if (number & 0b001000000) {LATFbits.LATF0=1;} else {LATFbits.LATF0=0;}
    if (number & 0b010000000) {LATBbits.LATB0=1;} else {LATBbits.LATB0=0;}
    if (number & 0b100000000) {LATBbits.LATB1=1;} else {LATBbits.LATB1=0;}
             
}

/************************************
/ LEDarray_streetlight
/ Function used to display the street light
/ light off if day or between 1-5am
/ light on if night and not between 1-5am
/ also resets noonCheck during the day
************************************/
void LEDarray_streetlight(char *hours, char *noonCheck)
{
    if (day==0) {                                       // check if it is night     
            if (*hours>=1 & *hours<5) {LATHbits.LATH3=0;}     // light off between 1-5am
            else {LATHbits.LATH3=1;}                          // turn light on if night
        } 
        else {LATHbits.LATH3=0; *noonCheck=0;}    // light off if day and reset noonCheck
}