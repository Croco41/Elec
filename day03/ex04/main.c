#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

#define F_CPU 16000000UL
#define BAUD 115200
#define MYUBRR 8

void uart_init(void)
{
	UBRR0H = (unsigned char)(MYUBRR >> 8);
	UBRR0L = (unsigned char)MYUBRR;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	sei();
}

void uart_tx(char c)
{
	while (!(UCSR0A & (1 << UDRE0)))
		;
	UDR0 = c;
}

char uart_rx(void)
{
	while (!(UCSR0A & (1 << RXC0)))
		;
	return UDR0;
}

void uart_printstr(const char *str)
{
	while (!(UCSR0A & (1 << UDRE0)))
		;

	uint16_t i;
	while (str[i])
	{
		uart_tx(str[i]);
		i++;
	}
}

#define BACKSPACE 0x7F
#define ENTER 0x0D

void led_init(void)
{
	DDRB |= 0x0F; // Configure PORTB pins 0 to 3 as outputs
}

void led_blink(void)
{
	for (int i = 0; i < 5; i++)
	{				   // Fait clignoter les LEDs 5 fois
		PORTB |= 0x0F; // Allume les LEDs
		_delay_ms(200);
		PORTB &= ~0x0F; // Éteint les LEDs
		_delay_ms(200);
	}
}

void get_string(char *str, int echo)
{
	char c;
	int i = 0;
	while ((c = uart_rx()) != ENTER)
	{
		if (c == BACKSPACE && i > 0)
		{
			uart_tx('\b'); // Envoie le caractère backspace au terminal pour reculer le curseur
			uart_tx(' ');  // Remplace le dernier caractère par un espace
			uart_tx('\b'); // Recule à nouveau le curseur
			i--;		   // Décrémente l'indice pour effacer le caractère du buffer
			str[i] = '\0'; // Termine la chaîne plus tôt pour ignorer le dernier caractère
		}
		else if (c >= ' ' && c <= '~')
		{
			str[i++] = c;
			uart_tx(echo ? c : '*');
		}
	}
	str[i] = '\0';
	uart_tx('\r');
	uart_tx('\n');
}

void check_credentials(void)
{
	const char *correct_username = "spectre";
	const char *correct_password = "password";

	char user_input[50]; // Buffer for username
	char pass_input[50]; // Buffer for password

	uart_printstr("Enter your login:\r\nusername: ");
	get_string(user_input, 1); // Echo visible for username

	uart_printstr("password: ");
	get_string(pass_input, 0); // Echo masked for password

	if (strcmp(user_input, correct_username) == 0 && strcmp(pass_input, correct_password) == 0)
	{
		uart_printstr("Hello ");
		uart_printstr(user_input);
		uart_printstr("!\r\nShall we play a game?\r\n");
		led_blink();
	}
	else
	{
		uart_printstr("Bad combination username/password\r\n");
	}
}

int main(void)
{
	uart_init();
	uart_tx('\n');
	check_credentials();
	while (1)
	{
	}
}
