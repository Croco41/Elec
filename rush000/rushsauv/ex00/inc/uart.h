#ifndef UART_H
#define UART_H

#include <avr/io.h>

# define UBRR_VALUE ((uint16_t)(((float)(F_CPU / 16.0 / UART_BAUDRATE) + 0.5) - 1))

// UART
void    uart_init(void);
void    uart_tx(char c);
uint8_t uart_rx(void);
void    uart_printstr(const char *str);
void    print_hex_value(char c);

#endif