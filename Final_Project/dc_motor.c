#include <xc.h>
#include "dc_motor.h"
#include "LEDs.h"

/************************************
 * Function to initialise Timer 2 & DC Motor Control
************************************/
void dc_motor_init(int PWMperiod){
	//initialising TRIS and LAT registers for DC Motor pins
    LATEbits.LATE2 = 1;
    TRISEbits.TRISE2 =  0;
    LATEbits.LATE4 = 1;
    TRISEbits.TRISE4 = 0;
    LATCbits.LATC7 = 1;
    TRISCbits.TRISC7 = 0;
    LATGbits.LATG6 = 1;
    TRISGbits.TRISG6 = 0;

    // timer 2 config
    T2CONbits.CKPS=0b011; // 1:8 prescaler
    T2HLTbits.MODE=0b00000; // Free Running Mode, software gate only
    T2CLKCONbits.CS=0b0001; // Fosc/4

    // Tpwm*(Fosc/4)/prescaler - 1 = PTPER
    T2PR=199; //Period reg 10kHz base period (199)
    T2CONbits.ON=1;

    PWM6DCH= 0; //0% power
    PWM7DCH= 0; //0% power
    
    PWM6CONbits.EN = 1;
    PWM7CONbits.EN = 1;
    
    RE2PPS=0x0A; //PWM6 on RE2
    RC7PPS=0x0B; //PMW7 on RC7
}



/************************************
 * Function to set PWM output from the values in the motor structure
************************************/
void dc_motor_setPWM(struct DC_motor *m){
	int PWMduty; //tmp variable to store PWM duty cycle
    
	if (m->direction){ //if forward
		// low time increases with power
		PWMduty = m->PWMperiod - ((int)(m->power)*(m->PWMperiod))/100;
    //for forward if PMWduty = 0 then full power
	}
	else { //if reverse
		// high time increases with power
		PWMduty = ((int)(m->power)*(m->PWMperiod))/100;
    //for reverse if PMWPduty = PMWPeriod then full power
	}

	*(m->dutyHighByte) = PWMduty; //set high duty cycle byte
        
	if (m->direction){ // if direction is high
		*(m->dir_LAT) = *(m->dir_LAT) | (1<<(m->dir_pin)); // set dir_pin bit in LAT to high without changing other bits
	} else {
		*(m->dir_LAT) = *(m->dir_LAT) & (~(1<<(m->dir_pin))); // set dir_pin bit in LAT to low without changing other bits
	}
}

/************************************
 * Function to stop the buggy gradually 
************************************/
void dc_motor_stop(struct DC_motor *mL, struct DC_motor *mR)
{
    BRAKE = 1; // turn on brake lights
    // while power is not 0
    while ((mL->power != 0) | (mR->power != 0))
    {
        // reduce L motor power in increments of 1
        if (mL->power > 0) {mL->power -= 1;}
        // reduce R motor power in increments of 1
        if (mR->power > 0) {mR->power -= 1;}
        
        dc_motor_setPWM(mL);
        dc_motor_setPWM(mR);
        __delay_ms(10);        // delay to give gradual stop
    }
    BRAKE = 0;  // turn off brake lights
}

/************************************
 * Function to make the buggy turn left 45 degree
************************************/
void dc_motor_turnLeft45(struct DC_motor *mL, struct DC_motor *mR){
    mL->direction = 0; //Left backwards
    mR->direction = 1; //Right forwards
    
    while (mR->power < mR->turningCircle){
        mL->power += 2; //Increase backwards more as DC motor when back seemed less strong
        mR->power += 1;
        dc_motor_setPWM(mL);
        dc_motor_setPWM(mR);
        __delay_ms(3); 
    }
    __delay_ms(30);
    dc_motor_stop(mL,mR);   
}

/************************************
 * Function to make the buggy turn left 90 degree
************************************/
void dc_motor_turnLeft90(struct DC_motor *mL, struct DC_motor *mR){  
 dc_motor_turnLeft45(mL,mR);
 dc_motor_turnLeft45(mL,mR);
}

/************************************
 * Function to make the buggy turn right 45 degree
************************************/
void dc_motor_turnRight45(struct DC_motor *mL, struct DC_motor *mR) {
    mL->direction = 1;
    mR->direction = 0;

    while (mL->power < mL->turningCircle) { //Number Comes from trial and error of around 45 degree
        mL->power += 1;
        mR->power += 2;
        dc_motor_setPWM(mL);
        dc_motor_setPWM(mR);
        __delay_ms(3);
    }
    __delay_ms(30);
    dc_motor_stop(mL, mR);
}

/************************************
 * Function to make the buggy turn right 90 degree
************************************/
void dc_motor_turnRight90(struct DC_motor *mL, struct DC_motor *mR){  
 dc_motor_turnRight45(mL,mR);
 dc_motor_turnRight45(mL,mR);
}

/************************************
 * Function to make the buggy accelerate forwards to full power
************************************/
void dc_motor_ForwardAccelerate(struct DC_motor *mL, struct DC_motor *mR){
    // set direction to forward 
    mL->direction = 1;
    mR->direction = 1;
    
    // even up the power on both wheels
    if (mL->power > mR->power) {mR->power = mL->power;}
    else {mL->power = mR->power;}
    
    // while power is not 100
    while ((mL->power != fwdMaxPower) | (mR->power != fwdMaxPower))
    {
        // increase both motors power in increments of 1
        mL->power += 1;
        mR->power += 1;
        dc_motor_setPWM(mL);
        dc_motor_setPWM(mR);
        __delay_ms(10);        // delay so not a sudden increase 
    }
}


/************************************
 * Function to make the buggy accelerate backwards to full power
************************************/
void dc_motor_BackwardAccelerate(struct DC_motor *mL, struct DC_motor *mR){
    // set direction to reverse
    mL->direction = 0;
    mR->direction = 0;
    
    // even up the power on both wheels
    if (mL->power > mR->power) {mR->power = mL->power;}
    else {mL->power = mR->power;}
    
    // while power is not 100
    while ((mL->power != bkwdMaxPower) | (mR->power != bkwdMaxPower))
    {
        // increase both motors power in increments of 1
        mL->power += 1;
        mR->power += 1;
        dc_motor_setPWM(mL);
        dc_motor_setPWM(mR);
        __delay_ms(10);        // delay so not a sudden increase 
    }
}

/************************************
 * Function to make the buggy shuffle forwards and backwards to help card reading
************************************/
void dc_motor_shuffle(char attempt,struct DC_motor *mL, struct DC_motor *mR){
    
    unsigned char max_power;
    //if odd number it will go forward, if even it will go backwards
    if (!(attempt % 2)){
        max_power = 40; //backwards
        mL->direction = 0;
        mR->direction = 0;
    } 
    else 
    {
        max_power = 15; //forwards
        mL->direction = 1;
        mR->direction = 1;
    }


    while ((mL->power != max_power) | (mR->power != max_power)) 
        {
        // increase both motors power in increments of 1
        mL->power += 1;
        mR->power += 1;
        dc_motor_setPWM(mL);
        dc_motor_setPWM(mR);
        __delay_ms(7); // delay so not a sudden increase
        }

    __delay_ms(100);
    
    dc_motor_stop(mL,mR);
}

/************************************
 * Function to calibrate the DC motors for turning (allows slightly more / less)
************************************/
void dc_motor_movemoventCalibration(struct DC_motor *mL, struct DC_motor *mR){
    char clicks;
    
    LATDbits.LATD7=1; //Turn RD7 on to show that calibration needs to be initialised
    while(PORTFbits.RF3); //Need to press RF3 to start calibration, do nothing till it is
    LATDbits.LATD7=0; //Turn off RD7 as calibration has started
    LATHbits.LATH3=1; //Turn on RH3 to show movement calibration is on going
    
    dc_motor_turnRight90(mL,mR); //Seeing the turn right, easier to see 90 degree than 45 degree
    
    //Outputs clicks approximately equal to 10 if held and less than if not held   
    clicks = LEDs_holdOrPress();
    
    while(clicks < 5){ //If click is held calibration is ended
        if(!PORTFbits.RF3){ //Click RF3 to get the car to turn more
            mL->turningCircle += 1;
            mR->turningCircle += 1;
            //Blink RD7 to show change is happening
            LATDbits.LATD7=1;
            __delay_ms(500);
            LATDbits.LATD7=0;
            __delay_ms(500);
            //Show the turn again to see if other changes are needed
            dc_motor_turnRight90(mL,mR);
            clicks = LEDs_holdOrPress();
        } else if(!PORTFbits.RF2){ //Click RF2 to get the car to turn less
            mL->turningCircle -= 1;
            mR->turningCircle -= 1;
            //Blink RD7 to show change is happening  
            LATDbits.LATD7=1;
            __delay_ms(500);
            LATDbits.LATD7=0;
            __delay_ms(500);
            //Show the turn again to see if other changes are needed
            dc_motor_turnRight90(mL,mR);
            clicks = LEDs_holdOrPress();
        }    
    }
    LATHbits.LATH3=0; //Turn RH3 off to show end of calibration  
    __delay_ms(100); //Gives some time for user to remove finger
}