#include "uart.h"
#include "i2c.h"
#include <util/delay.h>

volatile uint8_t slave = 1;
volatile uint8_t setup = 0;

#define BROADCAST_ADDR 0x02

uint8_t i2c_receive_check(void)
{
	TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWEA); // Préparer à recevoir
	uint8_t timeout = 0;
	while (!(TWCR & (1 << TWINT)) && (PIND & (1 << PD2)))
		;

	if ((TWSR & 0xF8) == TW_SR_SLA_ACK)
	{													 // Vérifier si SLA+R a été reçu et ACK envoyé
		uint8_t received_data = TWDR;					 // Lire les données reçues
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); // Préparer pour la prochaine réception
		return received_data;							 // Retourner les données reçues
	}
	return 0; // Aucune donnée valide reçue
}

int main(void)
{
	DDRD &= ~(1 << PD2); // PD2 (INT0) comme entrée
	PORTD |= (1 << PD2); // Activer la résistance de tirage interne
	uart_init();
	i2c_init();
	setup_addr(BROADCAST_ADDR);

	while (!setup)
	{
		if (!(PIND & (1 << PD2)))
		{
			_delay_ms(10);
			slave = 0;
			break;
		}
		uint8_t received_data = i2c_receive_check();
		if (received_data == 'S')
		{
			setup = 1;
			break; // Sortie de la boucle si le message "Tu es l'esclave" est reçu
		}
	}
	if (!slave)
	{
		uart_printstr("Je suis le maître\r\n");
	}
	else
	{
		uart_printstr("Je suis l'esclave\r\n");
	}
}