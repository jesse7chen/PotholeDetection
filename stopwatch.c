#include "stopwatch.h"
#include "LPC11xx.h"
#include <stdio.h>

#ifdef DEMO
static int fun_ptr_set = 0;
static void (*fun_ptr)(void);


void delayedExecuteInit(int ticks){
    /* Enable clock for our 16 bit timer (1st one) */
    LPC_SYSCON->SYSAHBCLKCTRL |= 1UL << 7;
    
    /* Activate timer, but set it to halt */
    LPC_TMR16B0->TCR = 0x03;
    
    /* Reset timer and generate interrupt when we hit count limit */
    LPC_TMR16B0->MCR = (1UL << 1) | (1UL << 0);
    /* Set time to count to */
    //LPC_TMR32B1->MR0 = 240000;
    // 2399 will give 50us resolution, each TC increment is 50 us
    // Gives us a max delay value of ~3 seconds
    LPC_TMR16B0->PR = 2399;
    
    LPC_TMR16B0->MR0 = ticks;
    /*  Enable timer interrupt */
    NVIC_SetPriority(TIMER_16_0_IRQn, 0);
    //NVIC_ClearPendingIRQ(TIMER_32_0_IRQn);
    NVIC_EnableIRQ(TIMER_16_0_IRQn);
}

// Time is in ms!
void delayedExecute(void (*desired_function)(void), int time){
    int ticks;
    // Change time in ms to ticks
    ticks = time*20;
    
    // Initialize timer
    
    delayedExecuteInit(ticks);
    // Set function 
    fun_ptr = desired_function;
    fun_ptr_set = 1;
    // Start timer as usual
    LPC_TMR16B0->TCR = 1UL;
}

void executeFunction(void){
    // Stop and reset timer
    LPC_TMR16B0->TCR = 0x03;
    // Execute requested function if there is one
    if (fun_ptr_set){
        fun_ptr();
        fun_ptr_set = 0;
    }  
}


#elif TIME_TEST
void stopwatchInit(void){
    /* Enable clock for our 16 bit timer (1st one) */
    LPC_SYSCON->SYSAHBCLKCTRL |= 1UL << 7;
    
    /* Activate timer, but set it to halt */
    LPC_TMR16B0->TCR = 0x03;
    
    /* Stop timer and generate interrupt when we hit count limit */
    LPC_TMR16B0->MCR = (1UL << 0) | (1UL << 2);
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
    // Start stopwatch
    LPC_TMR16B0->TCR = 1UL;
}

// Should return time in ms
double stopwatchStop(void){
    double time;
    // Stop counters
    LPC_TMR16B0->TCR = 0;
    // Number of 50us (0.05 ms) increments since stopwatchStart
    time = LPC_TMR16B0->TC;
    // Change time to ms
    time = time/20.0;
    //printf("SW time: %f ms\r\n", time);
    
    // Reset stopwatch
    LPC_TMR16B0->TCR = 0x03;
    
    return time;
}
#endif
