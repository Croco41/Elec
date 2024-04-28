#include <avr/io.h>
#include <util/delay.h>

#define PCA9555_ADDRESS 0x20 // Adresse esclave du PCA9555PW

uint8_t nbrs[10];
uint8_t chif[4];

void init_nbrs()
{
	nbrs[0] = 0b00111111;
	nbrs[1] = 0b00000110;
	nbrs[2] = 0b01011011;
	nbrs[3] = 0b01001111;
	nbrs[4] = 0b01100110;
	nbrs[5] = 0b01101101;
	nbrs[6] = 0b01111101;
	nbrs[7] = 0b00000111;
	nbrs[8] = 0b01111111;
	nbrs[9] = 0b01101111;

	chif[0] = 0b01111111;
	chif[1] = 0b10111111;
	chif[2] = 0b11011111;
	chif[3] = 0b11101111;
}

// Fonction pour initialiser le bus I2C
void i2c_init()
{
	TWBR = 72;			// Pour une fréquence de 100 kHz : TWBR = 72
	TWCR = (1 << TWEN); // Activation du bus I2C
}

// Fonction pour envoyer un start condition sur le bus I2C
void i2c_start()
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)))
		; // Attendre que la condition de départ soit envoyée
}

// Fonction pour envoyer un stop condition sur le bus I2C
void i2c_stop()
{
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

// Fonction pour envoyer une donnée sur le bus I2C
void i2c_write(uint8_t data)
{
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)))
		; // Attendre que la donnée soit envoyée
}

uint8_t i2c_read(uint8_t ack)
{
	TWCR = _BV(TWINT) | (ack ? _BV(TWEA) : 0) | _BV(TWEN); // Configurer pour lire avec ACK ou pas
	if (ack)
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); // Recevoir le byte avec ACK
	else
		TWCR = (1 << TWINT) | (1 << TWEN); // Recevoir le byte sans ACK
	while (!(TWCR & (1 << TWINT)))
		; // Attendre que la donnée soit reçue
	return (TWDR);
}

// Fonction pour sélectionner le PCA9555PW et configurer les broches en sortie
void setup_pca9555()
{
	i2c_start();
	i2c_write(0x20 << 1 | 0); // Remplacer par l'adresse correcte
	i2c_write(0x06);		  // Adresse du registre de configuration
	i2c_write(0b00111111);	  // Configurer la broche IO0_7 en sortie, selection chiffre
	i2c_write(0b00000000);	  // Configurer les broches IO1_0 à IO1_7 en sortie, 7 segments et point
	i2c_stop();
}

void turn_on_nbrs(uint8_t n, uint8_t c)
{
	i2c_start();
	i2c_write(0x20 << 1);
	i2c_write(0x02);
	i2c_write(chif[c - 1]); // Selection du chiffre
	i2c_write((nbrs[n]));	// Activation des bons segments
	i2c_stop();
}

void turn_off_nbrs()
{
	i2c_start();
	i2c_write(0x20 << 1);
	i2c_write(0x02);
	i2c_write(0b11111111);
	i2c_write(0b00000000);
	i2c_stop();
}

int main()
{
	uint8_t count = 0;

	// Initialiser le bus I2C
	i2c_init();
	init_nbrs();

	// Configuration du PCA9555PW - broches en mode sortie pour les 7 segments et le choix chiffre 4
	setup_pca9555();

	while (1)
	{
		// Le programme principal continue ici
		turn_off_nbrs();
		turn_off_nbrs();
		turn_off_nbrs();
		turn_off_nbrs();
		turn_on_nbrs(4, 2);
		turn_off_nbrs();
		turn_off_nbrs();
		turn_off_nbrs();
		turn_off_nbrs();
		turn_on_nbrs(2, 1);
	}

	return 0;
}