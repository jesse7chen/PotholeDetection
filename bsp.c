/* Board Support Package */
#include "bsp.h"
#include "LPC11xx.h"
#include <stdio.h>
#include "GPS.h"
#include "LED.h"
#include "timer.h"
#include "button.h"
#include "config.h"

#include "stopwatch.h"

void TIMER32_0_IRQHandler(void){
    /* Check which match register triggered the interrupt here */
    if((LPC_TMR32B0->IR & 0x01) == 0x01){
        /* Call read button handler if the interrupt was timer used for the button by reading if bit 0 is 1 */
        /* Clear interrupt register */
        LPC_TMR32B0->IR = (0x01);
        // Call GPS function

    }
    NVIC_ClearPendingIRQ(TIMER_32_0_IRQn);
    readGPS();
}

void TIMER32_1_IRQHandler(void){
    /* Check which match register triggered the interrupt here */

    if((LPC_TMR32B1->IR & 0x01) == 0x01){
        /* Call read button handler if the interrupt was timer used for the button by reading if bit 0 is 1 */
        /* Clear interrupt register */
        LPC_TMR32B1->IR = (0x01);
        timerEnd();
    }
    NVIC_ClearPendingIRQ(TIMER_32_1_IRQn);
}

void TIMER16_0_IRQHandler(void){
    /* Check which match register triggered the interrupt here */
    
    // Note, if operating in stopwatch mode, this should only trigger if the stopwatch overflows
    if((LPC_TMR16B0->IR & 0x01) == 0x01){
        /* Call read button handler if the interrupt was timer used for the button by reading if bit 0 is 1 */
        /* Clear interrupt register */
        LPC_TMR16B0->IR = (0x01);
        
#ifdef DEMO
        executeFunction();
        
#elif TIME_TEST
        printf("Stopwatch overflowed\r\n");
        // Reset stopwatch
        LPC_TMR16B0->TCR = 0x03;
#endif
    }
    NVIC_ClearPendingIRQ(TIMER_16_0_IRQn);
}

void TIMER16_1_IRQHandler(void){
    /* Check which match register triggered the interrupt here */

    if((LPC_TMR16B1->IR & 0x01) == 0x01){
        /* Call read button handler if the interrupt was timer used for the button by reading if bit 0 is 1 */
        /* Clear interrupt register */
        LPC_TMR16B1->IR = (0x01);
        readButtons();
    }
    NVIC_ClearPendingIRQ(TIMER_16_1_IRQn);
}
