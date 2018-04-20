#include "accelerometer.h"
#include "i2c.h"
#include "bluetooth.h"
#include "string.h"

static uint8_t data_int = 0;
static int curr_int;
static float list[3];
static uint8_t potholeDetected = 0;
static char temp_buffer[20];

#define ACCEL_ADDRESS_W 0x98
#define ACCEL_ADDRESS_R 0x99
#define PRESCALE (400000-1) //48000 PCLK clock cycles to increment TC by 1 

#define ACCEL_SENSITIVITY 0.047
#define THRESHOLD .5

#define ACCEL_SENSITIVITY 0.047

float getListVal(int idx){
    return list[idx];
}

void init_accelerometer(void)
{
	//init for direct read
	accel_writeregister(0x07, 0x00);
	accel_writeregister(0x07, 0x18);
	accel_writeregister(0x07, 0x19);
    
    initTimer0();
	
	// Interrupt based Tap Detection Init
	
//	accel_writeregister(0x07, 0x00); // Allows accel setting changes
//	//accel_writeregister(0x09, 0x84); //Set to Z direction - Transitions at .18g
//	accel_writeregister(0x09, 0x9A); //Set to Z direction - Transitions at .18g
//	accel_writeregister(0x0A, 0x01); // One count transitions
//	accel_writeregister(0x06, 0x04); // Set Interrupt
//	accel_writeregister(0x08, 0x00); // Set sample Rate
//	accel_writeregister(0x07, 0xC1); // Allows accel setting changes
//	int data = accel_readregister(0x07);
//	printf("0x%04x \n\r", data);
//	data = accel_readregister(0x08);
//	printf("0x%04x \n\r", data);
//	
//	LPC_SYSCON->SYSAHBCLKCTRL |= (1UL << 6);
//	
//	LPC_GPIO0->DIR &= ~(1UL << 11); // Set to input
//	LPC_IOCON->R_PIO0_11 = 1UL;
//	
//	LPC_GPIO0->IS &= ~(1UL << 11); // Edge Triggers Interrupt
//	//LPC_GPIO0->IS |= (1UL << 11); // Edge Triggers Interrupt
//	LPC_GPIO0->IBE &= ~(1UL << 11); // One edge Triggers Interrupt
//	LPC_GPIO0->IEV |= (1UL << 11); // Rising edges Triggers Interrupt
//	//LPC_GPIO0->IEV &= ~(1UL << 11); // Falling edges Triggers Interrupt
//	LPC_GPIO0->IE |= (1UL << 11);

//	NVIC_EnableIRQ(EINT0_IRQn);
		
}

uint8_t getAccPotholeDet(void){
    return potholeDetected;
}

void resetAccPotholeDet(void){
    potholeDetected = 0;
}

void TIMER32_0_IRQHandler(void) 
{
	uint8_t data;
	data = accel_readregister(0x02);
	list[curr_int] = data_convert(data);
    sprintf(temp_buffer, "%d\r\n", data);
    bleWriteUART(temp_buffer, strlen(temp_buffer));
	
//	if(prev_int == 1 || curr_int == 2)
//	{
//		//printf("here");
//		//printf("%f \n\r", ((list[0] + list[1] + list[2])/3));
//		prev_int = 1;
//		if(list[2] - list[1] > .2 || list[2] - list[1] < -0.4)
//		{
//			printf("Detected \n\r");
//		}
//	}
	
	curr_int = curr_int + 1;
	if(curr_int == 3)
	{
		curr_int = 0;
	}
    
    if(checkAccel() == 1){
        // Pothole has been detected, set flag
        potholeDetected = 1;
    }
	LPC_TMR32B0->IR |= (1<<0); //Clear MR0 Interrupt flag
    NVIC_ClearPendingIRQ(TIMER_32_0_IRQn); 
}

uint8_t checkAccel(void)
{
	int curr = curr_int;
	int prev = curr - 1;
	if (prev < 0)
	{
		prev = 2;
	}
	if((list[curr] - list[prev] > THRESHOLD) || (list[curr] - list[prev] < -THRESHOLD))
	{
		//printf("Detected \n\r");
		return 1;
	}
    return 0;
}

float data_convert(uint8_t data)
{
	int8_t temp = (0xC0);
	signed char ret0 = 0;
	float ret;
	
	//printf("0x%02x \n\r", temp);
	//printf("%d \n\r", data);
	if(data < 32)
	{
		ret0 = data;
		// Do nothings 
	}
	else if (data < 64)
	{
		ret0 = data;
		ret0 = ret0 + temp;
	}
	else
	{
		//printf("error val \n\r");
	}
	return ret0 * ACCEL_SENSITIVITY;
	
}

void initTimer0(void)
{
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<9); //Enable 32Bit Timer0 Clock

	LPC_TMR32B0->CTCR = 0x0;
	LPC_TMR32B0->PR = PRESCALE; 

	LPC_TMR32B0->MR0 = 1;
	LPC_TMR32B0->MCR |= (1<<0) | (1<<1); // Interrupt & Reset on MR0 match
	LPC_TMR32B0->TCR |= (1<<1); //Reset Timer0

	NVIC_EnableIRQ(TIMER_32_0_IRQn); //Enable timer interrupt
	
	data_int = 0;
	curr_int = 0;

	LPC_TMR32B0->TCR = 0x01; //Enable timer
}

void accel_writeregister(uint8_t reg, uint8_t data)
{
	write_address(ACCEL_ADDRESS_W, 0); // Write Accel Address
	write_byte(reg); // Register Set
	write_byte(data); // Data 
	LPC_I2C->CONSET |= 0x10; // End Transmission
	LPC_I2C->CONCLR = 0x08;
}

uint8_t accel_readregister(uint8_t reg)
{
		uint8_t data = 0;
		write_address(ACCEL_ADDRESS_W, 0);
		write_byte(reg);
		LPC_I2C->CONSET  |= (1 << 5);
		//LPC_I2C->CONSET |= 0x10;
		LPC_I2C->CONCLR = 0x08;
			
		write_address(ACCEL_ADDRESS_R, 1);
		
//		for(int i = 0; i < 0xFFFF; i++)
//		{
//		}
	
		data = read_byte();
		LPC_I2C->CONSET |= 0x10;
		LPC_I2C->CONCLR = 0x08;
	
		return data;
}
