#include "buzzer.h"
#include "LPC11xx.h"
#include "timer.h"
#include "config.h"
#include "stopwatch.h"

void buzzerInit(void){
    //set port 1_5 to output (high current drain in LPC1114)
    LPC_GPIO1->DIR |= (1UL << 5);
    // Set port 1_4 to output
    LPC_GPIO1->DIR |= (1UL << 4);
    // Set port 1_4 high to stop buzzer
    LPC_GPIO1->DATA |= (1<<5);
    // Set GPIO pin low
    buzzerOff();
}

void buzzerOn(void){
	LPC_GPIO1->DATA &= ~(1<<4);
}

void buzzerOff(void){						 
	LPC_GPIO1->DATA |= (1<<4);
}

void hapticWarnUser(void){
    buzzerOn();
    // Set buzzerOff to execute in one second
#ifdef DEMO
    // Second argument is time in ms
    delayedExecute(buzzerOff, 1500);
#elif TIME_TEST
    // Wait a half second
    threadWait(24000000);
    buzzerOff();
#endif
    
}
