#include <avr/io.h>
#include <util/delay.h>
#include "i2c.h"
#include "expander.h"

uint8_t nbrs[11];
uint8_t chif[4];

void exp_transmit_cmd(uint8_t *cmd, uint8_t nb_octets)
{
	i2c_start();								 // Commence une communication I2C
	i2c_write(PCA9555_ADDRESS << 1 | I2C_WRITE); // Envoie l'adresse de l'expandeur avec le bit d'écriture
	for (uint8_t i = 0; i < nb_octets; ++i)
	{
		i2c_write(cmd[i]); // Envoie les commandes byte par byte
	}
	i2c_stop(); // Termine la communication
}

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
	nbrs[10] = 0b01000000;

	chif[0] = 0b01111111;
	chif[1] = 0b10111111;
	chif[2] = 0b11011111;
	chif[3] = 0b11101111;
}

// Fonction pour sélectionner le PCA9555PW et configurer les broches en sortie
void setup_pca9555()
{
	i2c_start();
	i2c_write(0x20 << 1 | 0); // Remplacer par l'adresse correcte
	i2c_write(0x06);		  // Adresse du registre de configuration
	i2c_write(0b00000001);	  // Configurer la broche IO0_7 en sortie, selection chiffre
	i2c_write(0b00000000);	  // Configurer les broches IO1_0 à IO1_7 en sortie, 7 segments et point
	i2c_stop();
}

void turn_on_nbr(uint8_t n, uint8_t c)
{
	i2c_start();
	i2c_write(0x20 << 1);
	i2c_write(0x02);		// Registre de sortie
	i2c_write(chif[c - 1]); // Selection du chiffre
	i2c_write((nbrs[n]));	// Activation des bons segments
	i2c_stop();
}

void manual_turn_on(uint8_t a, uint8_t b)
{
	i2c_start();
	i2c_write(0x20 << 1);
	i2c_write(0x02); // Registre de sortie
	i2c_write(a);	 // Selection du chiffre
	i2c_write(b);	 // Activation des bons segments
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

void startRush(void)
{
	init_nbrs();
	// Configurer les LED en sortie (pour le portB)
	DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB4);

	uint16_t i = 300;
	while (i-- > 0)
	{
		// Active les 4 leds
		PORTB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4);

		// Configuration du PCA9555PW - broches en mode sortie pour les 7 segments
		// setup_pca9555();
		turn_off_nbrs();
		turn_on_nbr(8, 1);
		turn_off_nbrs();
		turn_on_nbr(8, 2);
		turn_off_nbrs();
		turn_on_nbr(8, 3);
		turn_off_nbrs();
		turn_on_nbr(8, 4);
		turn_off_nbrs();
		_delay_ms(5);
	}
	turn_off_nbrs();
	PORTB &= ~(1 << PB0) & ~(1 << PB1) & ~(1 << PB2) & ~(1 << PB4);
	_delay_ms(1000);
}