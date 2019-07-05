#include "TM4C123.h"                    // Device header

// The goal of this project:
// To implement the Third most common application 
// of Timers: Measuring Time between events.
// The other applications are: Creating Delays
// and Counting number of events. 

// The measuring time between events will 
// be implemented for an application - the ultraSonic Sensor. 

// UltraSonice has a Trigger pin - output pin
// an Input pin-Called Echo, 
// 5V power pin
// and ground pin. 

// Operation: The trigger pin sends an output signal
// The Echo pin recieves the signal when it bounces off an 
// obstacle and goes HIGH, the time elapsed 
// when Echo pin is HIGH to when it goes LOW is multiplied
// by a CONSTANT (derived from MCU period and Sound speed - LOOK it up at cortex-m.com)
// UltraSonic usually takes a fixed amount of time(Check DATASHEET)
// for Setting its output pin(Trigger pin)
// therefore, you would need to wait after changing Trigger pin output for THAT specific amount

// To improve readability, we employ SYMBOLIC variables for UltraSonic pins

#define ECHO (1U<<6) // PB6
#define TRIGGER (1U<<4) // PA4(Output)
#define BLUE_LED (1U<<2) // PF2 onboard Blue LED

// We need to create 4 individual functions
// 1: We need to create a function to initialize Timers to capture the time between Edges of the Echo pin
// 2: We need to create a Delay function to create a delay of Microseconds - The UltraSonic time delay for changing Ouput pin
// 3: We will create a function to initialize our PORT rather than doing it in the main function
// 4: We will write a function to calculate the distance of the object infront of our ultraSonice Sensor


uint32_t highEdged;
uint32_t lowEdged;

uint32_t distanceCm; // store distance in centimeters
const double _16Mhz_1clock = 62.5e-9; //
const uint32_t MULTIPLIER = 5882; // Derived from Speed of Sound for multiplication
// with the difference in timerstamps captured

void Timer0_init(void); // Function 1
void delay_Microsecond(uint32_t time); // Function 2
void Ports_Init(void); // Function 3; Initializing the PORTS
uint32_t t_measuredD(void); // Function 4

uint32_t distance_in_cm; // To stored distance measured return value; 

int main()
{
	//Initialize our PORTS first - Trigger and BLUE_LED
	Ports_Init();
	// Next, initialize Timer0;
	Timer0_init();
	// Next we blink the BLUE_LED,
	// when we detect an object that is within 15 cm of the UltraSonice
	while(1)
	{
		distance_in_cm = t_measuredD();
	    if(t_measuredD()<15)
				{
					GPIOF->DATA |= BLUE_LED; // Turn on LED
				}
	}
}

void Timer0_init(void) // Function 1
{
	SYSCTL->RCGCTIMER |= (1U<<0); // We use Timer0 because we're interested in capturing the Edges of the 
	// the Echo pin ( when the Signal from the Trigger bounces back and is read Echo). As we defined above, Echo
	// is going to be connected to PB6. Therefore, to capture Edges (i.e. Falling-edges) of PB6, we use TIMER0 
	// as Timer0 is connected to PB6. 
	// The goal is to start the Timer0 when PB6 goes HIGH(Trigger signal recieving) and 
	// stop the Timer0 when PB6 goes LOW. 
	
	// Start by configuring the GPIO PORTB pin 6
	SYSCTL->RCGCGPIO |= (1U<<1); // Provide Clock access to GPIO PORTB; Because ECHO connected to PB6
	GPIOB->DIR &=~ECHO; // Set ECHO pin as Input
	GPIOB->DEN |= ECHO; // Enable ECHO pin as Digital 
	GPIOB->AFSEL |= ECHO; // We Enable the Alternate Function of PB6 because we're using the TIMER0 on this pin 
	// Next, we need to configure PB6 as a Capture Compare PWM pin; Two step - 'T0CCP0' - Recall the Capture operation of Timers
	GPIOB->PCTL &=~ 0x0F000000; // This code is available in the Datasheet
	GPIOB->PCTL |=  0x07000000; // This code is availabe in the Datasheet
	
	// Now we configure the Timer Module itself
	// Before we start configuration we ALWAYS disable the Timer first
	TIMER0->CTL &=~1;// same as =0x00; but the method uncommented is friendly way
	TIMER0->CFG |= 4; // 16-bits mode configure
	TIMER0->TAMR = 0x17; // We set the characteristics of the Timer: We enable the Timer as Edge-time mode(we want the time betwee edges),capture mode and as an up-counter
	TIMER0->CTL |= 0x0C; // TAEVENT bit to both edges; i.e. detect event at both edges - faling and rising
	TIMER0->CTL |= 1; // // Finally, Enable the TIMER0;
}

// Next we implement the Function to create a MicroSecond Delay

void delay_Microsecond(uint32_t time) // Function 2
{
	// First, we enable clock access to Timer Module 1 because we used Timer0 in
	// measuring Time between Echo edges
  SYSCTL->RCGCTIMER|= (1U<<1);
	// Disable Timer before making any changes
	TIMER1->CTL |= (1U<<1);
	// Configure Timer1 as a 16-bit Timer
	TIMER1->CFG |= 4;
	// Configure operation mode of Timer1
	TIMER1->TAMR =0x02; // TIMER1 set as Periodic time mode 
	// Load the interval value to create a Microsecond delay 
	TIMER1->TAILR = 16-1; // 16 because we're running MCU at 16MHz
	// Next, we clear the Interrupt Register for any previous Interrupts that might've been set
	TIMER1->ICR = 0x1;
	// Finally, we enable the Timer1
	TIMER1->CTL |=1;
	
	// Finally, we poll and wait for the Timeout to occur - using the loop 
	
	for( int i= 0; i<time; i++)
	{
		while((TIMER1->RIS & 0x1)==0)
		{} // Waiting for Timer1 timeout Flag to be set;
		// Once the Flag is set, you acknowledge the flag by clearing the Interrupt
		// Clear Register;
		TIMER1->ICR =0x1;
	}
}


void Ports_Init(void) // Function 3; Initializing the PORTS
{
	SYSCTL->RCGCGPIO |= (1U<<0); // Enable Clock Access to GPIO Port A because Trigger in
	// connected to PA4
	SYSCTL->RCGCGPIO |= (1U<<5); //Enable Clock Access to GPIO Port F; For the Blue LED - Just to see some Outpu
	GPIOB->DIR |= TRIGGER; // Set Trigger pin as Output
	GPIOF->DIR |= BLUE_LED; // Set Blue_LED pin as Output
	GPIOA->DEN |= TRIGGER; // Enable Trigger pin as Digital
	GPIOF->DEN |= BLUE_LED; // Enable Blue_LED as Digital 
}

uint32_t t_measuredD(void) // Function 4
{
	// Before you calculate the Distance, you DISABLE the Trigger pin
	// And remember to give the Delay time to your UltraSonic Sensor to 
	// Disable the Trigger pin
	
	GPIOA->DATA &=~TRIGGER; // This DISABLES the Trigger pin
	delay_Microsecond(12); // We're assuming our ultraSonic needs 10MicroSeconds to disable the pin ; so we give 12 Microseconds just to be sure
	GPIOA->DATA |= TRIGGER; // Enable the Trigger pin again 
  delay_Microsecond(12); // Give the UltraSonic Sensor time to make the change
	GPIOA->DATA &=~TRIGGER; // Disable the Trigger pin again
	
	// The code above is HOW you send the ultraSonic Wave; you trigger the pin(After you've diabled first and waited)
	// wait for 12 microseconds (check datasheet of your UltraSonic Sensor)
	// Disable trigger again and wait for the ECHO pin to recieved the bounced wave
	
	// Next, you capture the FirstEdge i.e. the Rising edge  at the ECHO pin
	
	// But, first you clear the timer capture flag by writing 1 to 'CAECINT' bit in ICR of Timer0
	TIMER0->ICR =4; 
	// Next, we poll the RIS of TIMER0 until the timer capture flag is set for second edge(i.e. falling) of ECHO
	while((TIMER0->RIS & 4)==0){} // Wait till captured, by checking if CAERIS bit (bit 2) is high
	
			// Once the timer capture flag is set - i.e. Rising-edge captured - i.e. bounced wave recieved
			// all you do is store the Time Stamp of the Capture in a variable
			highEdged = TIMER0->TAR; // highEdged = the time at which last edge event took place
					// NOTE: Also, TAR stores the 'TIME' between Edges because we configured TIMER0 to 
					// operate as Edge-time mode NOT as Edge-count mode(counts the number of edges)
				
									// Capture second edge i.e. Falling edge
		
									//Next, we get the time for the Falling Edge as the previous one was a Rising Edge
									//Before we do that, we need to clear the Interrupt set for when TIMER0 captured an edge, to allow
									//the next edge to be detected
										TIMER0->ICR =4; // Clear the timer capture flag, to allow detection of next edge
										while((TIMER0->RIS & 4) ==0){} // wait for the next timer capture flag to be set  i.e. the Falling edge to be detected
												
														// Once the timer capture flag is set, read the timestamp from the TAR register of TIMER0 because
														// TIMER0 was configured as Edge-time mode ; i.e. to measure time to an edge
												lowEdged = TIMER0->TAR; 		
		
											     distanceCm = lowEdged - highEdged; // Since lowEdged occured the latest, you find the difference between
											     // between the timestamps to get the distance to the object
			                     distanceCm = _16Mhz_1clock *(double) MULTIPLIER * (double)distanceCm; // Where did you get this formula from???
											return distanceCm; 
}