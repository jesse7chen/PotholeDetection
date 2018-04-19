#include "flash.h"
#include <string.h>
#include "stdio.h"


#define IAP_LOCATION 0x1fff1ff1
#define CCLK 48000 // System clock in kHz

// Allocate parameter arrays
static unsigned int command_params[5];
static unsigned int status_result[4];

// Declare function pointer
typedef void (*IAP)(unsigned int [], unsigned int []);
static IAP iap_entry = (IAP)IAP_LOCATION;

// TODO: Should I be clearing my command params in between every call?

int writeFlash(unsigned int dest, unsigned int source, unsigned int numBytes){
    // Prepare flash to be written
    unsigned int startSec = dest/4096;
    // We subtract 1 because we need to count our starting byte in number of bytes we write.
    // Ex. If we write at address 0x7100, 256 bytes, then we write up to 0x71FF, not to 0x7200
    unsigned int endSec = (dest+numBytes-1)/4096;
    
    
    if(prepareWrite(startSec, endSec)!= CMD_SUCCESS){
        //printf("Error preparing flash write\r\n");
        return -1;
    }
        
    memset(command_params, 0, sizeof(unsigned int)*5);
    
    command_params[0] = 51;
    command_params[1] = (unsigned int)dest;
    command_params[2] = (unsigned int)source;
    command_params[3] = numBytes;
    command_params[4] = CCLK;
    
    iap_entry(command_params, status_result);
    
    printFlashStatus(status_result[0]);
    return status_result[0];
}

int eraseSector(unsigned int startSec, unsigned int endSec){
    if (prepareWrite(startSec, endSec)!= CMD_SUCCESS){
        return -1;
    }
    
    memset(command_params, 0, sizeof(unsigned int)*5);
    command_params[0] = 52;
    command_params[1] = startSec;
    command_params[2] = endSec;
    command_params[3] = CCLK;
    
    iap_entry(command_params, status_result);
    
    return status_result[0];   
}

static int prepareWrite(unsigned int startSec, unsigned int endSec){
    memset(command_params, 0, sizeof(unsigned int)*5);
    
    command_params[0] = 50;
    command_params[1] = (unsigned int)startSec;
    command_params[2] = (unsigned int)endSec;
    
    iap_entry(command_params, status_result);
    
    printFlashStatus(status_result[0]);
    return status_result[0];
}

unsigned int printFlashStatus(unsigned int status){
    switch(status){
        case CMD_SUCCESS:{
            printf("Command success\r\n");
            break;
        }
        case INVALID_COMMAND:{
            printf("Invalid command\r\n");
            break;
        }
        case SRC_ADDR_ERROR:{
            printf("Source address error\r\n");
            break;
        }
        case DST_ADDR_ERROR:{
            printf("Dest address error\r\n");
            break;
        }
        case SRC_ADDR_NOT_MAPPED:{
            printf("Source addr not mapped\r\n");
            break;
        }
        case DST_ADDR_NOT_MAPPED:{
            printf("Dst addr not mapped\r\n");
            break;
        }
        case COUNT_ERROR:{
            printf("Count error\r\n");
            break;
        }
        case INVALID_SECTION:{
            printf("Invalid section\r\n");
            break;      
        }
        case SECTOR_NOT_BLANK:{
            printf("Sector not blank\r\n");
            break;
        }
        case SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION:{
            printf("Sector not prepared for write operation\r\n");
            break;
        }
        case COMPARE_ERROR:{
            printf("Compare error\r\n");
            break;
        }
        case BUSY:{
            printf("Busy\r\n");
            break;
        }  
        default:{
            printf("Status code %d not recognized", status);
            break;
        }  
    }
    return status;
}

