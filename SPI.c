#include "SPI.h"
#include "LPC11xx.h"
#include "LED.h"

/* SSP Status register */
#define SSPSR_TFE       (0x1<<0)
#define SSPSR_TNF       (0x1<<1)
#define SSPSR_RNE       (0x1<<2)
#define SSPSR_RFF       (0x1<<3)
#define SSPSR_BSY       (0x1<<4)

/* SSP CR1 register */
#define SSPCR1_LBM      (0x1<<0)
#define SSPCR1_SSE      (0x1<<1)
#define SSPCR1_MS       (0x1<<2)
#define SSPCR1_SOD      (0x1<<3)

void SPI_init(void){
    // Ensure SPI is reset before initializing
    LPC_SYSCON->PRESETCTRL &= ~1UL;
    // IMPORTANT: Enable clock to IOCON block
    LPC_SYSCON->SYSAHBCLKCTRL |= (1UL << 16);
    
    // Initialize pins
    
    LPC_IOCON->PIO0_8 &= ~0x07;
    LPC_IOCON->PIO0_8 |= 1UL; // MISO0
    LPC_IOCON->PIO0_9 &= ~0x07;
    LPC_IOCON->PIO0_9 |= 1UL; // MOSI0
    LPC_IOCON->SCK_LOC |= 0x02; // Set location of SCLK to PIO0_6
    LPC_IOCON->PIO0_6 |= 0x02; // SCLK
    // Disable default behavior of chip select for now
    //LPC_IOCON->PIO0_2 &= ~0x07;
    //LPC_IOCON->PIO0_2 |= 1UL; // CS
    
    // Initialize clock for SPI0 and SPI1
    LPC_SYSCON->SYSAHBCLKCTRL |= (1UL << 11);
    LPC_SYSCON->SYSAHBCLKCTRL |= (1UL << 18);
    
    // Initialize peripheral clock for SPI0, needs to be less than 4MHz
    LPC_SYSCON->SSP0CLKDIV = 1;
    
    // Turn off the SPI0 reset control
    LPC_SYSCON->PRESETCTRL |= 1UL;
    LPC_SYSCON->PRESETCTRL |= (1UL << 2);
    
    // Set dataframe size to 8 bits and SCR to 7
    LPC_SSP0->CR0 |= 0x0707;
    
    // This should set our clock speed to 3MHz
    LPC_SSP0->CPSR = 0x02;
    
    // Enable the SPI controller
    LPC_SSP0->CR1 |= (1UL << 1);
    
}


int SPI_read(uint16_t* c){
    // Check that the recieve FIFO is not empty
    if ((LPC_SSP0->SR & (1UL << 2)) != 0){
        *c = LPC_SSP0->DR;
        return 0;
    }
    return -1;
    
}

int SPI_write(uint16_t c){
    // Check that transmit FIFO is not full;
    /*
    if((LPC_SSP0->SR & (0x02)) != 0){
       LPC_SSP0->DR = c;
       return 0;
    }
    return -1;
    */
    // Blocking write, wait until FIFO not full
    while((LPC_SSP0->SR & (0x02)) == 0);
    LPC_SSP0->DR = c;
    return 0;
}








