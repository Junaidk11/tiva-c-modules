#include "TM4C123.h"                    // Device header

int main()
{
	// Goal - Blink Board LED at 1Hz - once per second, with Systick Timer
	// Creating accurate delays as compared to previous nested loop delay
	// Configure GPIO for the LED
	SYSCTL->RCGCGPIO |= 0x20;
	GPIOF->DIR = 0x0E;
	GPIOF->DEN = 0x0E;
	
	// Systick Config
	SysTick->LOAD = 16000000-1; // Minus 1 because we start count at 0
	SysTick->CTRL = 5; // Enable SysTick, no interrupt, and use System Clock 
	
	// Now we only need to check if the counter has timed out
	// The way to check that: Check if timeout flag has been set - do this in the while loop. 
	
	while(1)
	{
		// check if COUNT flag is set
		if(SysTick->CTRL & 0x10000)
			GPIOF->DATA ^=2; // '^' corresponds to the toggle operator
	}
}
