#include "i2c.h"
#include "uart.h"
#include <util/delay.h>

volatile uint8_t delay_led = 2;

// fonction Maitre+esclave
void init_setup_led()
{
	// Configurer les LED en sortie (pour le portB)
	DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2);

	// Allumer toutes les LEDs (mettre PB0, PB1, PB2 à 1)
	PORTB |= (1 << PB0) | (1 << PB1) | (1 << PB2);

	// Semer le générateur de nombres aléatoires
	randomSeed(analogRead(0)); // Utiliser la lecture d'une pin non connectée pour le seed
}

// Fonction pour générer un délai aléatoire entre 1 et 10 secondes //Fonction Maitre
void random_delay()
{
	// Générer un nombre aléatoire entre 2 et 10
	uint8_t total_led = random(2, 11); // Produit un nombre de 2 à 10

	// Calculer le délai pour chaque LED
	delay_led = total_led * 1000 / 3;
}

// fonction Maitre+esclave
void countdown_led()
{

	_delay_ms(1000); // Attendre un peu avant de commencer à éteindre les LEDs

	// Éteindre PB2, laissant PB0 et PB1 allumés
	PORTB &= ~(1 << PB2);
	_delay_ms(delay_led); // Attendre le delaycalcule

	// Éteindre PB1, laissant PB0 allumé
	PORTB &= ~(1 << PB1);
	_delay_ms(delay_led);

	// Éteindre PB0
	PORTB &= ~(1 << PB0);
	_delay_ms(delay_led);
}

// fonction maitre
void send_delay_to_slave(uint8_t delay)
{
	i2c_start();							  // Commencer la transmission I2C
	i2c_send_address(SLAVE_ADDR + I2C_WRITE); // Adresse de l'esclave + bit d'écriture
	i2c_send_byte(delay);					  // Envoyer le délai
	i2c_stop();								  // Arrêter la transmission I2C
}

//  Sur le maître
void start_countdown()
{
	send_delay_to_slave(delay_led); // Envoyer le délai à l'esclave
	_delay_ms(100);					// Donner un peu de temps à l'esclave pour traiter le délai reçu
	countdown_led();				// Commencer le compte à rebours sur le maître
}

// RECEPTION ESCLAVE//
// fonction reception du delay par l'esclave

void setup_led_slave()
{
	i2c_init_slave(MY_SLAVE_ADDRESS); // Initialiser l'adresse de l'esclave
	sei();							  // Activer les interruptions globales
	init_setup_led();				  // Configurer les LEDs
	countdown_led();
	delay_led = 0; // Reinitialisation
}