// ***** 0. Documentation Section *****
// SwitchLEDInterface.c for Lab 8
// Runs on LM4F120/TM4C123
// Use simple programming structures in C to toggle an LED
// while a button is pressed and turn the LED on when the
// button is released.  This lab requires external hardware
// to be wired to the LaunchPad using the prototyping board.
// January 15, 2016
//      Jon Valvano and Ramesh Yerraballi

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void delayms(unsigned long ms); // Gives Delay in ms

// ***** 3. Subroutines Section *****

// PE0, PB0, or PA2 connected to positive logic momentary switch using 10k ohm pull down resistor
// PE1, PB1, or PA3 connected to positive logic LED through 470 ohm current limiting resistor
// To avoid damaging your hardware, ensure that your circuits match the schematic
// shown in Lab8_artist.sch (PCB Artist schematic file) or 
// Lab8_artist.pdf (compatible with many various readers like Adobe Acrobat).
int main(void){unsigned long volatile delay;
//**********************************************************************
// The following version tests input on PE0 and output on PE1
//**********************************************************************
  TExaS_Init(SW_PIN_PE0, LED_PIN_PE1, ScopeOn);  // activate grader and set system clock to 80 MHz
  
  SYSCTL_RCGC2_R |= 0x10;           // Port E clock
  delay = SYSCTL_RCGC2_R;           // wait 3-5 bus cycles
  GPIO_PORTE_DIR_R |= 0x02;         // PE1 output
  GPIO_PORTE_DIR_R &= ~(0x01);      // PE0 input
  GPIO_PORTE_AFSEL_R &= ~(0x03);    // not alternative
  GPIO_PORTE_AMSEL_R &= ~(0x03);      // no analog
  GPIO_PORTE_PCTL_R &= ~(0x000000FF); // bits for PE1,PE0
  GPIO_PORTE_DEN_R |= 0x03;         // enable PE1,PE0
	
	//The system starts with the LED on (make PE1 =1). 
	GPIO_PORTE_DATA_R = 0x02;
	//Wait about 100 ms
	delayms(100);  // 100ms delay makes a 5Hz period

  EnableInterrupts();           // enable interrupts for the grader
  while(1)
	{
			if(GPIO_PORTE_DATA_R&0x01)
			{
				//toggle the LED once
				GPIO_PORTE_DATA_R ^= 0x02;
				//Wait about 100 ms
				delayms(100);  // 100ms delay makes a 5Hz period
			}
			else
			{
				//turn the LED on
				GPIO_PORTE_DATA_R = 0x02;
			}
  }
  
}

void delayms(unsigned long ms){
  unsigned long count;
  while(ms > 0 ) { // repeat while there are still ms to delay
    count = 16000; // number of counts to delay 1ms at 80MHz
    while (count > 0) { 
      count--;
    } // This while loop takes approximately 3 cycles
    ms--;
  }
}
