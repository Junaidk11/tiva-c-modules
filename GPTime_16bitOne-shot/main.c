#include "TM4C123.h"                    // Device header

// Creating a precise delay, using GPTimers

void timerA0DelayMS(int delay);
int main()
{
	 SYSCTL->RCGCGPIO |= 0x20; //Enable Clock acess to GPIO Module on PORTF
	 GPIOF->DIR |= 0x0E; // Turning on ALL LEDs of PortF
	 GPIOF->DEN |= 0x0E; // Enable the pins of PortF as Digital 
	 
	while(1)
	{
		GPIOF->DATA ^=2; // Use the Toggle operator; Toggles PortF pin 1?
		timerA0DelayMS(50); // A 50 Millisecond Delay;
	}
	//The Result after uploading to the board is 
	// The LED barely blinks, its ON all the time
	// This is because the delay function in the While loop
	// is a One-shot timer, after first timeout, the Function call
	// is simply not executed i.e. it is as if the function call is 
	// Commented
	// Hence, the only code in the loop after first timeout is
	// 'GPIOF->DATA ^=2;' which explains why the LED is not Blinky at ALL
}

void timerA0DelayMS(int delay) // We configure the Timer as a function, and then use it in the Main Function;
{
	// First we need to Enable clock acces to Timer module
	SYSCTL->RCGCTIMER |=0;
	//We need to disable timer before we do anything with it
	TIMER0->CTL =0;
	// Now we set the timer Configuration, i.e. 16-bit or 32-bit etc
	TIMER0->CFG =0x04; // 16-bit mode
	// Next we decide if we want to operate in periodic or one-shot mode
	TIMER0->TAMR = 0x01; //one-shot mode, down-counter
	// Next we decide the delay amount 
	// For Millisecond delays with Bus clock of 16MHz; Recall the Calculations;
	// The 16000 above is the number of clock cycles equivalent to a Millisecond for a 
	// Bus Clock at 16 MHz; Delay is the number of Milliseconds dealy you desire
	// Minus 1 because we begin count at '0' - Refer to notes
	TIMER0->TAILR = 16000*delay-1; 
	// Next we clear the Interrupt status for TimerA
	TIMER0->ICR = 0x1;
	// Now we enable Timer 
	TIMER0->CTL |= 0x01;
	
	//Next we check the Raw Interrupt Status to	
	//check Interrupt Trigger when the TIMER0 has 
  // Timed out; i.e. Finished count; Resulting in 
	// Flag being set in RIS register
	while((TIMER0->RIS & 0x1)==0)
  {}	// 1 AND 1 = 1(Flag set; Timedout); 1 AND with 0 = 0 (Flag not Set); 
}