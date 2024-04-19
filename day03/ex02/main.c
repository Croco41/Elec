#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define UBRR 8 // Normal mode p165

void uart_init()
{
	// configurer le baud rate //p149
	UBRR0H = (uint8_t)(UBRR >> 8);
	UBRR0L = (uint8_t)UBRR;

	// activer le recepteur et transmetteur
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);

	// formats de donnees (8bits, pas de parite, 1 bit de stop)
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

/*attendre activement que le bit RXC0 du registre UCSR0A soit mis à 1,
ce qui indique qu'un caractère est disponible pour être lu dans le registre de données UDR0.
Dès qu'un caractère est disponible, il est retourné par la fonction.*/
char uart_rx(void)
{
	while (!(UCSR0A & (1 << RXC0)))
		; // RXCO sera set quand il y aura de la data nonlue dans UDR0

	return UDR0;
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

int main(void)
{
	char received_char;

	uart_init();

	while (1)
	{
		received_char = uart_rx();
		uart_tx(received_char + 1);
	}

	return 0;
}
