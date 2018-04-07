#ifndef _SPI_H_
#define _SPI_H_

#include <stdint.h>

void SPI_init(void);

int SPI_read(uint16_t* c);

int SPI_write(uint16_t c);

#endif /* _SPI_H_ */
