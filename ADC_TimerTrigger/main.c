#include "TM4C123.h"                    // Device header

// The goal of this project
// To configure ADC with a Timer Trigger
// that reads on-board Temperature Sensor
// at 1 Hz.
// We're going to use Sample Sequencer 3 
// to read input from Temperature sensor
// ss3 takes 1 sample from ADC input channel 


volatile int temperature; // Store ADC value

int main()
{
	// Enable Clock to WTimer and ADC module
	
	SYSCTL->RCGCADC |=0x01; // Enable Clock access to ADC module 0;
	SYSCTL->RCGCWTIMER |= 0x01; // Enable clock access to WTimer 
	
	// Initialize ADC first
	ADC0->ACTSS &=~8; // Disable Sample Sequencer first! SS3
	ADC0->EMUX &=~0xF000; // Disable software trigger for ADC conversion - The old stuff
	ADC0->EMUX |= 0x5000; // Enable Timer Trigger to start conversion at ADC0; - The new stuff
  ADC0->SSMUX3=0; // Get input from Channel 0
  
  // Next we set flag at First sample as we're using SS3- which only takes one sample
  ADC0->SSCTL3 = 0x0E; // Take MCU temperature, set flag at 1st sample
  // Now we enable the ADC0, by enabling the Sample Sequencer 3
	ADC0->ACTSS |= 8; 
	
	// Initialize the WiderTimer (WTimer) now
	WTIMER0->CTL =0; // Disable the Timer before you initialize it
	WTIMER0->CFG |=0x04;  //Select the 32-bit option for your WTimer
	WTIMER0->TAMR |= 0x02; // Select Peiodic and Down-Counter operation mode
	WTIMER0->TAILR = 16000000; // Since we want 1 Hz trigger period, i.e. ADC Triggered every second
	WTIMER0->CTL |= 0x20; //Next we select WTIMER to trigger ADC at timeout i.e. Timer Trigger ADC option 
	WTIMER0->CTL |= 0x01; // Finally, we Enable TimerA 

  // Next we start Conversion and wait for the Conversion to complete
	
	while(1)
	{
		while((ADC0->RIS & 8)==0){} // Wait till ADC0 conversion has been completed
		temperature  = 147 - (247*ADC0->SSFIFO3)/4096; //This equation converts the ADC value to the Temperature. 
		ADC0->ISC = 8; // Acknowledge you read the converted value by clearing completion flag to be notified for next sample conversion
	}
	
     
 	
	
}
	 