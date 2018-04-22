#include "bluetooth.h"
#include "SPI.h"
#include "LPC11xx.h"
#include "timer.h"
#include <stdio.h>
#include <string.h>

/* SPI Hardware requirements
The SPI clock should run <=4MHz
A 100us delay should be added between the moment that the CS line is asserted, and before any data is transmitted on the SPI bus
The CS line must remain asserted for the entire packet, rather than toggling CS every byte
The CS line can however be deasserted and then reasserted between individual SDEP packets (of up to 20 bytes each).
The SPI commands must be setup to transmit MSB (most significant bit) first (not LSB first)
*/

#define CS_PIN 2 // Pin is 0_2

/*
// Code to reverse bitstring
static const unsigned char lookup[16] = {
0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf};

static uint8_t reverse(uint8_t n) {
   // Reverse the top and bottom nibble then swap them.
   return (lookup[n&0xf] << 4) | (lookup[n>>4]);
}
*/

int bleInit(void){
    //set port 0_2 to output, as that is our chip select
    LPC_GPIO0->DIR |= (1<<CS_PIN);
    // Drive CS high to de-assert line
    LPC_GPIO0->DATA |= (1 << CS_PIN);
    
    // Switch module to data mode (doesn't matter actually)
    //bleSendAT(AT_SWITCH_MODE, AT_SWITCH_MODE_LEN);

    //threadWait(24000000);
    
    return 0;
}

int bleWriteLocation(location_t loc){
    char tempBuffer[120];
    //char* tempBuffer = "Latitude: 40.110590\r\nLongitude: -88.229039\r\nSpeed: 0.03896\r\n";
    int len;
    /*
    switch(loc.status){
        case(valid):{
            bleWriteUART("Valid\r\n", 7);
            break;
        }
        case(invalid):{
            bleWriteUART("Invalid\r\n", 9);
            break;
        }
        case(stale):{
            bleWriteUART("Stale\r\n", 7);
            break;
        }
    }*/
    sprintf(tempBuffer, "\r\nLatitude: %f\r\nLongitude: %f\r\n", loc.latitude, loc.longitude);
    //sprintf(tempBuffer, "Latitude: %f\r\nLongitude: %f\r\nSpeed: %f\r\nBearing: %f\r\n", loc.latitude, loc.longitude, loc.speed, loc.bearing);
    //sprintf(tempBuffer, "Latitude: %g\r\nLongitude: %g\r\nSpeed: %g\r\n", loc.latitude, loc.longitude, loc.speed);
    //sprintf(tempBuffer, "Double size: %d\r\n, float size: %d\r\n", sizeof(double), sizeof(float));
    len = strlen(tempBuffer);
    return bleWriteUART(tempBuffer, len);  
}

int bleWriteUART(char* s, uint8_t len){
    // Write single byte to bluetooth UART TX
    return sendMultSDEP(COMMAND_MSG_TYPE, SDEP_CMDTYPE_BLE_UARTTX, len, (uint8_t*)s);
}

int bleSendAT(char* cmd, uint8_t len){
    return sendSDEP(COMMAND_MSG_TYPE, SDEP_CMDTYPE_AT_WRAPPER, len, (uint8_t*)cmd);
}


// Max payload length is 16 bytes
int sendSDEP(uint8_t msgType, uint16_t cmdID, uint8_t len, uint8_t* payload){
    int i;
    int retVal = 0;
    int numPackets = (len&0x7F);

    
    // Enable CS by driving line low
    LPC_GPIO0->DATA &= ~(1 << CS_PIN);
    
    // Wait <100us
    threadWait(5000);
    
    // Write message type
    retVal |= SPI_write(msgType);
    // Write cmdID, lowest byte first
    retVal |= SPI_write(cmdID & 0x00FF);
    retVal |= SPI_write(cmdID >> 8);
    // Write payload length
    retVal |= SPI_write(len);
        
    for(i = 0; i < numPackets; i++){
        retVal |= SPI_write((payload[i]));
    }
    
    // Wait a bit before driving line high again, perhaps to get bytes on MOSI?
    threadWait(5000);
    
    // Disable CS by driving line high
    LPC_GPIO0->DATA |= (1 << CS_PIN);
    
    return retVal;   
}

int sendMultSDEP(uint8_t msgType, uint16_t cmdID, uint8_t len, uint8_t* payload){
    int packetLen;
    int bytesLeft = len;
    int retVal = 0;
        
    while(bytesLeft > 0){
       packetLen = bytesLeft; 
       if(packetLen > 16){
           // If length greater than 16, set len to 16 and set flag to show that more packets are coming
           packetLen = (16 | (1UL << 7));
       }
      // Wait 4ms before sending packet. Not sure why, but it works!
       threadWait(192000);
       sendSDEP(msgType, cmdID, packetLen, payload+(len-bytesLeft));
       bytesLeft -= 16;
    }
    
    return retVal;
}

