#include "TM4C123.h"                    // Device header

// The goal of this project
// Implementation of the ADC peripheral to
// read an analog signal 


// Sample Sequencers of Tm4c123gh6pm
// ss0, ss1, ss2, ss3 ; each one has a associated FIFO
// ss0 can hold 8 samples because the FIFO that it is associated with has a DEPT of 8; what do you mean??
// ss1 can hold 4 samples
// ss2 can hold 4 samples 
// ss3 can hold 1 sample 

// In this project we employ ss3
// because the analog signal is taken from
// a single channel 

// To select the desired Sample Sequencer
// we need to configure the ADC ACTive Sample Sequencer register
// i.e. ADCACTSS and set the appropriate bit to select our sample sequencer

// The ADC conversion can be started through many ways
// e.g. using Timers
// using PWM
// using analog comparator 
// External signal from GPIO
// Software trigger 
// The ADC trigger type can be set 
// by configuring  ADCEMUX register

// After selecting the ADC trigger type
// we need to start configure the 
// ADCPSSI register to start the conversion
// everytime there is a new reading from the 
// ADC input channel 

// If you want ADC, you can access it using the 
// pins shown below; this can be achieved from the 
// Data sheet of your MCU

/* For tm4c123gh6pm 
AIN0 ADC input 0 PE3
AIN1 ADC input 1 PE2
AIN2 ADC input 2 PE1
AIN3 ADC input 3 PE0

AIN4 ADC input 4 PD3
AIN5 ADC input 5 PD2
AIN6 ADC input 6 PD1
AIN7 ADC input 7 PD0

AIN8 ADC input 8 PE5
AIN9 ADC input 9 PE4

AIN10 ADC input 10 PB4
AIN11 ADC input 11 PB5
*/

// The end of conversion of an ADC
// can be determined by reading a particular 
// bit in the ADCRIS register, the ADC module 
// sets a flag to inform the completion
// of a conversion. 
// Once the flag in ADCRIS is set, 
// we can read the value from the 
// particular sequencer, 
// either ss0, ss1, ss2 or ss3
// depending which one you're using 

// Instead of polling the ADCRIS to determine
// the completion of ADC conversion, 
// we can configure an interrupt to be invoked
// upon completion of the conversion and the 
// data can be read automatically from the ADCSSFIFO. 

// After reading the converted data from the ADCSSFIFO
// register, we MUST clear the corresponding sequencer
// flag in the ADCRIS so that another conversion can 
// be detected. 
// Failing to clear the ADCRIS flag, would prevent the 
// detection of next conversion. 


// In this program we're using ADC input channel 0 which
// is connected to PE3
// AND we're going to use sample sequencer ss3 because we
// need just 1 channel and ss3's FIFO has a size of 4
// which is sufficient for 1 channel 

// We need to enable clock access to the 
// GPIO port E module and the ADC0 or ADC1 module
// Next, we need to select Alternative function of 
// the pin PE3 - which is the ADC channel 0 


int conversionResult; // variable to hold the value of conversion 

int main()
{
	// First we need to enable clock access to the ADC module 
	// Clock Access to the ADC module is achieved
	// via the RCGCADC and set bit 0 or bit 1 or both to
	// give clock access to ADC module ADC0 or ADC1 is set to 
	
	
	SYSCTL->RCGCGPIO |= 0x10; // Enable clock for PORT E
  SYSCTL->RCGCADC |= 0x01; // Enable clock access to ADC module 0 
  
	// PE3 Initialization for Analog input
	GPIOE->AFSEL |= 8; // Enable Alternate function at pin PE3 because '8' = 0x08; corresponds to 0000 1000
	GPIOE->DEN &=~8; // Disable digital function at pin PE3
	GPIOE->AMSEL |= 8; // Select Analog mode for pin PE3
	
	//Initialize the ADC now
	ADC0->ACTSS &=~8; // Always disbale the Sample Sequencer, in this case it is SS3
	ADC0->EMUX &=~ 0xF000; //Configure Software trigger to start ADC conversion; Check Datasheet, it says clear bit 0xF000 selects Software trigger;
	ADC0->SSMUX3 =0; // Get input signal from channel 0; 
	// Following the input at channel 0, we take 1 sample at a time and set flag at 1st sample because we only have 1 sample - because ss3 can hold only 1 sample
	ADC0->SSCTL3 |=6; // Take one sample at a time, set flag at 1st sample
	ADC0->ACTSS |=8; // Finally, we enable the ss3 to take samples
	
	// Now we start the conversion 
	// and wait for the completion flag to be set. 
  
  while(1)
	{  
		ADC0->PSSI |=8; // Start conversion at SS3;
		//Next we wait for conversion to complete
		while((ADC0->RIS & 8)==0){} // wait for completion flag to be set
		conversionResult = ADC0->SSFIFO3; // SSFIFO3 is the associated FIFO for SS3; the converted value is stored in FIF03; Sample sequencers store sample from ADC channels and FIFOs store the converted result
		ADC0->ISC =8; // Clears the 'conversion complete' flag; allows the next sample conversion to be detected		
	}			
		
	
}
	
	
	
	
	
	
	 