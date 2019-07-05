#include "TM4C123.h"                    // Device header

void timer1ADelaySec(int delay);
int main()
{
	SYSCTL->RCGCGPIO |= 0x20; // Enable Clock Access to the PORTF GPIO module
	GPIOF->DIR |= 0x0E; // Set all the LEDs as output
	GPIOF->DEN |= 0x0E; // Set all the GPIOF LED pins as Digital
	while(1)
	{
		GPIOF->DATA ^= 0x02; // Toggle the RED LED
		timer1ADelaySec(2); // Use our 'Seconds' Delay function to create a delay of 1 Second 	
	}
	// The output shows the RED LED blinking at 1 second (i.e. toggling at 1Hz) Delays
  // Change the funxtion argument and you will notice the toggling of the
	// the LED at different frequencies depending on the argument. 
}

void timer1ADelaySec(int delay)
{
	SYSCTL->RCGCTIMER |= 0x02;  // enable timer access to Timer1 block
	TIMER1->CTL =0; // Disable Timer1 first
	TIMER1->CFG =0x04; // Select 16-bit mode for Timer
	TIMER1->TAMR = 0x02; // Select periodic-mode 
	TIMER1->TAILR = 64000-1; // TimerA delay generator with a prescaler of 250 
	TIMER1->TAPR = 250-1; //TimerA prescaler register loaded with 250-1; 
  TIMER1->ICR = 0x1; //Clear timerA Interrupt register
	TIMER1->CTL |= 0x01; //Finally, Enable TimerA
	
	for(int i =0; i<delay; i++)
	{
		while((TIMER1->RIS & 0x1)==0){} //While Timer hasn't timedout, you stay in this loop
	  TIMER1->ICR = 0x1; // Acknowledge the Timeout by clearing the Interrupt register of Timer1		
	}	
}