#include <xc.h>
#include <pic18f67k40.h>
#include "interrupts.h"
#include "i2c.h"
#include "color.h"
#include "LEDs.h"
#include "dc_motor.h"
#include "timers.h"
#include "ColourLED.h"

/************************************
 * Function to turn on interrupts (pin set up for colour sense) and set if priority is used
************************************/
void Interrupts_init(void)
{    
    INTCONbits.IPEN=1;  // define priority level
    
    //Setting up interrupt on RB0
    IPR0bits.INT0IP = 1;  ///high priority  
    INTCONbits.INT0EDG = 0; //interrupt on falling edge
    
    //Input on RB0 for interrupt 0 (is default setting)
    PIE0bits.INT0IE = 1;  //enable interrupt external bit on (0)
    PIR0bits.INT0IF = 0;  //Ensures flag is off
 
    INTCONbits.GIE=1; 	// turn on high priority interrupts globally
}

/************************************
 * I2C Interrupt Setup
************************************/
void I2C_interrupt_init(unsigned int lowT, unsigned int highT){
    
    unsigned int lowT_lowB = (lowT & 0b11111111);         //High threshold, low byte
    unsigned int lowT_highB = (lowT >> 8);                //Low threshold, high byte
    unsigned int highT_lowB = (highT & 0b11111111);       //High threshold, low byte
    unsigned int highT_highB = (highT >> 8);              //High threshold, high byte
    
    color_writetoaddr(0x00, 0x13);    //Initialising the interrupt on I2C
    color_writetoaddr(0x04, lowT_lowB);    //Initialising lower byte low threshold value 
    color_writetoaddr(0x05, lowT_highB);    //Initialising high byte low threshold value 
    color_writetoaddr(0x06, highT_lowB);    //Initialising low byte high threshold value 
    color_writetoaddr(0x07, highT_highB);    //Initialising high byte high threshold value 
    color_writetoaddr(0x0C, 0b0001);    //Initialising  persistence filter and setting to 1
}

/************************************
 * I2C Interrupt clear
************************************/
void I2C_interrupt_clear(void){
    I2C_2_Master_Start();                //Start condition
    I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
    I2C_2_Master_Write(0xE6);            // special function, clear interrupt
    I2C_2_Master_Stop();
}

/************************************
 * I2C Interrupt calibration - setting interrupt values 
************************************/
unsigned int interrupt_set(void){
    
    ColourLED_set(3);
    unsigned int interrupt_value;    

    LATDbits.LATD7=1;
    while(PORTFbits.RF3); //wait for click to read card
    interrupt_value = color_read_Clear();     
    LATDbits.LATD7=0;
    
    __delay_ms(1000);
   
    return interrupt_value;
}

/************************************
 * calibration routine for light interrupt
************************************/
void interrupt_calibration(unsigned int *high_interrupt_val, unsigned int *low_interrupt_val)
{  
    *high_interrupt_val = interrupt_set();   //Set with the blue card close to
    *low_interrupt_val = interrupt_set();    //Set with the black card at stopping distance
    LATDbits.LATD7=1;      // set indicator LEDs
    LATHbits.LATH3=1;
    while(PORTFbits.RF3);  // wait for click to read card         
    __delay_ms(1000);      // delay for usability
    LATDbits.LATD7=0;      // set indicator LEDs
    LATHbits.LATH3=0;
    // set interrupt with calibration values
    I2C_interrupt_init(*low_interrupt_val, *high_interrupt_val);   
}

/************************************
 * Timer interrupt setup
************************************/
void timer0_interrupt(void){
   //Interrupt for Timer Reset    
    IPR0bits.TMR0IP = 1;  //it is lower priority
    PIE0bits.TMR0IE = 1;  //enable interrupt source INT0 (enable them on these bits)
    PIR0bits.TMR0IF = 0;  //Ensures flag is off
    INTCONbits.GIE=1; 	// ensures interrupts turned on high priority interrupts globally
}

/************************************
 * High priority interrupt service routine (timer & colour sense)
************************************/
void __interrupt(high_priority) HighISR()
{    
    if(PIR0bits.INT0IF) //check the interrupt source for colour sense
    { 					
        color_interrupt = 1; //changing global variable
        PIR0bits.INT0IF = 0;   //Clearing interrupt flag
        I2C_interrupt_clear();  //Clearing interrupt I2C
        color_writetoaddr(0x00, 0x03);  //Disabling the interrupt
    }
    
    if (PIR0bits.TMR0IF) // timer interrupt for the car memory
    {
        timer_interrupt = 1; //timer global variable
        T0CON0bits.T0EN=0;   //timer off
        PIR0bits.TMR0IF = 0; //clearing flag
    }
}
