#include "stopwatch.h"
#include "LPC11xx.h"
#include <stdio.h>

void stopwatchInit(void){
    /* Enable clock for our 16 bit timer (1st one) */
    LPC_SYSCON->SYSAHBCLKCTRL |= 1UL << 7;
    
    /* Activate timer, but set it to halt */
    LPC_TMR16B0->TCR = 0x03;
    
    /* Generate interrupt and reset timer when we hit count limit */
    LPC_TMR16B0->MCR = (1UL << 1) | (1UL << 0);
    /* Set time to count to */
    //LPC_TMR32B1->MR0 = 240000;
    // 2399 will give 50us resolution, each TC increment is 50 us
    LPC_TMR16B0->PR = 2399;
    // Gives us a max stopwatch value of 3 seconds
    LPC_TMR16B0->MR0 = 60000;
    /*  Enable timer interrupt */
    NVIC_SetPriority(TIMER_16_0_IRQn, 0);
    //NVIC_ClearPendingIRQ(TIMER_32_0_IRQn);
    NVIC_EnableIRQ(TIMER_16_0_IRQn);
}

void stopwatchStart(void){
    // Start timer
    LPC_TMR16B0->TCR = 1UL;
}

double stopwatchStop(void){
    double time;
    // Stop counters
    // LPC_TMR16B0->TCR = 0;
    // Number of 50us (0.05 ms) increments since stopwatchStart
    time = LPC_TMR16B0->TC;
    // Change time to ms
    time = time/20;
    printf("SW time: %f ms\r\n", time);
    
    // Reset stopwatch
    LPC_TMR16B0->TCR = 0x03;
    
    return time;
}
