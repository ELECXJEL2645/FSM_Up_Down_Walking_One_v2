/*  FSM Up Down counter Example
*   Operate an RGB LED using a finite state machine with a button input 
*	where the input from the button changes the direction of the FSM.
*
*   Board: NUCLEO L476RG
*   Author: Dr Craig Evans
*	Editied: Andrew Knowles 2022
*	MBED OS: 6.14
*/

#include "mbed.h"
 
// defines directions as 0/1. Note UPPERCASE
#define UP 0
#define DOWN 1
 
//led colours    R      G      B
BusOut RGB_LED(PA_15, PA_14, PA_13); //define output bus pins 

void init_buttons();
void init_led();

InterruptIn buttonA(PC_12);
 
// struct for state
struct State {
    int RGB_LED;  // output value for current state
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
volatile int g_buttonA_flag = 0;
 
// Button A interrupt service routine
void buttonA_isr();
 
int main(){
	init_buttons();
	init_led();
	
    // Button A has a pull-down resistor, so the pin will be at 0 V by default
    // and rise to 3.3 V when pressed. We therefore need to look for a rising edge
    // on the pin to fire the interrupt
    buttonA.rise(&buttonA_isr);
 
    // set inital state
    int state = 0;
	
    // set initial direction
    int direction = UP;
 
    while(1){  // loop forever
        // check if flag i.e. interrupt has occured
        if (g_buttonA_flag){
            g_buttonA_flag = 0;  // if it has, clear the flag
            // swap direction when button has been pressed
            direction = !direction;
        }
 
        // set output of current state (use dot syntax to access struct members)
        RGB_LED = g_fsm[state].RGB_LED;
		
        // implement required delay
        ThisThread::sleep_for(500ms);
		
        // set the next state depending on direction
        state = g_fsm[state].next_state[direction];
    }
}
 
// Button A event-triggered interrupt
void buttonA_isr(){
    g_buttonA_flag = 1;   // set flag in ISR
}

void init_buttons(){
	// since Button A has an external pull-down, we should disable to internal pull-down
    // resistor that is enabled by default using InterruptIn
    buttonA.mode(PullNone);
}

void init_led(){
    RGB_LED.write(0);   //turn off all leds by writing the decimal equivalent of 0b111
}