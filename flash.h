#ifndef _FLASH_H_
#define _FLASH_H_

#define CMD_SUCCESS 0
#define INVALID_COMMAND 1
#define SRC_ADDR_ERROR 2
#define DST_ADDR_ERROR 3
#define SRC_ADDR_NOT_MAPPED 4
#define DST_ADDR_NOT_MAPPED 5
#define COUNT_ERROR 6
#define INVALID_SECTION 7
#define SECTOR_NOT_BLANK 8
#define SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION 9
#define COMPARE_ERROR 10
#define BUSY 11

int writeFlash(unsigned int dest, unsigned int source, unsigned int numBytes);

int eraseSector(unsigned int startSec, unsigned int endSec);

unsigned int printFlashStatus(unsigned int status);

static int prepareWrite(unsigned int dest, unsigned int numBytes);

#endif /* _FLASH_H_ */
