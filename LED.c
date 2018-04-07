#include "LED.h"
#include "LPC11xx.h"

void configureGPIO(void)
{

	//set port 0_7 to output (high current drain in LPC1114)
    LPC_GPIO0->DIR |= (1<<7);
    //LPC_GPIO0->DIR |= (1UL << 6);
    //LPC_GPIO0->DIR |= (1UL << 9);
}

void ledOn(void)
{
	LPC_GPIO0->DATA &= ~(1<<7);
}

void ledOff(void)
{						 
	LPC_GPIO0->DATA |= (1<<7);
}
