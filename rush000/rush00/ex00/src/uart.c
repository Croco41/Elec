#include "uart.h"

void uart_init(void)
{
    // Set baud rate
    UBRR0H = (uint8_t)((UBRR_VALUE >> 8) & 0xFF);
    UBRR0L = (uint8_t)(UBRR_VALUE & 0xFF);

    // Enable receiver and transmitter
    UCSR0B |= (1 << TXEN0) | (1 << RXEN0);

    //Disable all interrupt
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

// Fonction pour transmettre un caractère sur l'UART
void uart_tx(char c)
{
    // Attendre que le tampon de transmission soit vide
    while (!(UCSR0A & (1 << UDRE0)));

    // Placer les données dans le tampon, envoyer
    UDR0 = (uint8_t)(c);
}

uint8_t uart_rx(void)
{
    // Attendre que le tampon de réception soit plein
    while (!(UCSR0A & (1 << RXC0)));

    // Retourner les données reçues
    return UDR0;
}

void uart_printstr(const char *str)
{
    while (*str)
    {
        uart_tx(*str);
        str++;
    }
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
    uart_printstr((const char *)hex);
}
