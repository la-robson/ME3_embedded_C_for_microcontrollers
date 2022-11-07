#include <xc.h>
#include "ColourLED.h"

/************************************
 * Function initialise pins for colour sense LED
************************************/
void ColourLED_init(void)
{
    //Red
    LEDRed = 0; //Turn off
    TRISGbits.TRISG0 = 0; //Set to output
    //Green
    LEDGreen = 0; 
    TRISEbits.TRISE7 = 0;
    //Blue
    LEDBlue = 0;
    TRISAbits.TRISA3 = 0;
}

/************************************
 * Function to turn off all colour sense LEDS
************************************/
void ColourLED_off(void)
{
    LEDRed = 0;
    LEDGreen = 0;
    LEDBlue = 0;
}

/************************************
 * Function to allow the colour sense LED to be turned on depending on number inputted
************************************/
void ColourLED_set(char c)
{
    if (c==0){LEDRed = 1; LEDGreen = 0; LEDBlue = 0;} // red
    if (c==1){LEDRed = 0; LEDGreen = 1; LEDBlue = 0;} // green
    if (c==2){LEDRed = 0; LEDGreen = 0; LEDBlue = 1;} // blue
    if (c==3){LEDRed = 1; LEDGreen = 1; LEDBlue = 1;} // clear
}
