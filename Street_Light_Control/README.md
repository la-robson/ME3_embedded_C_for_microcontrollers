# My Solution:
This project contains the soloution to the problem defined in the assignment.

### Goals
The following goals were defined in the project brief: 
1. Monitors light level with the LDR and turns on an LED in low light conditions (i.e. night-time) and off in bright conditions (i.e. daytime)
1. Displays the current hour of day on the LED array in binary
1. Turns the light off between approx. 1am and 5am
1. Adjusts for daylight savings time
1. Maintain synchronicity with the sun indefinitely
1. Be fully automatic (requires zero maintenance after installation)

These goals needed to be achived in consice well structured code. 

### Approach
##### Key funcions:
 - I employed an LDR to detect if it is day or night, using an interrupt to toggle the day night status. In this case it is calibrated so that night is the LDR covered by a hand and day is the LDR in ambient light, in reality it should be calibrated using daylight with some tolerance for cloudy weather. In this interupt the dawn and dusk variables are assigned values.
 - The time is tracked using the inbuilt clock, each 1 second cycle increases the time variable by 1, this is is used by the hour variable that increases every 'hourFactor' number of seconds, there is also a date variable tracking the number of days that have passed, a day of the week variable and a leap year variable. 
 - There is an LED array to display the time in hours in binary.
 - The date variables are used so that daylight savings time is appropriately added
 - There is a function calculating the solar noon to check for time drift using the dawn and dusk variables. This verifys that solar noon aligns with noon with a ±0.5'hourFactor' tolerance, and corrects if it is more than this. 
 -  The program can operate in two different modes by changing the hourFactor variable, testing mode where an hour lasts 2s and the operating mode where an hour lasts a full 60 minutes. 
 -  I also utilised an LCD display which shows the time in hours, the day of the year and the day/night status to aid clarity when testing the system. This was not required in the project brief but it made the testing process significantly simpler as it is easier to read and can show more information. 

I employed global variables to define the test/operating state, and to define the hourFactor, the time in seconds, the day/night status and the dawn and dusk times, all other variables were defined locally within their functions. I set the clock to cycle every 1s so that there is greater resolution in my time measurement, and if the time is corrected because the solar noon has drifted too far, it will be corrected with greater accuracy. 

I have structured the code so that the majority of the code is in functions in their relavent file, e.g. the date_update function which updates the date and resets the time is in the timers file. Many of these functions utilise pointers in addition to the few global variables, this is neccesary as many functions modify multiple variables but it is only possible to return one. 

### Testing
As stated above there is a testing mode where an hour lasts 2s. When in testing mode I could try out a range of senarios by adjusting the initial values. I tested daylight savings and leap years by adjusting the starting dates. The LCD screen displaying the time, date and day/night status (1=day, 0=night) was very helpful in the debugging process, as it was much clearer what values the variables held. 

Below is a link to a video showing a test cycle of 1 day:

https://imperiallondon-my.sharepoint.com/:f:/g/personal/lar19_ic_ac_uk/En-BuQeS0xBCgDuct7YIYGABL8E5LdIr0zlEFDMRT5oxCw?e=cNoDFH 

