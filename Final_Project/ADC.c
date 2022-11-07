#include <xc.h>
#include "ADC.h"

/************************************
 * ADC initialising for battery sense, using F6 as input
************************************/
void ADC_init(void)
{
    TRISFbits.TRISF6=1; // Select pin F6 as input
    ANSELFbits.ANSELF6=1; //Ensure analogue circuitry is active

    // Set up the ADC module - check section 32 of the datasheet for more details
    ADREFbits.ADNREF = 0; // Use Vss (AS Vss is 0V) as negative reference (AD_Negative_Ref)
    ADREFbits.ADPREF = 0b00; // Use Vdd (As Vdd is 3.3V) as positive reference (AD_Positive_Ref)
    ADPCH=0b101110; // Select channel RA3/ANA3 for ADC
    ADCON0bits.ADFM = 0; // Left-justified result (i.e. no leading 0s)
    ADCON0bits.ADCS = 1; // Use internal Fast RC (FRC) oscillator as clock source for conversion
    ADCON0bits.ADON = 1; // Enable ADC
}

/************************************
 * Gets the value from the ADC, and outputs 1 if low battery (below 20%), outputs 0 otherwise
************************************/
char ADC_getval(void)
{
    unsigned int tmpval;
    char low=0;
       
    ADCON0bits.GO = 1; // Start ADC conversion

    while (ADCON0bits.GO); // Wait until conversion done (bit is cleared automatically when done)
        
    tmpval = ADRESH; // Get 8 most significant bits of the ADC result - if we wanted the 
	// full 10bit result we would also need to read the 2 LSBs from ADRESL and combine the result.

    tmpval = (tmpval * 20) / 51; //Converting to percentage (taken factor of 5 out, 255 max)
    
   if (tmpval <= 20){low = 1;} //If battery less than 20 low = 1

    return low; 
}
