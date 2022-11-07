#ifndef _timers_H
#define _timers_H

#include <xc.h>

#define _XTAL_FREQ 64000000

extern int time; 
extern int hourFactor;
extern int dawn;
extern int dusk;
extern char day;

void Timer0_init(void);
void time_to_hour(char *hours, char *hourCheck);
int update_date (char *hours, int date, char *dayOfWeek);
int year_update (char *leapYear, int date);
void daylight_savings (char *hours, int date, char dayOfWeek, char *savings);
void time_drifting (char *noonCheck, char savings);

#endif
