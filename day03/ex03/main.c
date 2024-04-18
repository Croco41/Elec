#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define UBRR 8 // Normal mode p165

void uart_init()
{
	// configurer le baud rate //p149
	UBRR0H = (uint8_t)(UBRR >> 8);
	UBRR0L = (uint8_t)UBRR;

	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0); // Activer RX, TX et l'interruption RX

	// formats de donnees (8bits, pas de parite, 1 bit de stop)
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	sei();
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

ISR(USART_RX_vect)
{
	char c = UDR0; // Lire le caractère reçu
				   // Transformer le caractère en minuscule s'il est en majuscule et vice versa
	if (c >= 'A' && c <= 'Z')
	{
		c += 'a' - 'A'; // Conversion en minuscule
	}
	else if (c >= 'a' && c <= 'z')
	{
		c -= 'a' - 'A'; // Conversion en majuscule
	}
	// if (islower(c))
	// {
	// 	c = toupper(c); // Convertir en majuscule si c'est une minuscule
	// }
	// else if (isupper(c))
	// {
	// 	c = tolower(c); // Convertir en minuscule si c'est une majuscule
	// }
	uart_tx(c); // Envoyer le caractère transformé
}

int main(void)
{
	uart_init();

	while (1)
	{
	}

	return 0;
}
