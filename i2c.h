#ifndef _I2C_H_
#define _I2C_H_

#include "LPC11xx.h"
#include <stdio.h>
#include <stdint.h>

uint8_t read_byte(void);
void write_byte(uint8_t byte);
void write_address(uint8_t addr, uint8_t rw);
void configure_i2c(void);

uint8_t I2C_read(uint8_t address);
void I2C_write(uint8_t address, uint8_t byte);

#endif
