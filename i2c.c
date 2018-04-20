#include "i2c.h"

#define ACCEL_ADDRESS_W 0x98
#define ACCEL_ADDRESS_R 0x99

#define ACCEL_SENSITIVITY 0.047

void configure_i2c(void)
{

	LPC_IOCON->PIO0_4 |= 1UL;
	LPC_IOCON->PIO0_5 |= 1UL;
	
	LPC_SYSCON->SYSAHBCLKCTRL |= (1UL<<5);
	
	LPC_SYSCON->PRESETCTRL &= ~(1UL<<1);
	LPC_SYSCON->PRESETCTRL |= (1UL<<1); 
	
	LPC_I2C->CONSET |= (1UL << 6); // Set to Master
	
	LPC_I2C->SCLH = 200; // Set register to 30 for 12MHz Clock for Fast Mode
	LPC_I2C->SCLL = 200;
	
	
}

void write_address(uint8_t addr, uint8_t rw)
{
	LPC_I2C->CONSET |= (1UL << 5); // Set Start
	
	while(LPC_I2C->STAT == 0xF8) // Wait
	{}
	uint32_t status = LPC_I2C->STAT;
	
	if((status == 0x08) || (status == 0x10)) // SUccess if START or Repeated START
	{
		//printf("Success Start \n\r");
		LPC_I2C->DAT = addr; // WRITE ADDRESS
		LPC_I2C->CONCLR = 0x28; //clr SI bits
	}
	else
	{
		//printf("ADDR ERR: 0x%04x \n\r", status);
		return;
	}
	
	while(LPC_I2C->STAT == 0xF8)
	{}
	status = LPC_I2C->STAT;
	
	if(status == 0x18) //write
	{
		//printf("Success ADDR Write \n\r");
		return;
	}
	else if(status == 0x40) //read
	{
		//printf("Success ADDR Read \n\r");
		LPC_I2C->CONCLR = 0x08;
		return;
	}
	else
	{
		//printf("ADDR ERR: 0x%04x \n\r", status);
		return;
	}
}

void write_byte(uint8_t byte)
{
	LPC_I2C->DAT = byte; // Load data into data register
	LPC_I2C->CONCLR = 0x08;
	
	while(LPC_I2C->STAT == 0xF8)
	{}
	uint32_t status = LPC_I2C->STAT;
	
	if(status == 0x28) // Byte sent correctly
	{
		//printf("Success Write \n\r");
		return;
	}
	else
	{
		printf("WRITE ERR: 0x%04x \n\r", status);
		return;
	}
}

uint8_t read_byte(void)
{
//	uint32_t status = LPC_I2C->STAT;
//	printf("b4Read: 0x%04x \n\r", status);
	
	uint8_t byte = LPC_I2C->DAT;
	
//	
//	LPC_I2C->CONSET = 0x04;
//	LPC_I2C->CONCLR = 0x08;

//	while(LPC_I2C->STAT == 0xF8)
//	{}
//	
//	while(byte == 0x99)
//	{
//		byte = LPC_I2C->DAT;
//		printf("w \n\r");
//	}
//	
//	LPC_I2C->CONCLR = 0x0C;
	
	while(LPC_I2C->STAT == 0xF8)
	{}
	uint32_t status = LPC_I2C->STAT;
		
	if(status == 0x58) // Byte sent correctly NACK Sent
	{
		//printf("Success Read \n\r");
		//printf("DATA: %d \n\r", byte);
		return byte;
	}
	else
	{
		printf("READ ERR: 0x%04x \n\r", status);
		printf("ERR DATA: %d \n\r", byte);
		return 0;
	}
}

void I2C_write(uint8_t address, uint8_t byte)
{
	write_address(address, 0x00);
	write_byte(byte);
	
	LPC_I2C->CONSET |= 0x10;
	LPC_I2C->CONCLR = 0x08;
}

uint8_t I2C_read(uint8_t address)
{
	uint8_t ret;
	write_address(address, 0x01);
	ret = read_byte();
	
	LPC_I2C->CONSET |= 0x10;
	LPC_I2C->CONCLR = 0x08;
	return ret;
}


