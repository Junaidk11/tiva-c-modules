#include "TM4C123.h"                    // Device header

// The goal of this project:
// To configure TIMERS to count the 
// occurence of an event using 
// Capture Compare PWM (CCP0 - For TIMERA or CCP1 - Form TIMERB)
// at rising-edge.

void timer3A_RisingEdgeEvent_init(void);
int timer3A_RisingEdgeEvent_capture(void);
uint32_t number_of_events =0 ; // A variable to store the number of events captured by the TIMER3A

int main()
{
	  
   	// Start with GPIO and Timer Initilization
	  // Both done in a single Function
	  timer3A_RisingEdgeEvent_init(); 
	  while(1)
		{
			number_of_events=timer3A_RisingEdgeEvent_capture();
		}

}

void timer3A_RisingEdgeEvent_init(void)
{
	// Start with enabling clock access to Timer block 3
	SYSCTL->RCGCTIMER |= 0x08; 
	// Enable clock to GPIO PORTB - because Timer3 of TimerA is connected to PORTB pin 2
	SYSCTL->RCGCGPIO |= 0x02; 
	
	// First we Configure/Initialize the GPIO pin PB2 
	// Next, we set the direction of PB2 as input 
	GPIOB->DIR &=~ 0x04; 
	// Next, we set PB2 as a digital pin
	GPIOB->DEN |= 0x04;
	// Next, we need to enable the Alternate function of PB3 
  GPIOB->AFSEL |= 0x04; 
  // Next, we need to configure PB2 as : Timer 3 of TimerA Capture Compare PWM pin i.e T3CCPO
  // In order to configure PB2 as T3CCP0, the datasheet gives a code that needs to loaded into 
	// 'PCTL' register specifically for PB2 to be confgiured as a Capture Compare PWM pin
	// the code is '0x00000F00' 
	GPIOB->PCTL &=~ 0x00000F00; // Configuring PB2 as T3CCP0 pin 
	GPIOB->PCTL |= 0x00000700; 
	// End of GPIO Initialization
	
	//Start of Timer Initialization
	// FIRST - disable Timer before configuration
	TIMER3->CTL = 0x00; 
	// Next, we configure TIMER3 as 16-bit timer mode
	TIMER3->CFG = 0x04; 
	// Next, we configure the operation mode of Timer3 - i.e periodic/one-shot, count-up/count-down, etc - read datasheet
	TIMER3->TAMR = 0x13; //'TAMR'  TImerA Mode Register: We selected as periodic, up-counter, edge-count, and capture mode
	// Next, we set the count-limit - i.e. count 10 occurence, 20 occurence etc...
	TIMER3->TAMATCHR = 0xFFFF; //set the count limit, compared to 'TAR' to determine if the event occured??
	//'TAMATCHR' - TimerA Match Register - i.e this register stores the maximum count possible
	// We can use the 8-bit prescaler to increase the Maximum count - i.e. 24-bits all together
	TIMER3->TAPMR = 0xFF; // used with TAMATCHR to expand to 0xFFFFFF - 24-bit register with the prescaler
	// Finally, we Enable the Timer3
  TIMER3->CTL |= 1; // Enable Timer3A
	
}
	
// The following function '' collects the event'' - this 
// function captures the event

int timer3A_RisingEdgeEvent_capture(void)
{
	// This function returns the value stored in the 'TAR' register
	// The value in the TAR register is compared with the TAMATCHR register
	// to determine if an event as occured.
	number_of_events = TIMER3->TAR;
	return number_of_events; //compared with TAMATCHR to determine match event  ???
}

	
	
	
	