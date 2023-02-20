/*	FSM_Up_Down_Walking_One_v2
*	===================================
*
*	Function:               Preform an interrupt upon the detection of an event (a button press)
*	Circuit Schematic No.:  1 : 
*	Required Libraries:     
*
*	Authored by:            Dr Craig Evans
*	Date:                   
*	Collaberators:          Andrew Knowles
*				Dr Tim Amsdon
*	Version:                1.0
*	Revision Date:          02/2023 
*	MBED Studio Version:    1.4.1
*	MBED OS Version:        6.12.0
*	Board:	                NUCLEO L476RG	*/

#include "mbed.h"
 
// defines directions as 0/1. Note UPPERCASE
#define UP 0
#define DOWN 1
 
//led colours    R      G      B
BusOut LEDS(PA_15, PA_14, PA_13); //define output bus pins 

void init_buttons();
void init_led();

InterruptIn Joystick_button(PC_12);
 
// struct for state
struct State {
    int LEDS;  // output value for current state
    int time;  // wait time for state (ms)
    int next_state[2]; // next state (depending on direction 0 - UP, 1 - DOWN)
};
 
// array of states in the FSM, each element is the output of the counter, wait time, next state(s)
State g_fsm[4] = {
    {0b000,500,{1,3}},
    {0b001,500,{2,0}},
    {0b010,500,{3,1}},
    {0b100,500,{0,2}},
};
 
// flag - must be volatile as changes within ISR
// g_ prefix makes it easier to distinguish it as global
volatile int g_Joystick_button_flag = 0;
 
// Joystick push-button interrupt service routine
void Joystick_button_isr();
 
int main(){
	init_buttons();
	init_led();
	
    // The joystick push-button has a pull-up resistor, so the pin will be at 3.3 V by default
    // and fall to 0 V when pressed. We therefore need to look for a falling-edge
    // on the pin to fire the interrupt
    Joystick_button.fall(&Joystick_button_isr);
 
    // set inital state
    int state = 0;
	
    // set initial direction
    int direction = UP;
 
    while(1){  // loop forever
        // check if flag i.e. interrupt has occured
        if (g_Joystick_button_flag){
            g_Joystick_button_flag = 0;  // if it has, clear the flag
            // swap direction when button has been pressed
            direction = !direction;
        }
 
        // set output of current state (use dot syntax to access struct members)
        LEDS = g_fsm[state].LEDS;
		
        // implement required delay
        ThisThread::sleep_for(500ms);
		
        // set the next state depending on direction
        state = g_fsm[state].next_state[direction];
    }
}
 
// Joystick push-button event-triggered interrupt
void Joystick_button_isr(){
    g_Joystick_button_flag = 1;   // set flag in ISR
}

void init_buttons(){
    // Set the joystick push-button to use an internal pull-up resistor
    Joystick_button.mode(PullUp);
}

void init_led(){
    LEDS.write(0);   //turn off all leds by writing the decimal equivalent of 0b111
}
