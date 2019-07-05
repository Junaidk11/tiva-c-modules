//PF4 GPIO SW1
//PF0 GPIO SW2
//PF1 GPIO RGB LED (Red)
//PF2 GPIO RGB LED (Blue)
//PF3 GPIO RGD LED (Green)

#include "TM4C123.h"                    // Device header

int main()
{
	SYSCTL->RCGCGPIO |= 0x20; // Enable Clock Access to port F
	GPIOF->DIR |= 0x08; // This will set pin3(Green LED) to Output 
  GPIOF->DEN |= 0x18; // Sets both Green LED and SW1 as Digital pins
	GPIOF->PUR |= 0x10; // Sets SW1 pin as PULL-UP because SW1 is active-low, i.e. normally its at HIGH
  while(1)
	{
		if(!(GPIOF->DATA & 0x10)) // Bit-wise AND with SW1 bit to check if SW1 pressed
		{
			GPIOF->DATA |= 0x8; // SW1 was pressed, we turn on green LED
		}
		else
		{
			GPIOF->DATA &=~ 0x8; // SW1 not pressed, so turn off green LED
		}
	}

}


