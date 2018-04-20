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
#include "camera_detect.h"

#include "accelerometer.h"

// Old GPS timer handler
/*
void TIMER32_0_IRQHandler(void){
    // Check which match register triggered the interrupt here 
    if((LPC_TMR32B0->IR & 0x01) == 0x01){
        // Call read button handler if the interrupt was timer used for the button by reading if bit 0 is 1
        // Clear interrupt register
        LPC_TMR32B0->IR = (0x01);
        // Call GPS function
        //readGPS();
    }
    NVIC_ClearPendingIRQ(TIMER_32_0_IRQn); 
}
*/

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

void UART_IRQHandler(void){
    uint8_t interruptID;
    
    interruptID = (LPC_UART->IIR & 0x0E) >> 1;    
    if (interruptID == 0x02){
        // Data available
        if (readNMEA_UART() == -1){
            resetGPSbuffer();
        }
    }
    
    NVIC_ClearPendingIRQ(UART_IRQn);
}

void PIOINT1_IRQHandler(void){
    // GPIO pin interrupt, looking for interrupt on PIO1_2 (camera detected pothole)
    
    if((LPC_GPIO1->MIS & (1UL << 2)) == 1){
        // Camera has detected something
        setCameraDetect(1);
        // Clear interrupt
        LPC_GPIO1->IC = (1UL << 2);
    }
    
    NVIC_ClearPendingIRQ(EINT1_IRQn);
    __NOP();
	__NOP();
}

