#include <xc.h>
#include <stdlib.h>
#include "memory.h"

/************************************
 * Store the forward driving time in the memory
************************************/
void memory_time(struct memory *m, unsigned int curr_time, char stage_count)
{
    m->time[stage_count] = curr_time;
}

/************************************
 * Store the card colour in the memory
************************************/
void memory_card(struct memory *m,char card, char stage_count)
{
    m->cards[stage_count] = card;
}
