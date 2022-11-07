# Course project - Archie and Lauren's Solution

## Project objectives
Our task was to program the car to be able to do the following tasks in the 'mine' consistantly:
1. Move towards a card and stop *before* hitting it
2. Read the colour
3. Do the appropriate move, dictated by card colour (see end of readme for mine specification)
4. Navigate back to the start when the white card is found
5. Navigate back to the start if an error occurs

![Navi Diagram](gifs/maze.gif)

There are a set of constraints on the challenge, which affected our solution, the key constraints are listed below:
1. The car must be completely autonomous once it starts the maze (no button pressing or serial communication)
2. We cannot use any additional components (aside from a cardboard hood on the color click sensor)
3. We cannot control the lighting, it will be the ambient room lighting (including light from windows)

Note: There are two different mine difficulties, easy contains red, green and blue only, while hard contains all colours.

## Overview of our solution
### Summary
We achieved the key objectives by taking readings of the lighting in front of the car and using these values to determine the appropriate movement. Interrupts were used for timers and detecting light level (proximity of card). The memory was implemented by recording card readings and timings into arrays in a memory structure. Movement was achieved by combining a selection of basic movements into more complex routines for each card. Calibration routines were used to help improve consistency of movement, colour reading, interrupt threshold levels and set the route difficulty. 

Our code is broken down into functions wherever possible, and these functions are all stored in appropriately named files with the file name in the function, e.g. the function color_check is in color.c and color.h. These functions are then grouped as 'operations' in operation.c to simplify the main. 

For a 5 minute video presesentation explaining our solution [click here](https://imperiallondon-my.sharepoint.com/:f:/g/personal/lar19_ic_ac_uk/Ej6MN_bFCspFtpupZZ7LxhkB_RirhG7_2NO00CbMDhyllQ?e=dnxGbB)

Below is a more detailed description of each of the key elements in the solution.

### Movement 
All of the movement functions are made up of basic building blocks of movement such as turn right/left 45°. These base functions in the dc_motor files are combined in the movement files into the more complex movements for each coloured card, the functions can be passed 1 to do the movement, or 0 to do it in reverse. These are then combined into a single function where the card colour numbers can be passes in and the car will perform the relevant movement. 

In order to make the turns and movements consistent we used multiple techniques. Firstly we have a calibration routine that is called at the start so that you can modify the turning circle so the car performs an exact 90° turn. This works by allowing the user to +/-1 to the maximum power of the forward motor and at the same time +/-2 to the maximum power of the reverse motor in a turn. Secondly, we found that using low powers with longer delays, this means the car is slower, but avoiding rapid acceleration means there is less wheel slip, making it more consistant.

Note: Despite our best efforts, due to the open loop nature of the dc motor the car however does not always turn at exactly 45/90° and sometimes needs a hand!

### Interrupting Movement 
The interrupt works by firstly turning on the tricolour LED, the interrupt using the tricolour sensor interrupt using the high and low threshold. The theory is: all cards when approach reflects more light from the LED and therefore is brighter than ambient. As a result when a card approaches it will trigger a high threshold. Furthermore, black is the only card which causes the opposite effect therefore it is the only card which uses the low threshold of the interrupt.

A high threshold value is set with the blue card around 0.5 cm away from the cover and a low threshold value with the black card around 2 cm away (where you would expect the car to stop). Blue is selected as it is the darkest of the colours, which still causes the light levels to increase (all but black). The blue card needs to be 0.5 cm away to ensure the others stop 0.5 cm or more away. As per the requirements of the stopping before the card.

Once the interrupt triggers it turns on a global variable and temporarily disables itself. This global variable causes the car to stop, and stops the timer (see memory). Once the car has done the relevant manoeuvre, and started moving off again, the interrupt is enabled again. 

### Colour Sensing
The color click chip on the car has 4 different sensors giving red, green, blue, and clear readings. It also includes a tricolour LED that can produce combinations of red, green and blue light. 

To determine card colour we chose to take a set of colour readings for 4 different LED settings, red, green, blue and white (all three colours), these readings are stored as arrays a colour structure. Initially we converted these readings into HSV values, but we found that the variations in readings was amplified in HSV form, making ot harder to distinguish between colours, so we kept our readings in RGBC.

To improve the consistency of readings, and reduce the impact of ambient lighting we calibrated and normalise (essentially interpolating between black and white) the readings using the equations below: 

x calibrated = ((x - black reading)*100)/(white reading - black reading)

x normalised = (x*100)/clear reading

In order to identify the colour of the card we used a series of if else statements checking for each card in turn. First checking for black and white using the calibrated readings, then if not black or white normalising the readings and comparing the normalised readings to a set of mean values for each card. For each colour we selected 3 different readings to check (4 for a couple), we chose the readings with the highest consistency, and the largest difference to other colours (to avoid colours being misclassified). The spreadsheet used for this can be seen below, the left hand side being the average calibrated values and the right being the maximum difference squared between readings (variation). In the comments of the code, the comments state whether we are causing them to completely avoid other colours, or just improving the probablity the correct colour is selected, this is referred to as 'comfirming'. The final configuration of the colour check function was achieved through an iterative testing and improvement cycle.

![image](https://user-images.githubusercontent.com/92311637/146033843-c91bde3e-ab19-400d-b8f8-baf595722925.png)

### Memory
In order for the car to retrace its steps it needs to record its previous movements. We implemented a memory structure which contains two arrays, one containing card readings, the other containing time values for how long the car was driving forward between each card. 

When the card is read the colour number associated to it is input in to the memory structure. When the card starts moving (at the start of after a move) the timer is started, when the colour sense interrupt is triggered, the car is stopped (as stated above), however also the time is taken from the timer, and put in the memory structure too.

When the car has a failed reading, or reads a white or black card it will trigger reverse mode. This is triggered by changing the value of 'forward'. This gets the car to turn around (using the blue movement) and also, starts the car. I doing so it subtracts the last time recorded in the memory (i.e. the time before it hit the white / black) from 2^(16)-1 and sets it the timer. 

Once the timer overflows a global timer variable is set to 1. This causes the second last movement (in reverse) to occur. Then the same process as before happens using the second last time. This process continues to occur until the 'stage_count' (the stage the code is currently on) goes negative, at this point the car should be back to the start.

### Main logic
Below is the code used in main.c (correct as of 16:00,14/12/2021). It shows the logic used to combine all of the elements described above, not including initialising and calibration. 

	// main driving functionality 
    while(1)
    {
        if (color_interrupt==1) // if there is a proximity alert:
        {
            // stop car and record driving time in memory 
            Movement_stop_and_time(&motorL, &motorR, stage_count, &myMemory);
            
            // get card reading and assign to memory
            card = card_routine(&myColour, mode, &motorL, &motorR, stage_count, &myMemory);
            
            // execute appropriate movement 
            operations_executeCard(card, &motorL, &motorR,low_interrupt_val, high_interrupt_val, &forward);
            
            // iterate the stage count
            stage_count += 1;       
        } 
        
        //Initialising the 'reverse' movement
        if (forward==0) 
        {operations_reverseMovementSetup(&motorL, &motorR, myMemory.time[stage_count], &forward, &stage_count);}
        
        // execute reverse movement when timer interrupts 
        if ( timer_interrupt & (stage_count >= 0))
        {operations_reverseMovementExecute(&motorL, &motorR, myMemory.cards[stage_count], myMemory.time[stage_count], &stage_count);} 
        
        // if driving for 30s and fail to find card return to start 
        else if ( timer_interrupt & (forward == 1) )
        {operations_specialReverseMovementSetup(&motorL, &motorR, &myMemory, stage_count, &forward);}
        
        // if back to start stop and flash LED forever
        else if ((stage_count < 0) & timer_interrupt)
        {operations_end(&motorL, &motorR);}
        
        // check battery is above 20%, if below stop and flash LED forever
        operations_batteryCheck(&motorL, &motorR); 
    }


## Calibration routine guide
Follow the instructions below to calibrate the car, if the car is not working as expected press the red reset button and try calibrating again. 
#### 1. 90° Turn
1. RD7 LED should come on, to show you can start calibrating, press RF3 button (RH3 LED should come on and RD7 goes off)
2. Watch the car turn does it turn exactly 90 degrees?
3. If the turn was acceptable hold the RF3 button, if the turn is not good push press RF3
4. If you need to turn more press RF3, if less RF2 (you may need to press mutiple times, press until RD7 flashes)
5. Return to step 2

#### 2. Colour (Black and white card)
1. RD7 LED should come on
2. Place black coloured card in front of sensor at the distance you want to stop
3. Press RF2 button (next to LED)
4. Hold card in place until lights flash and the tricolour LED has cycled through red, green, blue and white
5. Switch the black card for the white card and repeat step 4

#### 3. Mode
1. RD7 LED should come on
2. For easy mode (red, green, blue, black, white only) press RF2 (next to on LED)
3. For hard mode (all colours) press RF3 (opposite on LED

#### 4. Interrupt
1. The front LED should turn on as white, and the RD7 LED should be on 
2. Hold the blue card 0.5cm away from the front of the sensor hood, press RF3. 
3. The RD7 LED should blink off and on again 
4. Place the black card 2cm away from the sensor hood and press RF3 again
5. Both LEDs should come on 

Line up the car, and press RF3 to get the car to go!


## "Mine" environment specification (taken from project brief)
A "mine" is constructed from black plywood walls 100mm high with some walls having coloured cards located on the sides of the maze to assist with navigation. The following colour code is to be used for navigation:

Index | Colour | Instruction
--------|---------|---------
0 |Failed reading | None (return home)
1 |Red | Turn Right 90
2 |Green | Turn Left 90
3 |Blue | Turn 180
4 |Yellow | Reverse 1 square and turn right 90
5 |Pink | Reverse 1 square and turn left 90
6 |Orange | Turn Right 135
7 |Light blue | Turn Left 135 
8 |White | Finish (return home)
9 |Black | Maze wall colour (return home)
