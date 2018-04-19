#include "timer.h"
#include "LPC11xx.h"
#include "LED.h"

static volatile int timer_status = 0;
static void (*fun_ptr)(void);
static int fun_ptr_set = 0;

// There is no prescale register, so ticks refers to number of clock cycles at 48 MHz 
void timer_init(int ticks){
        
    /* Enable clock for our 32 bit timer (2nd one) */
    LPC_SYSCON->SYSAHBCLKCTRL |= 1UL << 10;
    /* Activate timer, but set it to halt */
    LPC_TMR32B1->TCR = 0x03;
    /* Generate interrupt and reset timer when we hit count limit */
    LPC_TMR32B1->MCR = (1UL << 1) | (1UL << 0);
    /* Set time to count to */
    LPC_TMR32B1->MR0 = ticks;
    /*  Enable timer interrupt */
    NVIC_SetPriority(TIMER_32_1_IRQn, 0);
    //NVIC_ClearPendingIRQ(TIMER_32_0_IRQn);
    NVIC_EnableIRQ(TIMER_32_1_IRQn);
    
}

void timerStart(void){
    timer_status = 0;
    // Start timer
    LPC_TMR32B1->TCR = 1UL;
}

void timerEnd(void){
    timer_status = 1;
    
    // Stop timer
    LPC_TMR32B1->TCR = 0x03;
    
    // Execute requested function if there is one
    if (fun_ptr_set){
        fun_ptr();
        fun_ptr_set = 0;
    }
}

int getTimerStatus(void){
    return timer_status;
}

void threadWait(int ticks){
    timer_init(ticks);
    timerStart();
    while(timer_status != 1);
    return;
}

/*
void delayedExecute(void (*desired_function)(void), int ticks){
    // Initialize timer
    timer_init(ticks);
    // Set function 
    fun_ptr = desired_function;
    fun_ptr_set = 1;
    // Start timer as usual
    timerStart();  
}
*/
