#include "TM4C123.h"                    // Device header

// The Goal of this project
// is to combine 16-bit TimerA and TimerB 
// to use them as a 32-bit timer and create 
// a Delay in Seconds
// '1U<<2' is: Unsigned value 1 shifter to
// the left by 2 bits; i.e. 1U<<2 = 4
// 1U<<0 = 1, 1U<<1 = 2 etc

#define BLUE (1U<<2); // Define Blue LED

void DelaySecond(uint32_t delay); // Concatinated 32-bit Timer Function
int main()
{
	SYSCTL->RCGCGPIO |= (1U<<5); // Enable Clock Access to GPIO PORTF module
	GPIOF->DIR |= BLUE; //Set Blue LED pin as Ouput
	GPIOF->DEN |= BLUE; //Set Blue LED pin as a Digital pin
	
	while(1)
	{
		// Test the concatenated Timer0 
		GPIOF->DATA ^=BLUE; //Toggle our BlUE LED pin
    DelaySecond(10); //Create a Delay of 10 seconds 		
	}
	// The output shows the BLUE LED toggling at 10 Seconds 
	// with Timer0, which is a 16-bit Timer
	// BUT it is configured as a 32-bit timer 
	// by setting a bit in its Control Register
	// Refer to the Datasheet for the Control Register
}

void DelaySecond(uint32_t delay) 
{
    SYSCTL->RCGCTIMER |= 0x01; // Enable Clock Access to Timer0 Module
	  TIMER0->CTL |= 0x00; // Disable Timer0 before configuration
	  // The Datasheet states that
	  // to concatinate Timers, simply write '0' to the Configuration register
	  // of your Timer
	  TIMER0->CFG = 0; // Configure Timer0 for a 32-bit timer mode
	  TIMER0->TAMR= 0x02; // Set the Timer operation as Periodic and Down counter
	  TIMER0->TAILR = 16000000-1; // For a Delay of a second Delay
	  TIMER0->ICR = 0x01; // clear timer0 timeout flag 
	  TIMER0->CTL |= 0x01; // Enable our Timer0;
	
	  // Now we use the Loop as before with
	  // our delay argument to create the 
	  // delay as desired
	
	for(int i =0;i<delay;i++)
	{
		while((TIMER0->RIS & 0x01)==0)//Wait till the timer is timedout
		{}
		TIMER0->ICR = 0x01; // Acknowledge the Timedout Flag 
	}
}