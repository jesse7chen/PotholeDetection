#include "UART.h"
#include "timer.h"
#include "LED.h"
#include "LPC11xx.h"


// TODO: Add support for GPS + DEBUG writing
void UART_init(){
    // Initialize clocks for GPIO and for IOCON
    LPC_SYSCON->SYSAHBCLKCTRL |= (1UL << 6) | (1UL << 16);
    
    // Initialize GPIO pins for UART functionality
    LPC_IOCON->PIO1_6 |= 1UL;
    LPC_IOCON->PIO1_7 |= 1UL;
    
    // Initialize clock for UART
    LPC_SYSCON->SYSAHBCLKCTRL |= (1UL << 12);
    
    // Set UART clock to be CCLK/4 or 12 MHz
    LPC_SYSCON->UARTCLKDIV = (4UL <<  0);
    
    // Set 8 bits, no parity, active divisor latches
    LPC_UART->LCR = 0x83;  
    
    // Setting baud rate to be 9600
    LPC_UART->DLL = 52;
    LPC_UART->DLM = 0;
    LPC_UART->FDR = 0x21;
    
    // Disable divisor latches now that baudrate is set. Need to do this to access receive and transmit buffers
    LPC_UART->LCR = 0x03;
    
}

// Reads a single byte
int UART_read(char* c){
    /* Store in local variable in case we want to check other bits in LSR */
    /*
    int LSR = LPC_UART->LSR;
    
    // Check for valid data
    if(LSR & 0x01){
        *c = (char)LPC_UART->RBR;
        return 0;
    }
    else{
        // Indicates that no data was available
        return -1;
    
    */
    
    // Setup time out timer for UART reads (currently 5ms)
    timer_init(240000);
    timerStart();
    // Code for blocking read with timeout
    while (!(LPC_UART->LSR & 0x01)){
        if (getTimerStatus() == 1){
            // Timer has expired, return
            return -1;
        }
    }
    *c = LPC_UART->RBR;
    return 0;
    
}

char UART_read_blocking(void){
    while (!(LPC_UART->LSR & 0x01));
    return (char)(LPC_UART->RBR);
}




// Writes a single byte, returns byte written
char UART_write(char c){
    //int LSR = LPC_UART->LSR;
    
    // Check that transmit register doesn't already have data
    // Non-blocking write
    /*
    if(LPC_UART->LSR & 0x020){
        LPC_UART->THR = c;
        return 0;
    }
    else{
        return -1;
    } 
    */
    // Blocking write
    
    while (!(LPC_UART->LSR & 0x20));
    LPC_UART->THR = c;

    return (c);
}

// Writes an entire string
int UART_write_string(char* s){
    char c = s[0];
    int idx = 0;
    int retVal = 0;
    
    // Keep writing until null terminator
    while(c != '\0'){
        UART_write(c);
        /*
        retVal = UART_write(c);
        if(retVal != 0){
            return retVal;
        }
        */
        c = s[++idx];
    }
    return retVal;
}

int UART_read_string(char* s, int maxLength){
    int idx = 0;
    char c = 0x01;
    int retVal = 0;
    
    // Keep reading until we hit NULL or maxLength
    while((c != '\0') && (idx < maxLength)){
        retVal = UART_read(&c);
        if (retVal != 0){
            return retVal;
        }
        s[idx++] = c;
    }
    return retVal;
}



