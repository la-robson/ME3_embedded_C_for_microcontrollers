#include <xc.h>
#include "timers.h"

/************************************
 * Function to set up timer 0
************************************/
void Timer0_init(void)
{
    T0CON1bits.T0CS=0b010; // Fosc/4
    T0CON1bits.T0ASYNC=1; // see datasheet errata - needed to ensure correct operation when Fosc/4 used as clock source
    T0CON1bits.T0CKPS=0b1000; // 1:256 counting 1 second cycles 
    T0CON0bits.T016BIT=1;	//16 bit mode	
	
    // initialise registers for 1 second cycles 
    TMR0H=0b00001011;            //write High reg first, update happens when low reg is written to
    TMR0L=0b10111010;
  
    T0CON0bits.T0EN=1;	//start the timer
}

/************************************
 * time_to_hour
 * Function to update the hour, checks if the time is a multiple of
 * the hour factor 
************************************/
void time_to_hour(char *hours, char *hourCheck)
{
    // increment the hour if an hour has passed     
    if (time%hourFactor == 0 & *hourCheck == 0 ) {*hours += 1; *hourCheck = 1;}
    if (time%hourFactor != 0) {*hourCheck = 0;}
}

/************************************
 * update_date
 * Function to reset the time and hours and update the date and dayOfWeek
************************************/
int update_date (char *hours, int date, char *dayOfWeek)
{
    if (*hours > 23 )                           //check if it is midnight  
    {                       
            *hours = 0;                         // reset the hour to 0
            time = 0;                           // reset the time to 0
            date += 1;                         // increment the date
            // increment the day of the week
            if (*dayOfWeek<7){*dayOfWeek += 1;}   
            else {*dayOfWeek = 1;}
    } 
    return date;
}

/************************************
 * year_update
 * Function to update the year and leapYear, and reset the date
************************************/
int year_update (char *leapYear, int date)
{
    // if at the end of a standard year or a leap year 
    if ((date==366 & *leapYear!=4) | (date==367 & *leapYear==4)) 
    {
            // if at end of a leap year reset date and leapYear
            if (*leapYear == 4) {date=1; *leapYear=1;}
            // reset the date to 1 and update leapYear
            else {date=1; *leapYear+=1;}
    }
    return date;
}

/************************************
 * daylight_savings
 * Function to apply daylight savings on the right days
************************************/
void daylight_savings (char *hours, int date, char dayOfWeek, char *savings)
{
    // Check date for clocks going forward
    if ((dayOfWeek==7) & (date>=84 | date<91) & (*hours==1)) 
    { 
        *hours += 1;         // move hours forward 1
        time += hourFactor; // move time forward 1
        *savings = 0;       // update savings tracker
    } 
    // Check date for clocks going back
    if ((dayOfWeek==7 & (date>=298 | date<305)) & (*hours==2) & (*savings==0)) 
    { 
        *hours -= 1;         // move hours back 1
        time -= hourFactor;  // move time back 1 hour
        *savings = 1;        // update savings tracker 
    }
}

/************************************
 * time_drifting
 * Function to compare the solar noon to the theoretical noon 
 * if the time has drifted by more than +- half an hour the time 
 * will get corrected
************************************/
void time_drifting (char *noonCheck, char savings)
{
    if (day==0 & *noonCheck==0)      // if night and not checked time yet
    {   
        // define variables to check for time drifting
        // solar noon values are all double to avoid division and decimal values
        int noonUpper = 25*hourFactor;  // upper bound for solar noon
        int noon = 24*hourFactor;       // expected solar noon
        int noonLower = 23*hourFactor;  // lower bound for solar noon
        int solarNoon = dusk+dawn;      // calculate solar noon
        
        if (savings == 0)           // check if in summer
        {             
            solarNoon += -2*hourFactor;  // correct for daylight savings
        }
        
        *noonCheck=1;                     // update check variable
        
        if (noonLower < solarNoon | noonUpper > solarNoon)
        {
            time += (noon - solarNoon);  // if time has drifted correct it 
        }                      
    }
}