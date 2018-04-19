#include "button.h"
#include "LPC11xx.h"
#include "stdint.h"
#include "stdio.h"
#include "bluetooth.h"

#define NUM_BUTTONS 1
#define REPORT 0

static int lastButtonPressed = -1;


void initButtons(void){
    /* In IOCON, all pins should be GPIO and pull-up enabled by default. They should also be configured as inputs by default */
    LPC_GPIO1->DIR &= ~(REPORT_B);
    
    //LPC_IOCON->PIO0_8 |= (1UL << 2);
    /* Enable clock for our timer */
    LPC_SYSCON->SYSAHBCLKCTRL |= 1UL << 8;
    
    /* Enable our timer */
    LPC_TMR16B1->TCR = 1UL;
    
    /* Generate interrupt and reset timer when we hit count limit */
    LPC_TMR16B1->MCR = (1UL << 1) | (1UL << 0);
    // 2399 will give 50us resolution, each TC increment is 50 us
    LPC_TMR16B1->PR = 2399;
    /* Count up to 100, this should be ~5ms */
    LPC_TMR16B1->MR0 = 100;

    /*  Enable timer interrupt */
    NVIC_SetPriority(TIMER_16_1_IRQn, 0);
    //NVIC_ClearPendingIRQ(TIMER_16_0_IRQn);
    NVIC_EnableIRQ(TIMER_16_1_IRQn);

}

/* Return button index if a button has been pressed, else return -1 */
int readButtons(void){
    static uint16_t states[6] = {0};
    int button = 0;

    /* I invert it because I think the button has a pull-up resistor */
    for(button = 0; button < NUM_BUTTONS; button++){
        states[button] = (states[button] << 1) | !rawButtonPresses(button) | 0xE000;
        if(states[button] == 0xF000){
            if(lastButtonPressed == -1){
                lastButtonPressed = button;
            }
            return button;
        }
    }
    return -1;
}

/* Returns 1 if pressed, 0 otherise */
static int rawButtonPresses(int button){
    switch (button){
        case(REPORT):
            return reportButtonPressed();
        default:
            return 0;
    }
}
static int reportButtonPressed(void){
    return LPC_GPIO1->DATA & (REPORT_B);
}

int getLastPressed(void){
    return lastButtonPressed;
}

void resetLastPressed(void){
    lastButtonPressed = -1;
}
