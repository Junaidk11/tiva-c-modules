#include "TM4C123.h"                    // Device header

// Symbolic names for readability

#define LCD GPIOB //For Display
#define RS    1   //0x01 : 0000 0001 ; PB0
#define RW    2   //0x02 : 0000 0010 ; PB1
#define EN    4   //0x04 : 0000 0100 ; PB2  

// For 4 bit data, we only use the pins PB7, PB6,  PB5 and PB4 ; Watch video 
void LCD4bits_Init(void);
void delay_micro(int n);
void delay_milli(int n); // psuedo delay - not precise!
void LCD_Write4bits( unsigned char data, unsigned char control);
void LCD4bits_Cmd(unsigned char command);
void LCD4bits_Data(unsigned char data);

int main()
{
	LCD4bits_Init();
	LCD4bits_Cmd(1);
	LCD4bits_Cmd(0x80);
	delay_milli(500);
	LCD4bits_Data('H');
	
}

void LCD4bits_Init(void)
{
	SYSCTL->RCGCGPIO |= (1U<<1); // Enable clock Access to PORTB
	LCD->DIR |= 0xFF; // Set all pins as output
	LCD->DEN |= 0xFF; // Digitally Enable all the pins of PORTB
 
	
	// After initializing the Pins
	// We need to send 3 commands to the LCD - we need a command function
	// set the mode
	// set cursor to home screen
	// turn on the LCD
	// clear the LCD
	
	LCD4bits_Cmd(0x28); // set 4-bit mode, 2-lines, 5x7 fonts ; from datasheet
	LCD4bits_Cmd(0x06); // Move cursor right
	LCD4bits_Cmd(0x01); // clear the screen
	LCD4bits_Cmd(0x0F); // Turn display on
	
}

void LCD_Write4bits( unsigned char data, unsigned char control)
{
	// We are trying to break the 8-bit data from PB7 to PB0; we only need PB7, PB6,  PB5 and PB4 nibble
	// to extract that data we perform the AND operation with the 'data' argument
	// Seperate the data and the control bits as follows
	data &= 0xF0; // b7b6b5b4 b3b2b1b0 & 1111 0000 = b7b6b5b4 = data ; gets rid of the lower nibble
	control &= 0x0F; // We keep the lower 4 bits and get rid of the upper 4 data bits
 	
	LCD->DATA |= data; // write the extracted data to the 'data register of the LCD'
	LCD->DATA |= control; 
	// The two lines above can be combined as LCD->DATA = data | control; which is simply LCD->DATA = data, followed by LCD->DATA |= control; 
	// We need to secure the command/data written to the internal registers by sending a pulse - EN=1
	LCD->DATA |= EN; // Send HIGH ; E=1;
	delay_micro(0);
	LCD->DATA &=~ EN; // Send LOW; E=0;
}

void LCD4bits_Data(unsigned char data)
{
	LCD_Write4bits(data & 0xF0, RS); //upper 4 bits of data
	LCD_Write4bits(data<<4, RS);  // lower 4 bits of data
	delay_micro(40);
	
}
void LCD4bits_Cmd(unsigned char command)
{
	LCD_Write4bits(command & 0xF0, 0); // Extract the upper 4 bits of command
	LCD_Write4bits(command<<4,0); // lower 4 bits of command
	if(command<4)
	  delay_milli(2);
	else
		delay_micro(4);
}

void delay_milli(int n) // psuedo delay - not precise!
{
	for (int i=0;i<n;i++)
	{
		for(int j=0;j<3;j++) //'3' is derived from frequency of the MCU
		{}
	}
}

void delay_micro(int n) // psuedo delay - not precise!
{
	for (int i=0;i<n;i++)
	{
		for(int j=0;j<3180;j++) //'3180' is derived from frequency of the MCU
		{}
	}
}