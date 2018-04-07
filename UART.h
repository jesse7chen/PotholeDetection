#ifndef _UART_H_
#define _UART_H_

void UART_init(void);

int UART_read(char* c);

char UART_read_blocking(void);

char UART_write(char c);

int UART_read_string(char* s, int maxLength);

int UART_write_string(char* s);




#endif /* _UART_H_ */
