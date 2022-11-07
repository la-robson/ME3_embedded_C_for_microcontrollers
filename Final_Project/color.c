#include <xc.h>
#include "color.h"
#include "card.h"
#include "i2c.h"
#include "LEDs.h"
#include "ColourLED.h"

/************************************
 * Function to initiate the colour click 
************************************/
void color_click_init(void)
{   
    //setup colour sensor via i2c interface
    I2C_2_Master_Init();      //Initialise i2c Master
     //set device PON
	 color_writetoaddr(0x00, 0x01);
    __delay_ms(3); //need to wait 3ms for everthing to start up
    //turn on device ADC
	color_writetoaddr(0x00, 0x03);
    //set integration time
	color_writetoaddr(0x01, 0xD5);
}

/************************************
 * Function to write readings to an address
************************************/
void color_writetoaddr(char address, char value){
    I2C_2_Master_Start();         //Start condition
    I2C_2_Master_Write(0x52 | 0x00);     //7 bit device address + Write mode
    I2C_2_Master_Write(0x80 | address);    //command + register address
    I2C_2_Master_Write(value);    
    I2C_2_Master_Stop();          //Stop condition
}

/************************************
 * Function to read the red value
************************************/
unsigned int color_read_Red(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x16);    //command (auto-increment protocol transaction) + start at RED low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (unsigned int)(I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

/************************************
 * Function to read the green value
************************************/
unsigned int color_read_Green(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);    //7 bit device address + Write (0) mode (note 0x52=0x29<<1)
	I2C_2_Master_Write(0xA0 | 0x18);    //command (auto-increment protocol transaction) + start at GREEN low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);    //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (unsigned int)(I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

/************************************
 * Function to read the blue value
************************************/
unsigned int color_read_Blue(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x1A);    //command (auto-increment protocol transaction) + start at BLUE low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (unsigned int)(I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}


/************************************
 * Function to read the blue value
************************************/
unsigned int color_read_Clear(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x14);    //command (auto-increment protocol transaction) + start at BLUE low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the LSB
	tmp=(tmp | (unsigned int)(I2C_2_Master_Read(0)<<8)); //read the MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

/************************************
 * Function to read all the colour values for each led into the structure 
************************************/
void color_read(struct colour *col)
{
    for (char i=0 ; i<4; ++i)
    {
        ColourLED_set(i);   // set led colour and delay
        __delay_ms(500);
        // read all the colours into the structure
        col->r[i] = color_read_Red();
        col->g[i] = color_read_Green();
        col->b[i] = color_read_Blue();
        col->c[i] = color_read_Clear();
    }
    ColourLED_off();
    
}

/************************************
 * Function to interpolate RGBC readings based off white and black readings
************************************/
void color_cal(struct colour *col)
{
    for (char i=0 ; i<4; ++i) // calibrate each reading
    {
        // check that reading is between max and min
        // if reading too high or low assign max or min value
        if (col->r[i] < black.r[i]) {col->r[i] = 0;}
        else if (col->r[i] > white.r[i]) {col->r[i] = 100;}
        else {col->r[i] = ((col->r[i]-black.r[i])*100)/(white.r[i]-black.r[i]);}
    
        // same logic as previous but repeated for each colour
        if (col->g[i] < black.g[i]) {col->g[i] = 0;}
        else if (col->g[i] > white.g[i]) {col->g[i] = 100;}
        else {col->g[i] = ((col->g[i]-black.g[i])*100)/(white.g[i]-black.g[i]);}
        
        if (col->b[i] < black.b[i]) {col->b[i] = 0;}
        else if (col->b[i] > white.b[i]) {col->b[i] = 100;}
        else {col->b[i] = ((col->b[i]-black.b[i])*100)/(white.b[i]-black.b[i]);}
        
        if (col->c[i] < black.c[i]) {col->c[i] = 0;}
        else if (col->c[i] > white.c[i]) {col->c[i] = 100;}
        else {col->c[i] = ((col->c[i]-black.c[i])*100)/(white.c[i]-black.c[i]);}
    }
}

/************************************
 * Function to take calibration readings of the black and white card
************************************/
void color_cal_init(void)
{
    LATDbits.LATD7=1;       //LED to show you need to click button to start calibration
    while(PORTFbits.RF2);   //Wait for button press
    //Calibration routine 
    LATDbits.LATD7=0;
    __delay_ms(2000);
    LEDs_flash();
    __delay_ms(1000);
    color_read(&black); //Stores readings in black global structure 
    
    __delay_ms(2000);
    LEDs_flash();
    __delay_ms(1000);
    color_read(&white); //Stores readings in white global structure 
}