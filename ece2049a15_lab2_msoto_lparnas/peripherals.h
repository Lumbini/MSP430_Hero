/*
 * peripherals.h
 *
 *  Created on: Jan 29, 2014
 *      Author: ndemarinis
 */

#ifndef PERIPHERALS_H_
#define PERIPHERALS_H_

#include <msp430.h>
#include <stdint.h>
#include "inc\hw_memmap.h"
#include "driverlibHeaders.h"
#include "CTS_Layer.h"

#include "grlib.h"
#include "LcdDriver/Dogs102x64_UC1701.h"

#define NUM_KEYS	5
#define LED4		BIT5
#define LED5		BIT4
#define LED6		BIT3
#define LED7		BIT2
#define LED8		BIT1


// You may find it useful to define a new type for representing the
// capacitive buttons. Since we have fewer than 8 buttons, we can use
// a char which will allow one bit to represent the state of each button.
// Which corresponds to which button?  Find out using the debugger and
// write #define statements for each button
typedef uint8_t CAP_BUTTON;

#define BUTTON_NONE 0x00   // No buttons pressed
// Add definitions here...


// Globals
extern tContext g_sContext;	// user defined type used by graphics library

// Prototypes for functions defined implemented in peripherals.c
void configDisplay(void);
void configTouchPadLEDs(void);
void BuzzerOn(void);
void BuzzerOff(void);

void configCapButtons(void);
CAP_BUTTON CapButtonRead(void);



#endif /* PERIPHERALS_H_ */
