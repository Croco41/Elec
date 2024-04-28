
#include <avr/io.h>
#include <util/delay.h>

#define UBRR 8 // Normal mode p165

/***********************************************************/
// void uart_init()
// {
// 	// configurer le baud rate //p149
// 	UBRR0H = (uint8_t)(UBRR >> 8);
// 	UBRR0L = (uint8_t)UBRR;

// 	// activer le recepteur et transmetteur
// 	UCSR0B = (1 << RXEN0) | (1 << TXEN0);

// 	// formats de donnees (8bits, pas de parite, 1 bit de stop)
// 	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
// }

// void uart_tx(char c)
// {
// 	// attendre que le transmit buffer soit vide
// 	while (!(UCSR0A & (1 << UDRE0)))
// 	{
// 	}
// 	// data dans le buffer, envoi du buffer
// 	UDR0 = c;
// }

// char uart_rx(void)
// {
// 	while (!(UCSR0A & (1 << RXC0)))
// 		;
// 	return UDR0;
// }

// void print_hexa(uint8_t num)
// {
// 	char hex_digits[] = "0123456789ABCDEF";
// 	uart_tx(hex_digits[num >> 4]);	 // Envoie le chiffre des quatre bits de poids fort
// 	uart_tx(hex_digits[num & 0x0F]); // Envoie le chiffre des quatre bits de poids faible
// }

// void uart_print(const char *str)
// {
// 	while (*str)
// 	{
// 		uart_tx(*str++);
// 	}
// }

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
}

void i2c_request_data(uint8_t ack)
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (ack << TWEA); // Configure pour recevoir avec ACK ou NACK
}

uint8_t i2c_read(uint8_t ack)
{
	TWCR = (1 << TWINT) | (ack ? (1 << TWEA) : 0) | (1 << TWEN); // Configurer pour lire avec ACK ou pas
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
#define PCA_IN0_REG 0x00  // en entree

#define I2C_READ 1
#define I2C_WRITE 0

// Fonction pour lire l'état du bouton
uint8_t read_button(void)
{
	i2c_start();
	i2c_write(PCA_ADDR << 1 | I2C_WRITE);
	i2c_write(PCA_IN0_REG);
	i2c_start();
	i2c_write(PCA_ADDR << 1 | I2C_READ);
	uint8_t button_state = i2c_read(0); // Lecture sans ACK car nous ne lisons qu'un seul octet
	i2c_stop();
	return button_state;
}

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

// Fonction pour mettre à jour l'état des LEDs en fonction de la valeur du compteur
void update_leds(uint8_t counter)
{
	uint8_t led_state = 0b11111111 & ~(counter << 1); // Les LEDs sont sur les bits 5, 6 et 7 de PORT0
	uint8_t cmd[] = {PCA_OUT0_REG, led_state};
	exp_transmit_cmd(cmd, 2);
}

int main()
{
	// uart_init();
	i2c_init();
	uint8_t counter = 0; // Initialise le compteur
	uint8_t buttonState = 0;
	uint8_t lastButtonState = 1;

	uint8_t enable_o0_3[] = {PCA_CNF0_REG, 0b11110001}; // Configuration pour le pin 3, 2, 1 du port 0

	// Commande pour mettre le pin 3 du port 0 à l'état haut
	// La valeur 0x08 est le résultat de 1 << 3, ce qui met le pin 3 à l'état haut
	uint8_t set_o0_3[] = {PCA_OUT0_REG, 0b11110001}; // Active le pin 3

	// Commande pour mettre le pin 3 du port 0 à l'état bas
	// La valeur 0x00 met tous les pins à l'état bas
	uint8_t clear_o0_3[] = {PCA_OUT0_REG, 0xFF}; // Désactive le pin 3

	uint8_t enable_sw3_0[] = {PCA_CNF0_REG, 0b11111110}; // sw3
	uint8_t set_sw3_0[] = {PCA_IN0_REG, 0b11111110};
	uint8_t clear_sw3_3[] = {PCA_IN0_REG, 0xFF}; // Désactive le sw3

	// Active le pin 3 comme sortie sur l'expander
	exp_transmit_cmd(enable_o0_3, 2);
	// exp_transmit_cmd(set_o0_3, 2);
	exp_transmit_cmd(clear_o0_3, 2);

	while (1)
	{

		i2c_start();
		i2c_write(0x20 << 1);
		i2c_write(0x00);

		i2c_start();
		i2c_write(0x20 << 1 | 1);
		buttonState = i2c_read(0);

		if (!(buttonState & 0b00000001))
		{
			if (buttonState != lastButtonState)
			{
				counter++;
				if (counter > 7)
				{
					counter = 0;
				}
				update_leds(counter);
				_delay_ms(200);
			}
		}

		lastButtonState = buttonState; // Mémoriser l'état actuel du bouton
		_delay_ms(50);				   // Debounce delay
	}
}