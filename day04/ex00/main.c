#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>

#define AHT20_ADDR 0x38 // Adresse I2C du AHT20
#define UBRR 8			// Normal mode p165

// https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf#page=114&zoom=100,45,68
//  p175

// 1- Initialiser l'I2C
//  I2C du MCU a 100khz
// 181-182-183

/*=> définir la vitesse de l'horloge I2C en configurant le registre TWBR (TWI Bit Rate Register)

si prescaler (defini dans TWSR) est a 1 (les deux bits de poids le plus faible de TWSR sont nuls)

TWBR = (Fcpu -16*SCL Frequency /(2 * prescaler Value))
donc
= (16 000 000 -16*100 000)/(2*1) =+-72
*/

void uart_init()
{
	// configurer le baud rate //p149
	UBRR0H = (uint8_t)(UBRR >> 8);
	UBRR0L = (uint8_t)UBRR;

	// activer le recepteur et transmetteur
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);

	// formats de donnees (8bits, pas de parite, 1 bit de stop)
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

	// UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0); // Activer RX, TX et l'interruption RX
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

void i2c_init(void)
{
	TWSR = 0;  // Set prescaler to 1
	TWBR = 72; // Set SCL to 100kHz at 16MHz CPU
}

/*Étape 2: Démarrer une Communication I2C

La fonction i2c_start doit envoyer une condition de start sur le bus I2C
et attendre que cette opération soit complétée.*/
// p183

/*Supposons que l'adresse de base d'un capteur soit 0x38 (en hexadécimal,
ce qui est couramment utilisé pour les adresses I2C). Lorsque vous voulez communiquer avec ce capteur,
vous devez d'abord envoyer cette adresse suivie du bit de lecture ou d'écriture :

	Pour une écriture, vous envoyez 0x38 << 1 | 0 = 0x70 (en décimal, 112).
	Pour une lecture, vous envoyez 0x38 << 1 | 1 = 0x71 (en décimal, 113).

Le décalage de bit (<< 1) dans ces calculs déplace l'adresse de 7 bits sur la gauche
pour laisser de la place pour le bit de lecture/écriture à la position la plus basse (bit 0).*/

void i2c_start(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // Envoyer une condition de start
	while (!(TWCR & (1 << TWINT)))
		; // Attendre la fin de la transmission de la condition de start

	// Envoyer l'adresse du périphérique avec le bit R/W
	TWDR = (AHT20_ADDR << 1) | TW_WRITE; // TW_WRITE est généralement défini à 0
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)))
		;

	uint8_t status = TWSR & 0xF8; // mask status
	//&& status != TW_MR_SLA_ACK)
	if (status != TW_MT_SLA_ACK)
	{
		uart_print("Erreur I2C, statut reçu : ");
		print_hexa(status);
		uart_print("\r\n");
	}
	else
	{
		print_hexa(status);
		uart_print("\r\n");
	}
	// uint8_t status = TWSR & 0xF8; // mask status
	// if (status != TW_START && status != TW_REP_START)
	// {
	// 	printf("Erreur I2C, statut reçu : %x\n", status);
	// 	return;
	// }
}

/*Étape 3: Arrêter une Communication I2C
La fonction i2c_stop envoie une condition de stop pour terminer la communication avec le capteur.
*/

void i2c_stop(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN); // Transmettre stop condition
													  // No waiting needed here as stop is an asynchronous operation
}
/*Programme Principal
Dans votre programme principal, vous pouvez utiliser ces fonctions
pour communiquer avec le capteur AHT20 et surveiller le statut de chaque opération.
*/

int main(void)
{
	uart_init();

	i2c_init();
	while (1)
	{
		i2c_start();
		// Assumption: here you would typically send the device address and check for ACK
		// Handling of data to/from the sensor
		i2c_stop();
		_delay_ms(1000);
		// Optionally print status to UART or handle it as required
	}

	return 0;
}
