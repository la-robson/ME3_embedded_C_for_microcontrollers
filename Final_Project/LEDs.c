#include <xc.h>
#include "LEDs.h"

/************************************
 * Function to initialise buggy & clicker 2 lights & buttons
************************************/
void LEDs_init(void)
{
    // set all pins to low
    BRAKE = 0;
    HLAMP = 0;
    MBEAM = 0;
    TURNL = 0;
    TURNR = 0;
    
    // set pins as output
    TRISDbits.TRISD4 = 0;
    TRISDbits.TRISD3 = 0;
    TRISHbits.TRISH0 = 0;
    TRISHbits.TRISH1 = 0;
    TRISFbits.TRISF0 = 0;
    
    // set board lights
    TRISDbits.TRISD7=0; //set TRIS value for pin (output)
    TRISHbits.TRISH3=0; //set TRIS value for pin (output)
    LATDbits.LATD7=0;   //set initial output state
    LATHbits.LATH3=0;   //initially off
    
    // setup pin for input (connected to button)
    TRISFbits.TRISF2=1; //set TRIS value for pin RF2 (input)
    ANSELFbits.ANSELF2=0; //turn off analogue input on pin 
    TRISFbits.TRISF3=1; //set TRIS value for pin RF3 (input)
    ANSELFbits.ANSELF3=0; //turn off analogue input on pin 
}

/************************************
 * Flash all LEDs for 2s to attract attention
************************************/
void LEDs_flash(void)
{
    for (char i=0; i<10; ++i)
    {
        __delay_ms(100);
        BRAKE = 1;
        HLAMP = 0;
        MBEAM = 1;
        TURNL = 0;
        TURNR = 1;
        __delay_ms(100);
        BRAKE = 0;
        HLAMP = 1;
        MBEAM = 0;
        TURNL = 1;
        TURNR = 0;
    }
    HLAMP = 0;
    TURNL = 0;
}

/************************************
 * Flash hazards for 2s
************************************/
void LEDs_hazards()
{
    for (char i=0; i<5; i++)
    {
        TURNL = 1;
        TURNR = 1;
        __delay_ms(200);
        TURNL = 0;
        TURNR = 0;
        __delay_ms(200);
    }
}


/************************************
 * Press or hold, outputs approximately 10 for a hold and 1 for a click
************************************/
char LEDs_holdOrPress(void){
    char i;
    char clicks=0;
    //Waiting until first click
    while(PORTFbits.RF3);
    //Seeing if the button is held or clicked 
    for (i=0;i<11;i++){
        if (!PORTFbits.RF3) {clicks += 1;}
        __delay_ms(250);
    }
    
    return clicks;
}