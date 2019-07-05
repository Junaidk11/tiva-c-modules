#include "TM4C123.h"                    // Device header

// The goal of this project is to configure a 
// wide timer - i.e. 32/64-bits Timers

#define BLUE (1U<<2) // Here we're defining the BLUE LED Symbolicaly

void wtimer1A_delaySec(int sec); // Our 32-bit Wide Timer Delay Function

int main(void)
{
	SYSCTL->RCGCGPIO |= (1U<<5); // Enable Clock Access to GPIO PORTF Module
	GPIOF->DIR = BLUE;  //Set PortF Direction register
	GPIOF->DEN = BLUE;  //COnfigure the BLUE LED pin as a Digital pin 
	
	while(1)
	{
		GPIOF->DATA ^= BLUE; //Toggle Blue
		wtimer1A_delaySec(1); //Create a delay of 10 seconds
	}
	
	//Next we're going to write the Timer function for a Delay created
	//by 32/64-bits Wide Timer - the second group of Timers
	//First group of timers are the 16/32-bits Timer
	// By default Group 1 Timers are 16-bit timers
	// By default . Group 2 Timers are 32-bit Timers
	// Use the Configuration Register to initialize the Timers 
	// as 32-bits or 64-bits respectively. 
}
void wtimer1A_delaySec(int sec)// 'wtimer1A' = Wide Timer1 A to create a delay in Seconds
{
	
	// We're using Wide Timer1, so we enable clock access to Wide Timer module
	SYSCTL->RCGCWTIMER |= 0x02; //Enable clock access to 'WTimer Module 1' ; Usually its 'SYSCTL->RCGCTIMER' thats 16/32-bits timer
  
	// The rules here are the same as the ones used for 16/32-bit timers
	// i.e. before you make any changes, you need to disable the timer;
	WTIMER1->CTL =0;  //disable WTimer before initialization of WTimer;
	// Next we set the operation mode for the Timer
	// Recall/Note: By default the Wide Timer, operates as a 32-bit timer
	// To configure the Wide Timer as 64-bit timer, set Configuration Register to 0, instead of 4, check DataSheet
	WTIMER1->CFG =0x04; // Configure the Wide Timer as a 32-bit Timer - The Default for Wide Timers
	WTIMER1->TAMR = 0x02; // Configure the TimerA Mode as Periodic mode and Down Counter; Check Datasheet for One-shot mode configuration
	WTIMER1->TAILR = 16000000 -1; // We set the Interval Load Register - this register generates the Delay; Recall: 16Million value creates a delay of 1 second - From Calculation
	
	//Next you enable the WTimerA after initialization
  WTIMER1->CTL |=0x01; // Enable WTimer A after initialization
	
	//Use a loop to create 'sec' seconds delay using your WTIMER1
	for(int i =0; i<sec; i++)
	{
		while((WTIMER1->RIS & 0x1)==0) // Wait for you Wide Timer1 to timeout AND sets Flag
		{}
		WTIMER1->ICR =0x01; // Once the Flag is set, i.e. WTIMER1 timedout, you acknowledge the Flag by clearing it;
	}
}