// Lab13.c
// Runs on LM4F120 or TM4C123
// Use SysTick interrupts to implement a 4-key digital piano
// edX Lab 13 
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// Port B bits 3-0 have the 4-bit DAC
// Port E bits 3-0 have 4 piano keys

#include "..//tm4c123gh6pm.h"
#include "Sound.h"
#include "Piano.h"
#include "TExaS.h"

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void delay(unsigned long msec);
int main(void){ unsigned long input,previous;// Real Lab13 
	// for the real board grader to work 
	// you must connect PD3 to your DAC output
  TExaS_Init(SW_PIN_PE3210, DAC_PIN_PB3210,ScopeOn); // activate grader and set system clock to 80 MHz
// PortE used for piano keys, PortB used for DAC        
  Sound_Init(); // initialize SysTick timer and DAC
  Piano_Init();
  EnableInterrupts();  // enable after all initialization are done
  while(1){                
// input from keys to select tone
		input = Piano_In(); 
    
		if(input!=previous)
		{			
			if(!input){ // just released     
				Sound_Off();
				continue;
			}
			
			if(input==0x01){       
				// 0x01 is key 0 pressed
				EnableInterrupts();
				 Sound_Tone(9556);      // Play 523.251 Hz wave										
			}
			if(input==0x02){								//(input==0x02)&&(input!=previous)       
			 //	0x02 is key 1 pressed
				EnableInterrupts();
				 Sound_Tone(8513);      // Play 587.330 Hz wave										
			}
			if(input==0x04){       
			 // 0x04 is key 2 pressed
				EnableInterrupts();
				 Sound_Tone(7584);      // Play 659.255 Hz wave							7583							
			}
			if(input==0x08){     
			 // 0x08 is key 3 pressed
				EnableInterrupts();
				 Sound_Tone(6378);      // Play 783.991 Hz wave										
			}
		}
			
		/*
		if(!input){ // just released     
			Sound_Off();
    }
		*/
		previous = input; 
		delay(10);  // remove switch bounce    
	}        
}

// Inputs: Number of msec to delay
// Outputs: None
void delay(unsigned long msec){ 
  unsigned long count;
  while(msec > 0 ) {  // repeat while there are still delay
    count = 16000;    // about 1ms
    while (count > 0) {       count--;
    } // This while loop takes approximately 3 cycles
    msec--;
  }
}


