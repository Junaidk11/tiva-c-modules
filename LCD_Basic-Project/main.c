#include "TM4C123.h"                    // Device header

// Flow chart in the course for a Basic LCD Program 
void LCD_init(void);
void LCD_Cmd(unsigned char command);
void delay_micro(int delay);
void delay_milli(int delay);
void LCD_Data(unsigned char data); // the argument is the data we wish to write to the LCD Display


																							// Notes//
// The LCD internally has two registers
// one for command and one for Data
// Setting RS=0; gives access to the command reigster of LCD; Hence, you can write commands to the LCD.
// setting RS=1; gives access to the data register of LCD; Hence, you can write data to the LCD.
// Pb7 to PB0 are the Data pins for this program- as per the video
// Therefore, when you set RS=0; you're getting access to the COMMAND Register of the LCD,
// you use the DATA register of GPIOB to load the COMMAND register ( After you set RS=1).
// Similarly, when you set RS=1; you're getting access to the 'DATA register of the LCD',
// you use the 'DATA register of GPIOB' to load the 'DATA register of the LCD'. 

// Check Code and watch the video again
int main()
{
	LCD_init(); 
	LCD_Cmd(0x01); //0x01 is the command for clearing the screen ; From Datasheet 
	LCD_Cmd(0x08); //Sets the cursor home; From Datasheet;
	delay_milli(500); // Allow the LCD to execute the commands
	LCD_Data('C');
	delay_milli(1);
	LCD_Data('M');
	delay_milli(500);
}

void LCD_init(void)
{
	SYSCTL->RCGCGPIO |= 0x01; // enable clock for portA - we select control pins of LCD on portA ; RS, RW and E controls
	SYSCTL->RCGCGPIO |= 0x02; // Enable clock for portB - we select the Data pins on Pb0 to PB7
	GPIOA->DIR |= 0xE0; // Sets PA7, PA6 and PA5 pins as output
	GPIOA->DEN |= 0xE0; // Enable pins above as Digital 
	GPIOB->DIR |= 0xFF; // Set PB7 to PB0 as output 
	GPIOB->DEN |= 0xFF; // Enable PB7 to PB0 as Digital output pins
	
	// The commands can be known from the LCD's datasheet - watch the LCD video
	LCD_Cmd(0x38); // 8-bits, 2 line displayu, 5x7 font
	LCD_Cmd(0x06); // increment automatically
	LCD_Cmd(0x0F); // Turn on display 
  LCD_Cmd(0x01);  //Clear display
	
}
void LCD_Data(unsigned char data) // the argument is the data we wish to write to the LCD Display
{
	// Recall: In order to send data to the LCD, we need to set the RS to 1; RS=1;
	// setting RS=1, allows the internal register of the LCD to access the Data register 
	
	GPIOA->DATA |= 0x20; // Set RS pin to 1; RS=Register Select, RS of LCD connected to PA5; RS=1, E=0, RW=0; ob 0010 0000
	GPIOB->DATA = data; //  Once you've selected the 'data' register of the LCD, you write your argument to the DATA reigster of GPIOAB; because according
	// to video we connected PB7 to PB0 as the data pins of the LCD data register 
	
	
	// After you write to the LCD's data register, you send a pulse for a brief moment to secure the Data ; i.e. Set E=1
	GPIOA->DATA |= 0x80; // 0b 1010 0000 ; Sets PA7 to 1; i.e. E=1. RS=1 RW=0; Always perform FRIENDLY Operation b/c you don't want to change the entire register by mistake!
	delay_micro(0); // Create a delay to allow the LCD to take carry out the function
	GPIOA->DATA &=~0x80; // Set the Pulse to LOW; E=0 RS=1 RW=0;
	delay_micro(0); // Create a delay to allow the LCD to take carry out the function
}
void LCD_Cmd(unsigned char command)
{
	// To write commands to LCD, you need to RS to 0
	// which will allow the internal reigster to access the command register and find the desired command
	// After sending the Command, send a pulse using 'E' which secures the data in the internal register
	// Followed by the pulse is a Delay for the command to be executed by the LCD - we need the
	// delay because the MCU is MUCH faster than the LCD. Therefore, to let the LCD catch up, we need to create the delay
	// we will create a pesudo delay but we can use Timers for more precise delays
	
	GPIOA->DATA |= 0x00; // This sets RS =0, RW=0 and E=0; RW=0 allows to write to the LCD, RS=0 allows to write Command
  GPIOB->DATA = command;  // The command we want to write to the Command register of the LCD; We selected PB7 to PB0 as our DATA pins;
	GPIOA->DATA |= 0x80; // After sending the command, we send the pulse by enabling 'E' i.e. sets E=1 to secure command
	delay_micro(0); // Create the delay needed for the specific command to be executed by the LCD
	GPIOA->DATA &=~ 0x80; // Set E=0, to set the pulse as LOW
	
	if(command<4)
		delay_milli(2);
	else
	delay_micro(37); 
}

void delay_micro(int delay)
{
	// A psuedo delay - not very accurate
	int i, j;
	for(i=0;i<delay;i++)
	{
		for(j=1;j<3;j++)
		{}
	}	
}
void delay_milli(int delay)
{
    int i, j;
	for(i=0;i<delay;i++)
	{
		for(j=1;j<3180;j++)
		{}
	}
}	
