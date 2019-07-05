#include "TM4C123.h"                    // Device header

// The goal of this project 
// is to configure GPIO interrupt
// mainly, the onboard switches will be used 
// to invoke an interrupt signal to the CPU(i.e. MCU)
// the MCU will handle the Interrupt signal by loading the Program Counter
// register on to the stack and load the register with the address
// of the interrupt handler associated with the switch that was pressed
// Alos we need to unlock Switch1 - Recall unlocking pins project
void delayMs(int delay);
int main()
{
	SYSCTL->RCGCGPIO |=0x20; // Enable clock access to GPIO portF module
	
	// Unlocking PF0 before we configure PORTF
	GPIOF->LOCK = 0x4C4F434B; // Special LOCK Register value for Unlocking 
	GPIOF->CR =0x01; // Set Commit Register to make PF0 configurable
	GPIOF->LOCK=0; 
	
	//Now Configure PORTF for switch and LED
	GPIOF->DIR &=~0x11; // Make PF4 and PF0 as input pins - switches
	GPIOF->DIR |= 0x0E; // Set all my LEDS as output pins
	GPIOF->DEN |= 0x1F; // Set all the pins as Digital 
	// Next we need to setup Pull up resister for PF0 and PF4; Pull-up means, the pin is normally high
	// and when switch actuated, the pin is low.
	GPIOF->PUR |= 0x11; // enable pullup for PF4 and PF0
	
	// Next we disable interrupts globally - ALWAYS whenever setting up interrupts
	__disable_irq();
	// We configure PF4 and PF0 as Fallen Edge interrupt; i.e. Interrupt invoked 
	// when the pins associated with the switches go from HIGH to LOW; i.e. we set Pull-up resistor 
	// for pins PF0 and PF4
	GPIOF->IS &=~0x11; //Make PF4 and PF0 edge sensitive
	GPIOF->IBE &=~ 0x11; //Interrupt not triggered on both edges - for both pins PF0 and PF4
	GPIOF->IEV &=~ 0x11;  //Trigger interrupt from PF0 and PF4 only on the Falling Edge - i.e. PF0 and PF4 are 
												//configured as Pull-ups, they're normally high, but when their corresponding switches
												//are actuated, the pins go LOW - Hence Falling Edge, triggers the interrupt.
	GPIOF->ICR |= 0x11; // Clear any prior interrupt 
	GPIOF->IM  |= 0x11; // Unmask the interrupt
	
	// Now we need to enable the interrupt in the NVIC and set the priority
	
	// Our GPIO PortF is IRQ[30]
  NVIC->IP[30] = 3<<5; // Set Interrupt from GPIO PORTF at a prioity of 3 - from 0 to 7 Scale // Can't find this Register
	NVIC->ISER[0] = 0x40000000; //Enable IRQ30
	__enable_irq(); // enable gloabl interrupt after you've configured the interrupt - important
	while(1)
	{
		// Just toggling RED LEDs in the while loop
		GPIOF->DATA |=0x02; 
		delayMs(100);
		GPIOF->DATA &=~0x02;
		delayMs(100); 
	}  
}
// Next we set the 'Call-back' function for the GPIOF port

void GPIOF_Handler(void)
{
	// In here we toggle the GREEN LED whenever the Interrupt is invoked 
	// by the switches on PF0 and PF4
	
	for (int i=0; i<3;i++)
	{
		GPIOF->DATA |= 0x08;
		delayMs(100);
		GPIOF->DATA &=~ 0x08;
		delayMs(100);
	}
	GPIOF->ICR |= 0x11; // Clear interrupt flag before return
}


void delayMs( int delay) // Copied from Timer0A Project
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
