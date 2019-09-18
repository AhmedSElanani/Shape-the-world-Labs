// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano
// January 15, 2016

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"


// Initialize SysTick with busy wait running at bus clock.
#define NVIC_ST_CTRL_R      (*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R    (*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R   (*((volatile unsigned long *)0xE000E018))

// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

void SysTick_Init(void);	// Initialize SysTick with busy wait running at bus clock.
void SysTick_Wait(unsigned long delay);	// The delay parameter is in units of the 80 MHz core clock. (12.5 ns)
void SysTick_Wait10ms(unsigned long delay);	// 10000us equals 10ms


// ***** 3. Subroutines Section *****


// Linked data structure
struct State {
  unsigned long OutCars;
	unsigned long OutWalk;	
  unsigned long Time;  
  unsigned long Next[8];}; 
typedef const struct State STyp;

#define goS   0
#define waitS 1
#define goW   2
#define waitW 3
#define goP	  4
#define HU1	  5
#define HU2	  6
#define HU3	  7
#define HU4	  8
#define HU5	  9
#define DW	  10	
	
STyp FSM[11]={
 {0x21,0x02,100,{goS,waitS,goS,waitS,waitS,waitS,waitS,waitS}}, 
 {0x22,0x02,100,{goP,goW,goP,goW,goP,goP,goP,goP}},
 {0x0C,0x22,100,{goW,goW,waitW,waitW,waitW,waitW,waitW,waitW}},
{0x14,0x22,100,{goS,goS,goS,goS,goP,goP,goS,goS}},
 {0x24,0x08,100,{goP,HU1,HU1,HU1,goP,HU1,HU1,HU1}},  
 {0x24,0x00,50,{HU2,HU2,HU2,HU2,HU2,HU2,HU2,HU2}}, 
 {0x24,0x08,50,{HU3,HU3,HU3,HU3,HU3,HU3,HU3,HU3}},
 {0x24,0x00,50,{HU4,HU4,HU4,HU4,HU4,HU4,HU4,HU4}},
 {0x24,0x08,50,{HU5,HU5,HU5,HU5,HU5,HU5,HU5,HU5}},
 {0x24,0x00,50,{DW,DW,DW,DW,DW,DW,DW,DW}},
 {0x24,0x02,50,{goW,goW,goS,goW,goW,goW,goS,goW}}
};

unsigned long S;  // index to the current state 
unsigned long Input; 

int main(void){ volatile unsigned long delay;
  TExaS_Init(SW_PIN_PE210, LED_PIN_PA765432,ScopeOff); // activate grader and set system clock to 80 MHz

  SYSCTL_RCGC2_R |= 0x00000031;     // 1) activate clock for Port A,E and F
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0
  // only PF0 needs to be unlocked, other bits can't be locked
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog on PF
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PF4-0
  GPIO_PORTF_DIR_R = 0x0A;          // 5) PF3 , PF1 out
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) disable alt funct on PF7-0
  GPIO_PORTF_DEN_R = 0x0A;          // 7) enable digital I/O on PF3 ,PF1
	
  GPIO_PORTA_AMSEL_R &= ~0xFC; 			// 3) disable analog function on PA7-2
  GPIO_PORTA_PCTL_R &= ~0xFFFFFF00; // 4) enable regular GPIO
  GPIO_PORTA_DIR_R |= 0xFC;    			// 5) outputs on PB5-0
  GPIO_PORTA_AFSEL_R &= ~0xFC; 			// 6) regular function on PB5-0
  GPIO_PORTA_DEN_R |= 0xFC;    			// 7) enable digital on PB5-0  

  GPIO_PORTE_AMSEL_R &= ~0x07; 			// 3) disable analog function on PE2-0
  GPIO_PORTE_PCTL_R &= ~0x00000FFF; // 4) enable regular GPIO
  GPIO_PORTE_DIR_R &= ~0x07;   			// 5) inputs on PE2-0
  GPIO_PORTE_AFSEL_R &= ~0x07; 			// 6) regular function on PE2-0
  GPIO_PORTE_DEN_R |= 0x07;    			// 7) enable digital on PE2-0

	
	SysTick_Init();   // Program 10.2
  
	S = goS;
  
  EnableInterrupts();
  while(1){
		
		//1) Output 6-LEDs in an unfriendly manner
		GPIO_PORTA_DATA_R = ((FSM[S].OutCars)<<2); 
		//2) Output PF3, PF1 in an unfriendly manner
		GPIO_PORTF_DATA_R = FSM[S].OutWalk;
		//3) Wait time
		SysTick_Wait10ms(FSM[S].Time);
		//4) Input from sensors
		Input = GPIO_PORTE_DATA_R&(0x07);     // read sensors
		//5) Go to next state
		S = FSM[S].Next[Input];
  }
}



void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;               // disable SysTick during setup
  NVIC_ST_CTRL_R = 0x00000005;      // enable SysTick with core clock
}
// The delay parameter is in units of the 80 MHz core clock. (12.5 ns)
void SysTick_Wait(unsigned long delay){
  NVIC_ST_RELOAD_R = delay-1;  // number of counts to wait
  NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears
  while((NVIC_ST_CTRL_R&0x00010000)==0){ // wait for count flag
  }
}
// 10000us equals 10ms
void SysTick_Wait10ms(unsigned long delay){
  unsigned long i;
  for(i=0; i<delay; i++){
    SysTick_Wait(800000);  // wait 10ms
  }
}
