#include "uart.h"
#include "i2c.h"
#include <util/delay.h>
#include <avr/interrupt.h>

volatile uint8_t master = 0;
volatile uint8_t setup = 0;

void i2c_init_slave(void)
{
	TWAR = SLAVE_ADDR << 1;							// Configurer l'adresse de l'esclave
	TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWIE); // Activer TWI, l'acquittement et l'interruption
	sei();											// Activer les interruptions globales
}

volatile uint8_t data_received = 0;

ISR(TWI_vect)
{
	switch (TWSR & 0xF8)
	{
	case TW_SR_SLA_ACK:						// SLA+W a été reçu; ACK a été retourné
	case TW_SR_DATA_ACK:					// Données reçues; ACK retourné
		data_received = TWDR;				// Lire les données
		TWCR |= (1 << TWINT) | (1 << TWEA); // Préparer à recevoir plus de données
		break;

	case TW_SR_STOP:						// STOP ou REPEATED START a été reçu
		TWCR |= (1 << TWINT) | (1 << TWEA); // Préparer à recevoir plus de données
		break;

	default:
		TWCR |= (1 << TWINT) | (1 << TWEA); // Réponse par défaut
		break;
	}
}

int main(void)
{
	DDRD &= ~(1 << PD2); // PD2 (INT0) comme entrée
	PORTD |= (1 << PD2); // Activer la résistance de tirage interne
	uart_init();
	i2c_init();
	// setup_addr(SLAVE_ADDR);
	i2c_init_slave();

	while (!setup)
	{
		if (!(PIND & (1 << PD2)) && !master)
		{
			master = 1;
			setup = 1;
			setup_addr(MASTER_ADDR);
			uart_printstr("Je suis le maître\r\n");
			i2c_start();
			i2c_write((SLAVE_ADDR << 1), 0);
			i2c_write('S', 0);
			i2c_stop();
			_delay_ms(10);
			break;
		}

		else if (data_received == 'S')
		{
			setup = 1;
			uart_printstr("Je suis l'esclave\r\n");
			data_received = 0; // Réinitialiser le flag
			break;			   // Sortie de la boucle si le message "Tu es l'esclave" est reçu
		}
	}
	if (master)
	{
		uart_printstr("Je suis le maître\r\n");
	}
	else
	{
		uart_printstr("Je suis l'esclave\r\n");
	}
}