#include "camera_detect.h"
#include "LPC11xx.h"

#define CAMERA_PIN (1UL << 2)

static uint8_t camera_detect = 0;

// Pin in PIO1_2
void cameraDetectInit(void){
    // Set IOCONFIG to regular pin function
    LPC_IOCON->R_PIO1_2 |= 0x01;
    
    // Set interrupt to be triggered on edges
    LPC_GPIO1->IS &= ~(CAMERA_PIN);
    // Set rising edge interrupt on pin
    LPC_GPIO1->IEV |= CAMERA_PIN;
    // Unmask pin interrupt
    LPC_GPIO1->IE |= CAMERA_PIN;
    
    // Set priority of interrupts for pins on port 1
    NVIC_SetPriority(EINT1_IRQn, 0);
    // Enable interrupts for pins on port 1
    NVIC_EnableIRQ(EINT1_IRQn);    
    
    return;
}

uint8_t getCameraDetect(void){
    return camera_detect;
}

void setCameraDetect(uint8_t n){
    camera_detect = n;
}
