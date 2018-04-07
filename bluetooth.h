#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_

#include <stdint.h>
#include "GPS.h"

#define SDEP_CMDTYPE_INITIALIZE 0xBEEF
#define SDEP_CMDTYPE_AT_WRAPPER 0x0A00
#define SDEP_CMDTYPE_BLE_UARTTX 0x0A01
#define SDEP_CMDTYPE_BLE_UARTRX 0x0A02

#define COMMAND_MSG_TYPE 0x10
#define RESPONSE_MSG_TYPE 0x20
#define ALERT_MSG_TYPE 0x40
#define ERROR_MSG_TYPE 0x80

#define AT_SWITCH_MODE "+++"
#define AT_SWITCH_MODE_LEN 3

#define AT_WRITE_UART_TEST "AT+BLEUARTTX=A\n"
#define AT_WRITE_UART_TEST_LEN 15
#define AT_LED_OFF "AT+HWMODELED=1\n"
#define AT_LED_OFF_LEN 15

int bleInit(void);

int bleWriteUART(char* s, uint8_t len);

int bleSendAT(char* cmd, uint8_t len);

int sendSDEP(uint8_t msgType, uint16_t cmdID, uint8_t len, uint8_t* payload);

int sendMultSDEP(uint8_t msgType, uint16_t cmdID, uint8_t len, uint8_t* payload);

static uint8_t reverse(uint8_t n);

int bleWriteLocation(location_t loc);

#endif /* _BLUETOOTH_H_ */
