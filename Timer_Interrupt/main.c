#include "TM4C123.h"                    // Device header

// The goal of this project:
// Implement Two Timer interrupts
// and visualize how they work

// One Interrupt at 1Hz - i.e. the Interrupt Service Routine for this Timer's interrupt
// will occur once every second.

// Second Interrupt at 10 Hz - i.e. the Interrupt Service Routine for this Timer's interrupt
// will occur 10 times in a second;

// We're going to use all 3 LEDs
// One of the LED's will run in the main function
// the other two will be used in the ISR of each respective timer's interrupt

// We give symbolic names to each LED 

#define GREEN (1U<<3) // Pin 3
#define RED (1U<<2)   // Pin 2
#define BLUE (1U<<4)  // Pin 4 

void timer1A_1Hz_Init(void);
void timer2A_10Hz_Init(void);


int main(void)
{ 
							/* Initializing our LED pins */
	SYSCTL->RCGCGPIO |= 0x20; //Enable clock access to PORTF
	GPIOF->DIR = 0x0e; // Set all LED pins as output
	GPIOF->DEN = 0x0e; // Select all LED pins as Digital pins
	
	 /* Initialize the Timers */
	
	timer1A_1Hz_Init();
	timer2A_10Hz_Init();
	
	// NOTE: YOU NEED TO ENABLE INTERRUPTS GLOBALLY ; i.e. In your main function
	// without the enabling of interrupts globally, interrupts will not take place in your program
	__enable_irq(); // This function enables interrupts globally
	
	while(1)
	{
		GPIOF->DATA^=BLUE; //The BLUE LED will be toggled in the main function
	}
		
}
					/* Setup Timer1A for an interrupt at 1Hz */
void timer1A_1Hz_Init(void)
{
	SYSCTL->RCGCTIMER |= 0x02; // Enable clock access to Timer 1 Block 
	TIMER1->CTL=0;          // Disable Timer1 before configuration - ALWAYS
	TIMER1->CFG |= 0x04;   // Set Timer1 as 1 16-bit Timer
	TIMER1->TAMR = 0x02;  // Select Timer1 operation mode as; Periodic and down-counter
	TIMER1->TAPR = 250; 	// We know 16-bit Timer cannot give 1 second delay so we use an 8-bit prescalar to get 1 Second delay; A value o
											  //of 250 will give us the necessary value to get 1 Second delay
	TIMER1->TAILR = 64000; // 64000 clock cycles give us 1 second; Which is the Interrupt signal period
  TIMER1->ICR =0x01; //clear TIMER1A timeout flag
	// New Register 
	TIMER1->IMR |= 0x01; // The 'Interrupt Mask Register' enables Interrupt invoking for TIMER1A at its timeout 
	TIMER1->CTL |=0x01; // Finally, we enable the Timer1
	
	// Next we need to Enable the Interrupt
	// For that, we go to the Nested Vector Interrupt Controller and select the Interrupt Number for TIMER1A 
	// The datasheet gives you the IRQ number for different peripherals, you load the desired interrupt 
	// value to the NVIC's ISER[0] register
	NVIC->ISER[0] |=0x00200000; // Enables IRQ21 (D21 of ISER[0]) - which corresponds to Interrupt of timer1A	
}
	
					/* Setup Timer2A for an interrupt at 10Hz */
void timer2A_10Hz_Init(void)
{
	SYSCTL->RCGCTIMER |= 0x04; // Enable clock access to Timer 2 Block 
	TIMER2->CTL=0;          // Disable Timer2 before configuration - ALWAYS
	TIMER2->CFG |= 0x04;   // Set Timer2 as 1 16-bit Timer
	TIMER2->TAMR = 0x02;  // Select Timer2 operation mode as; Periodic and down-counter
	TIMER2->TAPR = 250; 	// 16000000 Hz/250 = 64000 Hz
	TIMER2->TAILR = 6400; // 64000 clock cycles give us 1 second; You want a delay of 10seconds = (64000[clock cycles/second] )/(6400 [clock cycles])
  TIMER2->ICR =0x01; //clear TIMER2A timeout flag
	
	// New Register 
	TIMER2->IMR |= 0x01; // The 'Interrupt Mask Register' enables Interrupt invoking for TIMER2A at its timeout 
	TIMER2->CTL |=0x01; // Finally, we enable the Timer2A
	
	// Next we need to Enable the Interrupt
	// For that, we go to the Nested Vector Interrupt Controller and select the Interrupt Number for TIMER1A 
	// The datasheet gives you the IRQ number for different peripherals, you load the desired interrupt 
	// value to the NVIC's ISER[0] register
	NVIC->ISER[0] |=0x00800000; // Enables IRQ23 (D23 of ISER[0]) - which corresponds to Interrupt request of timer2A	
}





// Note: There is a Standard way to to write Interrupt Handlers - it allows the CPU to find the Interrupt Handler when an Interrupt is invoked by the peripheral 

// Note: For Timer1A - TIMER1A_Handler(void), and you don't need to put the prototype above the main function for Interrupt Handler functions

					/* Timer1A Interrupt Handler */ 

void TIMER1A_Handler(void)
{
    // The code below will be executed every second - the interrupt is invoked at 1Hz; 
	
	  // First you check if the timeout flaf of TIMER1A has been set in the Masked Interrupt Status Register -'IMS'
	
	 volatile int clear; // Sometimes flag doesn't clear, to be sure, we can read the ICR register of TIMER1 and it will clear the register - its a precaution
	  if(TIMER1->MIS & 0x1)
		{
			GPIOF->DATA ^=RED; // We will Toggle RED for simplicity and proof of concept; BUT we could do anything in this thread; i.e Run a motor, send a message etc. 
			// After we've doen our desired function, we clear interrupt flag to detect next interrupt
			TIMER1->ICR = 0x1; //Clear the TIMER1A timeout flag
			clear = TIMER1->ICR; // a read to force clearing of interrupt flag
		}
		else
		{
			// It rarely happens that the flag is not set; 
			// so you clear ALL the flags as below
					TIMER1->ICR = TIMER1->MIS;
		
      // Just a precaution, you read ICR to force clearing TIMER1 interrupt flag
      clear=TIMER1->ICR;
		}
}

void TIMER2A_Handler(void)
{
    // The code below will be executed every second - the interrupt is invoked at 10Hz; 
	
	  // First you check if the timeout flaf of TIMER2A has been set in the Masked Interrupt Status Register -'IMS'
	
	 volatile int clear; // Sometimes flag doesn't clear, to be sure, we can read the ICR register of TIMER2 and it will clear the register - its a precaution
	  if(TIMER2->MIS & 0x1)
		{
			GPIOF->DATA ^=GREEN; // We will Toggle GREEN for simplicity and proof of concept; BUT we could do anything in this thread; i.e Run a motor, send a message etc. 
			// After we've doen our desired function, we clear interrupt flag to detect next interrupt
			TIMER2->ICR = 0x1; //Clear the TIMER1A timeout flag
			clear = TIMER2->ICR; // a read to force clearing of interrupt flag
		}
		else
		{
			// It rarely happens that the flag is not set; 
			// so you clear ALL the flags as below
					TIMER2->ICR = TIMER2->MIS;
		
      // Just a precaution, you read ICR to force clearing TIMER2 interrupt flag
      clear=TIMER2->ICR;
		}
}