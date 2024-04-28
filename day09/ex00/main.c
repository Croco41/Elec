
#include <avr/io.h>
#include <util/delay.h>

#define UBRR 8 // Normal mode p165

/***********************************************************/
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

char uart_rx(void)
{
	while (!(UCSR0A & (1 << RXC0)))
		;
	return UDR0;
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

/*****************************************************/

/*-------------------------------------------------------------------------------*/

void i2c_init(void)
{
	TWSR = 0;  // Set prescaler to 1
	TWBR = 72; // Set SCL to 100kHz at 16MHz CPU 100000UL
			   // TWCR = (1 << TWEN); // Active l'I2C
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

	// uint8_t status = TWSR & 0xF8; // Lecture du statut
	// if (status != TW_MT_SLA_ACK && status != TW_MT_DATA_ACK && status != TW_MR_SLA_ACK)
	// {
	// 	uart_print("Erreur I2C, donnée non acquittée, statut reçu : ");
	// 	print_hexa(status);
	// 	uart_print("\r\n");
	// }
}

void i2c_request_data(uint8_t ack)
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (ack << TWEA); // Configure pour recevoir avec ACK ou NACK
}

uint8_t i2c_read(uint8_t ack)
{
	TWCR = _BV(TWINT) | (ack ? _BV(TWEA) : 0) | _BV(TWEN); // Configurer pour lire avec ACK ou pas
	while (!(TWCR & (1 << TWINT)))
		; // Attendre que la donnée soit reçue

	return (TWDR);
}

/*****************************************/
/**************************************************/
/*****************************************/
/*
En regardant la documentation fournie dans vos images :

	Table 4 montre que le Registre de configuration port 0 correspond à la commande 6.
	Ce qui justifie la définition #define EXP_CNF0_REG 0x06.

	Table 7 indique que le Registre de sortie port 0 correspond à la commande 2.
	 Ce qui explique la définition #define EXP_OUT0_REG 0x02.

Ces commandes sont utilisées comme premier octet après l'adresse de l'expander I2C
lors d'une transmission pour indiquer au PCF9555P quel registre vous souhaitez écrire ou lire.

Ainsi, selon la documentation :

	Pour configurer le port 0 comme sortie,
	vous écririez 0x06 suivi de la configuration du port.
	Pour changer l'état d'une sortie sur le port 0,
	vous écririez 0x02 suivi de l'état des pins souhaité.
*/

#define PCA_ADDR 0x20 // Adresse I2C de l'expandeur

#define PCA_CNF0_REG 0x06 // Registre de configuration du port 0 (100_3 lie au port 0)
#define PCA_OUT0_REG 0x02 // Registre de sortie du port 0

#define I2C_WRITE 0

void exp_transmit_cmd(uint8_t *cmd, uint8_t nb_octets)
{
	i2c_start();						  // Commence une communication I2C
	i2c_write(PCA_ADDR << 1 | I2C_WRITE); // Envoie l'adresse de l'expandeur avec le bit d'écriture
	for (uint8_t i = 0; i < nb_octets; ++i)
	{
		i2c_write(cmd[i]); // Envoie les commandes byte par byte
	}
	i2c_stop(); // Termine la communication
}

int main()
{
	// uart_init();
	i2c_init();

	uint8_t enable_o0_3[] = {PCA_CNF0_REG, 0b11110111}; // Configuration pour le pin 3 du port 0

	// Commande pour mettre le pin 3 du port 0 à l'état haut
	// La valeur 0x08 est le résultat de 1 << 3, ce qui met le pin 3 à l'état haut
	uint8_t set_o0_3[] = {PCA_OUT0_REG, 0b11110111}; // Active le pin 3

	// Commande pour mettre le pin 3 du port 0 à l'état bas
	// La valeur 0x00 met tous les pins à l'état bas
	uint8_t clear_o0_3[] = {PCA_OUT0_REG, 0xFF}; // Désactive le pin 3

	// Active le pin 3 comme sortie sur l'expander
	exp_transmit_cmd(enable_o0_3, 2);

	while (1)
	{
		// Met le pin 3 à l'état haut
		exp_transmit_cmd(set_o0_3, 2);
		_delay_ms(500); // Allume la LED pendant 500ms
		// Met le pin 3 à l'état bas
		exp_transmit_cmd(clear_o0_3, 2);
		_delay_ms(500); // Éteint la LED pendant 500ms
	}
}