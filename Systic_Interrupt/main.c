#include "TM4C123.h"                    // Device header

// The goal for this project is to use 
// a systic timer to generate an interrupt at 
// a fixed rate;
// And the Interrupt Handler will blink and LED
// The interrupt handler function can be replaced
// any code you wish to execute when the interrupt
// is invoked, but for simplicity we use LED blinking

int main()
{
	SYSCTL->RCGCGPIO |= 0x20; // Enable clock access to GPIO PortF
	GPIOF->DIR |= 0x0E; // Sets all the LEDs as output
	GPIOF->DEN |= 0x0E; // Set all the LED pins as Digital 
	
	// Configure Systick 
	// Whenever you wish to set up and Interrupt or enable an interrupt
	// you FIRST disable interrupt Globally
	__disable_irq(); //This line disables Interrupts Globally
	SysTick-> LOAD = 16000000 -1; // 16000000 is th eload register value because we want to generate interrupt every second; i.e. frequency of 1Hz
	SysTick->CTRL = 7; // The seven is 0x7, 0111, this Enables Systick, selects/use Interrupt, and use system clock
	__enable_irq(); // Now we enable interrupts
	
	while(1)
	{} // the while loop is empty because
		// The blinking will take place in the SysTick_Handler()
		// as that is the Interrupt Service Routine 
		// for an interrupt invoked by the SysTick timer at timeout
		// The System automatically knows the Handler and you don't give any 
		// arguments as it is a call-back function
	
		// The output is the RED LED blinking every second when the Interrupt is invoked 
		// at SysTick Timer timeout
}

void SysTick_Handler() // This is the Interrupt Handler for the Interrupt invoked by the SysTick Timer
{
	 //Perform your Code you desire at the Interrupt invoked
	
	GPIOF->DATA ^=0x02; // For simplicity we toggle the RED in the Interrupt Handler
											// invoked by the SysTick Timer
}