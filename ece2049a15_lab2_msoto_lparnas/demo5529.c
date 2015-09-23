#include <msp430.h>
#include <stdint.h>
#include "inc\hw_memmap.h"
#include "driverlibHeaders.h"
#include "CTS_Layer.h"
#include "grlib.h"
#include "LcdDriver/Dogs102x64_UC1701.h"
#include "peripherals.h"

// Define global variables
long stringWidth = 0;

int led_on;
// Function prototypes for this file
void swDelay(char numLoops);
int counter = 0;

enum states
{
	WelcomeScreen,
	GameStarting,
};

struct notes
{
	int frequency;
	float duration;
	char LED;
};

void BuzzerOn(int pitch)
{
	// Initialize PWM output on P7.5, which corresponds to TB0.3
	P7SEL |= BIT5; // Select peripheral output mode for P7.5
	P7DIR |= BIT5;

	TB0CTL  = (TBSSEL__ACLK|ID__1|MC__UP);  // Configure Timer B0 to use ACLK, divide by 1, up mode
	TB0CTL  &= ~TBIE; 						// Explicitly Disable timer interrupts for safety

	// Now configure the timer period, which controls the PWM period
	// Doing this with a hard coded values is NOT the best method
	// I do it here only as an example. You will fix this in Lab 2.
	TB0CCR0   = pitch; 					// Set the PWM period in ACLK ticks
	TB0CCTL0 &= ~CCIE;					// Disable timer interrupts

	// Configure CC register 3, which is connected to our PWM pin TB0.3
	TB0CCTL3  = OUTMOD_7;					// Set/reset mode for PWM
	TB0CCTL3 &= ~CCIE;						// Disable capture/compare interrupts
	TB0CCR3   = TB0CCR0/2;
	// Configure a 50% duty cycle
}

/*
 * Disable the buzzer on P7.5
 */
void BuzzerOff(void)
{
	// Disable both capture/compare periods
	TB0CCTL0 = 0;
	TB0CCTL3 = 0;
}

//Configure A Clock
void runtimerA2(void)
{
	TA2CTL = TASSEL_1 + MC_1 +ID_0;
	TA2CCR0 = 164; 	//164 +1 ACLK tics = 5/1000;
	TA2CCTL0 = CCIE;
}

#pragma vector = TIMER2_A0_VECTOR
__interrupt void count_down_ISR(void)
{
	counter++;
	if (counter ==200){led_on = 1;}
	if (counter ==400){led_on = 2;}
	if (counter ==600){led_on = 3;}
	if (counter ==800)
	{
		led_on = 4;
		counter = 0;
	}
}

char configButtons()
{
	char var = 0;
	//Push button are on P1.7 and P2.2
	//logic 0 indicates button pressed
	P1SEL = P1SEL & ~(BIT7);
	P1DIR = P1DIR & ~(BIT7);
	P1REN = P1REN | (BIT7);
	P1OUT = P1OUT | (BIT7);

	P2SEL = P2SEL & ~(BIT2);
	P2DIR = P2DIR & ~(BIT2);
	P2REN = P2REN | (BIT2);
	P2OUT = P2OUT | (BIT2);

	//1 is button 1
	//2 is button 2
	if (P1IN == 0)
	{
		var = 1;
	}
	else if (P2IN == 0)
	{
		var = 2;
	}
	else if (P1IN == 0 & P2IN == 0)
	{
		var = 4;
	}
	return var;
}

char configLEDs(char inBits)
{
	//LEDs 1-3 are P1.0, P8.1, P8.2
	P1SEL = P1SEL & ~(BIT0);
	P1DIR = P1DIR | (BIT0);

	P8SEL = P8SEL & ~(BIT1|BIT2);
	P8DIR = P8DIR | (BIT1|BIT2);

	P1OUT &= ~(BIT0);
	P8OUT &= ~ (BIT1|BIT2);

	switch (inBits)
	{
		case 0:
			P1OUT |= (BIT0);
			P8OUT |= (BIT1|BIT2);
		break;
		case 1:
			P1OUT |= (BIT0);
		break;
		case 2:
			P8OUT |= (BIT1);
		break;
		case 3:
			P8OUT |= (BIT2);
		break;
	}
	return inBits;
}

enum states state = GameStarting;
void main (void)
{
	// Stop WDT
	WDTCTL = WDTPW | WDTHOLD;		// Stop watchdog timer
	_BIS_SR(GIE);
	runtimerA2();
	//Perform initializations (see peripherals.c)
	configTouchPadLEDs();
	configDisplay();
	configCapButtons();

	CAP_BUTTON keypressed_state;
	while(1)
	{
		switch(state)
		{
			case WelcomeScreen:
	    		GrClearDisplay(&g_sContext);
	    		GrStringDrawCentered(&g_sContext, "MSP430 HERO!", AUTO_STRING_LENGTH, 51, 22, TRANSPARENT_TEXT);
	    		GrStringDrawCentered(&g_sContext, "Press S1 to Start", AUTO_STRING_LENGTH, 51, 42, TRANSPARENT_TEXT);
	    		GrFlush(&g_sContext);
	    		//Changes the state to Game Starting if button 1 is pressed
	    		if (configButtons() == 1)
	    		{
	    			state = GameStarting;
	    		}
	    		else state = WelcomeScreen;
	    	break;

			case GameStarting:
				if (led_on ==1)
				{
					GrClearDisplay(&g_sContext);
					GrStringDrawCentered(&g_sContext, "3", AUTO_STRING_LENGTH, 51, 22, TRANSPARENT_TEXT);
					GrFlush(&g_sContext);
					configLEDs(3);
					led_on = 0;
				}
				else if (led_on ==2)
				{
					GrClearDisplay(&g_sContext);
					GrStringDrawCentered(&g_sContext, "2", AUTO_STRING_LENGTH, 51, 22, TRANSPARENT_TEXT);
					GrFlush(&g_sContext);
					configLEDs(2);
					led_on = 0;
				}
				else if (led_on == 3)
				{
					GrClearDisplay(&g_sContext);
					GrStringDrawCentered(&g_sContext, "1", AUTO_STRING_LENGTH, 51, 22, TRANSPARENT_TEXT);
					GrFlush(&g_sContext);
					configLEDs(1);
					led_on = 0;
				}
				else if (led_on == 4)
				{
					GrClearDisplay(&g_sContext);
					GrStringDrawCentered(&g_sContext, "GO", AUTO_STRING_LENGTH, 51, 22, TRANSPARENT_TEXT);
					GrFlush(&g_sContext);
					configLEDs(0);
					led_on = 0;
				}
	    	break;
		}
	}
}



void swDelay(char numLoops)
{
	// This function is a software delay. It performs
	// useless loops to waste a bit of time
	//
	// Input: numLoops = number of delay loops to execute
	// Output: none
	//
	// smj, ECE2049, 25 Aug 2013

	volatile unsigned int i,j;	// volatile to prevent optimization
			                            // by compiler

	for (j=0; j<numLoops; j++)
    {
    	i = 50000 ;					// SW Delay
   	    while (i > 0)				// could also have used while (i)
	       i--;
    }
}



