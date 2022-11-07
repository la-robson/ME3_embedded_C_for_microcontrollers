#ifndef _DC_MOTOR_H
#define _DC_MOTOR_H

#include <xc.h>

#define _XTAL_FREQ 64000000
#define fwdMaxPower 16 //Max forwards power
#define bkwdMaxPower 50 //Max backwards power

struct DC_motor { //definition of DC_motor structure
    char power;         //motor power, out of 100
    char direction;     //motor direction, forward(1), reverse(0)
    unsigned char *dutyHighByte; //PWM duty high byte address
    unsigned char *dir_LAT; //LAT for dir pin
    char dir_pin; // pin number that controls direction on LAT
    int PWMperiod; //base period of PWM cycle
    char turningCircle; //How high the forwards power needs to go to turn the car 45 degrees
};

//function prototypes
void dc_motor_init(int PWMperiod); // function to setup PWM
void dc_motor_setPWM(struct DC_motor *m);
void dc_motor_stop(struct DC_motor *mL, struct DC_motor *mR);
void dc_motor_turnLeft45(struct DC_motor *mL, struct DC_motor *mR);
void dc_motor_turnLeft90(struct DC_motor *mL, struct DC_motor *mR);
void dc_motor_turnRight45(struct DC_motor *mL, struct DC_motor *mR);
void dc_motor_turnRight90(struct DC_motor *mL, struct DC_motor *mR);
void dc_motor_ForwardAccelerate(struct DC_motor *mL, struct DC_motor *mR);
void dc_motor_BackwardAccelerate(struct DC_motor *mL, struct DC_motor *mR);
void dc_motor_movemoventCalibration(struct DC_motor *mL, struct DC_motor *mR);
void dc_motor_shuffle(char attempt ,struct DC_motor *mL, struct DC_motor *mR);


#endif
