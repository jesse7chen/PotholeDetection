#ifndef _ACCELEROMETER_H_
#define _ACCELEROMETER_H_

#include <lpc11xx.h>
#include <stdio.h>
#include <rt_misc.h>
#include "LPC11xx.h"
#include <stdint.h>


/*
extern uint8_t data_int;
extern int curr_int;
extern double prev_int;
extern float list[3];
*/


void init_accelerometer(void);
uint8_t checkAccel(void);

void TIMER32_0_IRQHandler(void);

void initTimer0(void);
float data_convert(uint8_t data);

void accel_writeregister(uint8_t reg, uint8_t data);
uint8_t accel_readregister(uint8_t reg);

// Getters and setters
uint8_t getAccPotholeDet(void);
void resetAccPotholeDet(void);

#endif
