#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>

#define UBRR 8 // Normal mode p165

/*-------------------Bouton------------------------*/

void button_init(void)
{
	// Configurer le bouton en entrée
	DDRD &= ~(1 << PD2);
	// Activer la résistance de pull-up
	BUTTON_PORT |= (1 << PD2);
}

/*-----------------------UART--------------------------------*/
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

void uart_tx(char c)
{
	// attendre que le transmit buffer soit vide
	while (!(UCSR0A & (1 << UDRE0)))
	{
	}
	// data dans le buffer, envoi du buffer
	UDR0 = c;
}

void uart_print(const char *str)
{
	while (*str)
	{
		uart_tx(*str++);
	}
}

//-------------------------------------------------
//                    *****************
//------------------------------------------------------

void i2c_init(void)
{
	TWSR = 0;  // Set prescaler to 1
	TWBR = 72; // Set SCL to 100kHz at 16MHz CPU
}

void i2c_start(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // Envoyer une condition de start
	while (!(TWCR & (1 << TWINT)))
		; // Attendre la fin de la transmission de la condition de start
}

void i2c_stop(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN); // Transmettre stop condition
													  // No waiting needed here as stop is an asynchronous operation
}

void i2c_write(unsigned char data)
{
	TWDR = data;					   // Chargement de la donnée dans TWDR
	TWCR = (1 << TWINT) | (1 << TWEN); // Début de la transmission
	while (!(TWCR & (1 << TWINT)))
		; // Attente de la fin de la transmission

	uint8_t status = TWSR & 0xF8; // Lecture du statut
	if (status != TW_MT_SLA_ACK && status != TW_MT_DATA_ACK && status != TW_MR_SLA_ACK)
	{
		uart_print("Erreur I2C, donnée non acquittée, statut reçu : ");
		print_hexa(status);
		uart_print("\r\n");
	}
}

void i2c_request_data(uint8_t ack)
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (ack << TWEA); // Configure pour recevoir avec ACK ou NACK
}

uint8_t i2c_read(void)
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); // Configurer pour lire avec ACK
	while (!(TWCR & (1 << TWINT)))
		; // Attendre que la donnée soit reçue

	return (TWDR);
	// data_buffer[i]
	// print_hexa(TWDR); // Afficher directement la donnée reçue
	//  process_sensor_data(TWDR);
}

/*
	Écouter le Bouton : Attendre que l'utilisateur appuie sur le bouton SW1.
	Vérifier le Bus : S'assurer que le bus est libre (pas d'activité en cours).
	Devenir Maître : Envoyer une condition de démarrage et tenter de communiquer en tant que maître.
	Gérer les Collisions : Si une autre tentative de prise de contrôle du bus a lieu en même temps,
	laisser le protocole I2C résoudre la collision.
	*/

// Fonction pour initialiser le bus I2C
void i2c_init_master(void)
{
	// Activer I2C en tant que maître
	TWSR = 0x00;		// Aucune division de fréquence
	TWBR = 0x48;		// TWBR doit être supérieur à 10 pour une fréquence de SCL de 100KHz
	TWCR = (1 << TWEN); // Activer I2C
}

// Fonction pour vérifier si le bus est libre
uint8_t i2c_bus_is_free(void)
{
	return !(TWCR & (1 << TWSTO)) && !(TWCR & (1 << TWSTA));
	// TWCR=TWI control register
	// si TWSTO == 1condition de stop a ete envoyee le bus i2c estlibre ou en transition. Si 0 le bus est occupe ou en transition
	// TWSTA==1 une conditio de start a ete envoyee
	//=> vrai si et seulement si ni une condition de stop (en cours de traitement) ni une condition de start ne sont actives,
	// ce qui indique que le bus est libre pour une nouvelle communication
}

int main(void)
{
	uart_init();

	i2c_init();

	while (1)
	{
		i2c_start();

		// TWDR = (AHT20_ADDR << 1) | TW_WRITE;
		i2c_write(0b01110000);
		i2c_write(0xAC); // 12/16 doc AHT20
		i2c_write(0x33); // change rien?
		i2c_write(0x00);
		i2c_stop();

		_delay_ms(80); // Petit délai pour laisser le temps au capteur de préparer les données

		i2c_start();
		// i2c_write((AHT20_ADDR << 1) | 1); // Envoi de l'adresse avec le bit de lecture
		i2c_write(0b01110001);

		for (int i = 0; i < 7; i++)
		{
			if (i == 6)
			{
				i2c_request_data(0); // Envoyer NACK après le dernier octet
			}
			else
			{
				i2c_request_data(1); // Envoyer ACK après chaque réception
			}
			data_buffer[i] = i2c_read();
			// uart_print(data_buffer[i]);
		}
		uart_print("\r\n"); // Nouvelle ligne après chaque lot de données

		_delay_ms(1000); // Attendre 1 seconde avant de répéter le processus
	}

	return 0;
}
