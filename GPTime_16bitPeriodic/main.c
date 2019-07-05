#include "TM4C123.h"                    // Device header

void timer0ADelayMS( int delay);
int main()
{
	SYSCTL-> RCGCGPIO |= 0x20; // Enable Clock acces to GPIO PORTF; Our LEDs are here;
	GPIOF->DIR |= 0x0E; // 0x0E sets all the LEDs as Outputs // Note: '|=' is friendly programming way; Ensures only desired bits are manipulated; You could use '=' - not Friendly/Dangerous
	GPIOF->DEN |= 0x0E; // 0x0E configures all LED pins as Digital 
	
	while(1)
	{
		GPIOF->DATA ^= 0x02; //Toggle the RED LED
		timer0ADelayMS(1000); // Toggle the RED LED with 50 MS periodically.
	}
	// The output is RED LED blinking periodically every 50 millisecond; The shivering of the LED is because of the periodic TIMER
	// As you increase the DELAY, the shivering decreases
	// Also, the delay achieved via these Timers is more accurate 
	// As compared to the Delay through nested-loops; try 50 ms, 100 ms, 1000 ms - note the blink rate
}

// Periodic Timer instead of the One-shot timer

void timer0ADelayMS( int delay)
{
	// First we enable clock for the GPTM;
	// GPTM = General Purpose Timer Module;
	SYSCTL -> RCGCTIMER |= 1; // 1= Timer0;
	// Disable Timer0 before configuration
	TIMER0->CTL =0;
	// Next we select Timer mode as 16-bit Timer
	TIMER0->CFG = 0x04; // 4 = 16-bit timer mode selected
	// Next we select Timer operation; i.e One-shot/Periodic/Up Counter/ Down Counter
	TIMER0->TAMR = 0x02; // 0x02 means Periodic mode
	TIMER0->TAILR = 16000-1; // 16000 clock cycles is 1 millisecond; NOTE: We didn't use the Delay parameter as compared to One-shot operation;
	TIMER0->ICR = 0x1; // Clear the Interrupt Status Register
	TIMER0->CTL |= 0x01; // Finally Enable Timer0;
	
	for(int i=0;i<delay; i++)
	{
		//The timeout i.e. the flag is set EVERY SECOND;
		//The Timer A Interval Load Register is loaded with a value
		//equivalent to 1 milliseond for a Bus clock of 16 MHz, 
		//Therefore, this for loop will run for every millisecond and for every
		// millisecond, the while loop will wait till the Timeout flag is set by TimerA
		// So, if delay is 10millisecond, the for loop iterates 10 times,
		// for each interation, the counter counts down from 16000-1 to 0;
		while((TIMER0->RIS & 0x1)==0){} // Wait for the TimerA to timeout and set its Flag
		//Once the flag is set, you acknowledge it by clearing the interrupt status again; 
		TIMER0->ICR=0x1;
	}
	// Later we write a MORE efficient way to achieve periodic Timer instead of using the LOOPS	
}
