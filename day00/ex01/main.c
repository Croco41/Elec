#include <avr/io.h>

// page 84 de la doc sur les ports chapitre 14

// allumer la led PB0
// utilise les registres de microcontrôleur AVR pour allumer une LED connectée au pin PB0 du port B.
// Cela implique de modifier les registres de direction des données (DDRx)
// et les registres de port (PORTx) pour contrôler les états des pins.

// DDRB est le registre de direction des données pour le port B.
// Chaque bit de ce registre contrôle la direction d'un pin correspondant
// sur le port B : un bit à 1 configure le pin comme une sortie,
// tandis qu'un bit à 0 le configure comme une entrée.

int main(void)
{
	// Configure le pin PB0 comme sortie
	// DDRB = Data Direction Register pour le Port B
	// Le bit 0 du DDRB est mis à 1 pour configurer PB0 comme sortie
	// L'opérateur |= est utilisé pour effectuer un OU binaire
	// entre l'ancienne valeur de DDRB et la nouvelle(si 0 alors 1 0b00000001(1verslagauche))
	DDRB |= (1 << DDB0); // PB0 en sortie

	// Met le pin PB0 à HIGH pour allumer la LED
	// PORTB = Data Register pour le Port B
	// Le bit 0 du PORTB est mis à 1 pour envoyer un niveau haut à PB0
	PORTB |= (1 << PB0); // PB0 a 1
	while (1)
	{
	}

	return 0;
}