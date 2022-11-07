#include <xc.h>
#include <stdio.h>
#include "LCD.h"

/************************************
 * Function to toggle LCD enable bit on then off
 * when this function is called the LCD screen reads the data lines
************************************/
void LCD_E_TOG(void)
{
    LCD_E = 1;              //turn the LCD enable bit on
	__delay_us(2);          //wait a short delay
    LCD_E = 0;              //turn the LCD enable bit off again
}

/************************************
 * Function to set the 4-bit data line levels for the LCD
************************************/
void LCD_sendnibble(unsigned char number)
{
    // set the pins to the correct nibble
    if (number & 0b0001) {LCD_DB4=1;} else {LCD_DB4=0;}
    if (number & 0b0010) {LCD_DB5=1;} else {LCD_DB5=0;}
    if (number & 0b0100) {LCD_DB6=1;} else {LCD_DB6=0;}
    if (number & 0b1000) {LCD_DB7=1;} else {LCD_DB7=0;}

    LCD_E_TOG();		//toggle the enable bit to instruct the LCD to read the data lines
    __delay_us(5);      //Delay 5uS
}

/************************************
 * Function to send full 8-bit commands/data over the 4-bit interface
 * high nibble (4 most significant bits) are sent first, then low nibble sent
************************************/
void LCD_sendbyte(unsigned char Byte, char type)
{
    // set RS pin whether it is a Command (0) or Data/Char (1) using type argument
    LCD_RS = type;
    // send high bits of Byte using LCD_sendnibble function
    LCD_sendnibble(Byte>>4);
    // send low bits of Byte using LCDout function
    LCD_sendnibble(Byte & 0b00001111);
    __delay_us(50);               //delay 50uS (minimum for command to execute)
}

/************************************
 * Function to initialise the LCD after power on
************************************/
void LCD_Init(void)
{
    //set all pins low (might be random values on start up)
    LCD_RS = 0; 
    LCD_E = 0; 
    LCD_DB4 = 0; 
    LCD_DB5 = 0; 
    LCD_DB6 = 0; 
    LCD_DB7 = 0; 
   
    //Define LCD Pins as Outputs
    TRISCbits.TRISC6=0;
    TRISCbits.TRISC2=0;
    TRISBbits.TRISB3=0;
    TRISBbits.TRISB2=0;
    TRISEbits.TRISE3=0;
    TRISEbits.TRISE1=0;

    //Initialisation sequence code
    __delay_ms(45);      // wait 40 ms 
    LCD_sendnibble(0b0011);
    __delay_us(45);      // wait 39us
    LCD_sendnibble(0b0010);
    LCD_sendnibble(0b1000);
    __delay_us(45);     // wait 39us
    LCD_sendnibble(0b0010);
    LCD_sendnibble(0b1000);
    __delay_us(45);      // wait 37us
    LCD_sendnibble(0b0000);
    LCD_sendnibble(0b1100);
    __delay_us(45);     // wait 37us
    LCD_sendnibble(0b0000);
    LCD_sendnibble(0b0001);
    __delay_us(1560);   // wait 1530us
    LCD_sendnibble(0b0000);
    LCD_sendnibble(0b0101);
}

/************************************
 * Function to set the cursor to beginning of line 1 or 2
************************************/
void LCD_setline (char line)
{
    //Send 0x80 to set line to 1 (0x00 ddram address)
    if (line==1) {LCD_sendbyte(0x80,0);}    
    //Send 0xC0 to set line to 2 (0x40 ddram address)
    if (line==2) {LCD_sendbyte(0xC0,0);}
}

/************************************
 * Function to send string to LCD screen
************************************/
void LCD_sendstring(char *string)
{	  
	while(*string != 0){  // While the data pointed to isn?t a 0x00 do below (strings in C must end with a NULL byte) 
		LCD_sendbyte(*string++,1); 	//Send out the current byte pointed to and increment the pointer
    
	}
}

/************************************
 * Function to display useful time date and day info 
************************************/
void LCD_dispInfo (char hours, int date)
{
    char buf[20];                       // temp string for the LCD display
    sprintf(buf,"Time:%dh  ",hours);    // string of the time
    LCD_setline(1);                     // set line 1
    LCD_sendstring(buf);                // display time 
    sprintf(buf,"Date:%d  D/N: %d ",date, day);     // string of date and day/night status
    LCD_setline(2);                     // set line 2
    LCD_sendstring(buf);                // display date and day/night status
}