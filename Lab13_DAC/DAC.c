// DAC.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Implementation of the 4-bit digital to analog converter
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// Port B bits 3-0 have the 4-bit DAC

#include "DAC.h"
#include "..//tm4c123gh6pm.h"

// **************DAC_Init*********************
// Initialize 4-bit DAC 
// Input: none
// Output: none
void DAC_Init(void){unsigned long volatile delay;
  
	SYSCTL_RCGC2_R |= 0x00000012; // activate port B and E
	  delay = SYSCTL_RCGC2_R;    // allow time to finish activating
	
	    GPIO_PORTB_LOCK_R  |= 0x4C4F434B;     // unlock GPIO Port B
    GPIO_PORTB_CR_R    |= 0x0F;           // allow change to PB3-PB0

	
	GPIO_PORTB_AMSEL_R &= ~0x0F;      // no analog 
  GPIO_PORTB_PCTL_R &= ~0x0000FFFF; // regular function
  GPIO_PORTB_DIR_R |= 0x0F;      // make PB3-0 out
  GPIO_PORTB_DR8R_R |= 0x0F;    // can drive up to 8mA out
	GPIO_PORTB_AFSEL_R &= ~0x0F;   // disable alt funct on PB3-0
  GPIO_PORTB_DEN_R |= 0x0F;      // enable digital I/O on PB3-0
}


// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
void DAC_Out(unsigned long data){
	GPIO_PORTB_DATA_R = data;
}
