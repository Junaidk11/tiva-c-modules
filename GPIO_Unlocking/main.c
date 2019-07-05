//PF4 GPIO SW1
//PF0 GPIO SW2
//PF1 GPIO RGB LED (Red)
//PF2 GPIO RGB LED (Blue)
//PF3 GPIO RGD LED (Green)

#include "TM4C123.h"                    // Device header


// PF0 is locked by-default, so we need unlock it
// Extra Steps other than enabling the pin

int main()
{
	SYSCTL->RCGCGPIO |= 0x20; // Enable Clock Access to GPIO Module
	// To unlock the pin, we need to access the LOCK register
	// and write a particular 32 bit value to this register to gain access 
	// to that particular pin. 
	// The value is 0x4C4F434B in order to unlock PF0
  GPIOF->LOCK = 0x4C4F434B; 
	
	// Following the step above, before we can configure 
	// the PF0, we need to write to CR (commit Register) register to enable the bit
	// of PF0
	GPIOF->CR |=0x01; // Make PF0 configurable
	
  // Now we can do the usual steps to configure
	
	GPIOF->DIR = 0x08; // Setting PF3 as Output - Green LED
	GPIOF->DEN = 0x09; // Sets both PF0 and PF3 as digital Pins
	GPIOF->PUR = 0x01; // SW0 is active-lo; i.e. normally high

	while(1)
	{
		if(!(GPIOF->DATA & 0x01))
			GPIOF->DATA |= 0x08;
		else
			GPIOF->DATA &=~0x08;
	}
	//Comments: When you remove the Unlocking code for PF0
	// The LED still lights up, 
	// that is because the PF0 in its locked state is at 0
	// Hence, when the pin is read in the while loop,
	// the if statement is executed. 
	// However, once the pin is unlocked, its state is 
	// controlled by the push action on the switch. 

}

