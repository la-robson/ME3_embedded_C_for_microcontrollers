#include <xc.h>
#include <stdlib.h>
#include "color.h"
#include "card.h"
#include "ColourLED.h"
#include "LEDs.h"
#include "memory.h"

/************************************
 * Function to set the colour mode (easy = 0, hard = 1)
************************************/
char card_mode(void){
    char mode = 2; //Assumes 2 to start
    LATDbits.LATD7=1; //Turns on LED
    while (mode == 2) //loops until RF2 / RF3 is clicked
    {
        if (!PORTFbits.RF2){mode = 0;} //Easy Mode if RF2 is clicked (0)
        else if (!PORTFbits.RF3){mode = 1;} //Hard Mode if RF3 is clicked (1)
    }
    LATDbits.LATD7=0; //Turns on LED
    __delay_ms(1000); //delay to take finger off
    return mode;
}

/************************************
 * Normalises readings relative to clear by dividing by clear
************************************/
void card_normalise(struct colour *col)
{
    for (char i=0 ; i<4; ++i) // normalise each reading against clear
    {        
        if (col->c[i] != 0) // check that not dividing by 0 before calculation
        {
            col->r[i] = (100*col->r[i]) / col->c[i] ;
            col->g[i] = (100*col->g[i]) / col->c[i] ;
            col->b[i] = (100*col->b[i]) / col->c[i] ;
            col->c[i] = 100;
        }
    }
}

/************************************
 * Function to determine the colour of the card
 * takes in CALIBRATED colour structure (NOT normalised)
 * and outputs the card number
 * 
//// CARD ////
1 Red           Turn Right 90
2 Green         Turn Left 90
3 Blue          Turn 180
4 Yellow        Reverse 1 square and turn right 90
5 Pink          Reverse 1 square and turn left 90
6 Orange        Turn Right 135
7 Light blue	Turn Left 135
8 White         Finish (return home)
9 Black         Maze wall colour
 * 
//// MODE ////
0 easy (red, green, blue, black, white)
1 hard (all colours)
 * 
************************************/
char card_identify(struct colour *col, char mode)
{
    char card = 0; // create variable for the card
    int sum = 0;
    // calculate the sum of RGBC values
    for (char i=0 ; i<4; ++i)
    {        
        sum += col->r[i] + col->g[i] + col->b[i] + col->c[i];
    }
    
    // check first for black or white
    if (sum > (93*16)) {card = 8;}          // white card
    else if (sum < (10*16)) {card = 9;}     // black card
    
    // if not black or white normalise the reading then check for each colour
    else
    {
        card_normalise(col);
        
        if (mode == 0)      // EASY MODE (red, green, blue only)
        {
            //red
            if ( ((abs((int)(col->r[0] - red.r[0])) < tol)  //from blue
                & (abs((int)(col->b[0] - red.b[0])) < tol*2)) //confirm
                & (abs((int)(col->b[1] - red.b[1])) < tol) ) //from green & blue
            {
                if (abs((int)(col->g[2] - red.g[2])) < (tol*5) / 2) // from blue & green
                {card = 1;} // red card          
            }
            //green
            else if ( ((abs((int)(col->r[1] - green.r[1])) < tol*2) //confirming
                    & (abs((int)(col->g[1] - green.g[1])) < tol*2)) //confirming
                    & (abs((int)(col->b[1] - green.b[1])) < tol) )  //from red & blue
            {
                if (col->r[2] > 5) // from blue (ish)
                {card = 2;} // green card          
            }

            else if ( ((abs((int)(col->g[1] - blue.g[1])) < tol*2) //confirming
                    & (abs((int)(col->g[2] - blue.g[2])) < tol*2)) //from red
                    & (abs((int)(col->b[1] - blue.b[1])) < tol*2) ) //from green & blue
            {card = 3;} // blue card 
        }
        
        else if (mode == 1)   // HARD MODE (all colours)
        {
            //red
            if ( ((abs((int)(col->r[0] - red.r[0])) < tol)  //from blue
                & (abs((int)(col->b[0] - red.b[0])) < tol*2)) //confirm
                & (abs((int)(col->b[1] - red.b[1])) < tol) ) //from green & blue
            {
                if (abs((int)(col->g[2] - red.g[2])) < (tol*5) / 2) // from blue & green
                {card = 1;} // red card          
            }
            //green
            else if ( ((abs((int)(col->r[1] - green.r[1])) < tol*2) //confirming
                    & (abs((int)(col->g[1] - green.g[1])) < tol*2)) //confirming
                    & (abs((int)(col->b[1] - green.b[1])) < tol) )  //from red & blue
            {
                if (col->r[2] > 5) // from blue (ish)
                {card = 2;} // green card          
            }

            else if ( ((abs((int)(col->g[1] - blue.g[1])) < tol*2) //confirming
                    & (abs((int)(col->g[2] - blue.g[2])) < tol*2)) //from red
                    & (abs((int)(col->b[1] - blue.b[1])) < tol*2) ) //from green & blue
            {card = 3;} // blue card 

            else if ( ((abs((int)(col->r[0] - yellow.r[0])) < tol*3) //confirming
                    & (abs((int)(col->g[2] - yellow.g[2])) < tol*3)) //from red, orange
                    & (abs((int)(col->g[1] - yellow.g[1])) < tol*2)) //confirming
            {card = 4;} // yellow card 
            
//            else if ( ((abs((int)(col->r[2] - pink.r[2])) < tol*2) //from blue
//                    & (abs((int)(col->g[2] - pink.g[2])) < tol)) //from orange, red
//                    & (abs((int)(col->b[1] - pink.b[1])) < tol) ) //from blue
//            {card = 5;} // pink card 
//            
//            else if ( ((abs((int)(col->b[0] - orange.b[0])) < tol*2) //from blue
//                    & (abs((int)(col->b[1] - orange.b[1])) < tol*2)) //from blue
//                    & (abs((int)(col->r[0] - orange.r[0])) < tol) ) //from blue, yellow
//            {card = 6;} // orange card 
//
//
//            else if ( ((abs((int)(col->g[1] - lblue.g[1])) < tol*2) //confirming
//                    & (abs((int)(col->g[2] - lblue.g[2])) < tol)) //from orange, red
//                    & (abs((int)(col->b[1] - lblue.b[1])) < tol) ) //from orange, pink, blue, red
//            {card = 7;} // light blue card 
        }
       
    };
    return(card);
}


/************************************
 * Function from start to finish find the colour of a card and store it to memory
 * includes dealing with 'non card' 0 readings
************************************/
char card_routine(struct colour *col, char mode, struct DC_motor *mL, struct DC_motor *mR, char stage_count, struct memory *m)
{
    char card=0;    // set card to be a 'non card'
    char attempts=1; // set number of attempts to successfully get a card reading
    
    while(attempts < 6) // loop until a card reading is achieved or too many attempts
    {
        color_read(col); //populates structure 
        color_cal(col); //interpolates reads between black and white
        card = card_identify(col, mode); //assigns card a number
        
        //if reading didn't work shuffle the car before next loop
        for (char i=0; i<4; ++i)
        {       
        if (card==0) {dc_motor_shuffle(attempts, mL, mR); attempts +=1;}
        else {break;}       // if a reading is achieved break out of loop
        }
    }
    memory_card(m, card, stage_count);  // write card into memory
    return card;
}