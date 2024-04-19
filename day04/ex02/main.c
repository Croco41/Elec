#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>

#define AHT20_ADDR 0x38 // Adresse I2C du AHT20
#define UBRR 8			// Normal mode p165

uint32_t data_buffer[7];
float humidity = 0.0;
float temperature = 0.0;

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

void print_hexa(uint8_t num)
{
	char hex_digits[] = "0123456789ABCDEF";
	uart_tx(hex_digits[num >> 4]);	 // Envoie le chiffre des quatre bits de poids fort
	uart_tx(hex_digits[num & 0x0F]); // Envoie le chiffre des quatre bits de poids faible
}

void uart_print(const char *str)
{
	while (*str)
	{
		uart_tx(*str++);
	}
}

/*-------------------------------------------------------------------------------*/

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

/*
Humidité : les deux premiers octets et les 4 premiers bits du troisième octet.
Température : les 4 derniers bits du troisième octet et les quatrième et cinquième octets
*/

void process_sensor_data()
{
	// Les données sont réparties selon votre spécification :
	// Humidité : les deux premiers octets et les 4 premiers bits du troisième octet.
	uint32_t raw_humidity = ((uint32_t)data_buffer[1] << 12) | ((uint32_t)data_buffer[2] << 4) | ((uint32_t)data_buffer[3] >> 4);

	// Température : les 4 derniers bits du troisième octet et les quatrième et cinquième octets
	uint32_t raw_temperature = (((uint32_t)data_buffer[3] & 0x0F) << 16) | ((uint32_t)data_buffer[4] << 8) | data_buffer[5];

	// Convertir les données brutes en valeurs utilisables selon les formules spécifiées par le datasheet du AHT20
	humidity = (raw_humidity * 100.0) / 1048576.0;				  // Ajustez la formule en fonction du datasheet
	temperature = ((raw_temperature * 200.0) / 1048576.0) - 50.0; // Ajustez la formule en fonction du datasheet
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

		// i2c_stop();
		// i2c_stop();

		process_sensor_data(); // Traitement des données pour calculer température et humidité

		char str_h[10];
		char str_te[10];

		dtostrf(humidity, 4, 2, str_h);
		dtostrf(temperature, 4, 2, str_te);

		// uart_print("\r\n");

		uart_print("Temperature = ");
		uart_print((const char *)str_te);
		uart_print("ºC, Humidity = ");
		uart_print((const char *)str_h);
		uart_print("%\r\n");

		_delay_ms(1000); // Attendre 1 seconde avant de répéter le processus
	}

	return 0;
}
