//PF4 GPIO SW1
//PF0 GPIO SW2
//PF1 GPIO RGB LED (Red)
//PF2 GPIO RGB LED (Blue)
//PF3 GPIO RGD LED (Green)

#include "TM4C123.h"                    // Device header

void delayMS( int ms);
int main()
{
	//First we need Clock Access
	SYSCTL->RCGCGPIO |= 0x20; //: Enable clock access to PORTA : 0010 0000 - in Binary Note: Zeros before '2' in '0x20' can be omitted
	GPIOF->DIR |= 0x02; // 0000 00010 // Setting Port F as Output Direction - Red LED
  GPIOF->DEN |= 0x02; // After setting direction, we need to Enable the PORTF to be Digital 
	//GPIOF->DATA |=0x02; // After enabling, we need to write to the Pin to blink LED, Sets pin 1 of portF at High
	while(1)
	{
		 GPIOF->DATA |= 0x02; // After enabling, we need to write to the Pin to blink LED, Sets pin 1 of portF at High
	   delayMS(100);
		 GPIOF->DATA &=~ 0x02; // The friendly way to set a particular pin to Low without affecting the rest of the register
	   delayMS(100);
	}
}

//Learn Accurate Delays in the Future with Event Capture
void delayMS( int ms)
{
	int i, j;
	for(i=0;i<ms;i++)
	{
		// 3180 was derived Empirically, through Experiments
		for(j=0;j<3180;j++)
		{}
	}
}