#include "uart.h"

void uart_init(void)
{
    // configurer le baud rate //p149
    UBRR0H = (uint8_t)(UBRR >> 8);
    UBRR0L = (uint8_t)UBRR;

    // activer le recepteur et transmetteur
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    // Disable all interrupt
    UCSR0B &= ~(1 << RXCIE0);
    UCSR0B &= ~(1 << TXCIE0);

    // Format des trames: 8 bits de données
    UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);
    // Asynchrone
    UCSR0C &= ~(1 << UMSEL00);
    UCSR0C &= ~(1 << UMSEL01);
    // 1 bit de stop
    UCSR0C &= ~(1 << USBS0);
    // Pas de parité
    UCSR0C &= ~(1 << UPM00);
    UCSR0C &= ~(1 << UPM01);
}

void uart_tx(char c)
{
    // attendre que le transmit buffer soit vide
    while (!(UCSR0A & (1 << UDRE0)))
    {
    }
    // data dans le buffer, envoi du buffer
    UDR0 = c;
}

uint8_t uart_rx(void)
{
    // Attendre que le tampon de réception soit plein
    while (!(UCSR0A & (1 << RXC0)))
        ;

    // Retourner les données reçues
    return UDR0;
}

void uart_print(const char *str)
{
    while (*str)
    {
        uart_tx(*str++);
    }
}

void print_hexa(uint8_t num)
{
    char hex_digits[] = "0123456789ABCDEF";
    uart_tx(hex_digits[num >> 4]);   // Envoie le chiffre des quatre bits de poids fort
    uart_tx(hex_digits[num & 0x0F]); // Envoie le chiffre des quatre bits de poids faible
}

void print_hex_value(char c)
{
    char hex[3];
    hex[0] = (c >> 4) & 0x0F;
    hex[1] = c & 0x0F;
    hex[2] = '\0';
    for (int i = 0; i < 2; i++)
    {
        if (hex[i] < 10)
            hex[i] += '0';
        else
            hex[i] += 'A' - 10;
    }
    uart_print((const char *)hex);
}
