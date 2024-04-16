#include <avr/io.h>
#include <util/delay.h>

#define DEBOUNCE_TIME 50 // Temps de rebond en millisecondes

// DDRX (Data Direction Register),
// où X représente le port spécifique (par exemple, B ou D)

// Avec |, vous ajoutez ou activez des caractéristiques.
// Avec &, vous retirez ou désactivez des caractéristiques.
// Attention Activer un pin
// en entrée se fait par la "désactivation" de sa fonction de sortie.

void init_setup()
{
	// Configurer le pin de la LED en sortie (pour le portB)
	DDRB |= (1 << DDB0);
	// Configurer le pin du bouton en entree (port D)
	//  1<<PD2 Si BUTTON est 2, alors 1 << BUTTON donne 0b00000100.
	//~ un opérateur de complément à un binaire.
	// Il inverse tous les bits du résultat de 1 << BUTTON. Ainsi, ~(0b00000100) devient 0b11111011.
	// Ce bit pour PD2 est mis à 0.(on desactive la fonction de sortie)
	DDRD &= ~(1 << DDD2);
	// Activer la resistance de pull-up pour le bouton
	// Son rôle est de maintenir le pin à un état logique haut (HIGH) quand aucun autre dispositif
	// n'est connecté ou quand un dispositif externe laisse ce pin dans un état flottant (non connecté).
	PORTD |= (1 << PD2);
	// permet d'eviter les etats flottants
}

// opérateur XOR bit à bit (^=) pour basculer (toggle)
// l'état d'un pin spécifique sur un port de microcontrôleur AVR
void shift_led(void)
{
	PORTB ^= (1 << PB0);

	// L'opération XOR a deux propriétés importantes : 0 ^ 1 = 1 et 1 ^ 1 = 0.
	// Ainsi, l'utilisation de XOR avec un bit à 1 a pour effet de basculer ce bit
	//: si le bit était à 1, il devient 0, et si le bit était à 0, il devient 1
}

int main(void)
{
	init_setup();
	// un entier non signé de 8 bits
	// une variable qui ne nécessite qu'une plage de valeurs de 0 à 255
	// aide à économiser de l'espace mémoire précieux
	uint8_t button_pressed_last = 0;

	while (1)
	{
		// verifier si le bouton est presse?
		if (!(PIND & (1 << PD2)))
		{
			if (!button_pressed_last)
			{
				shift_led();
				button_pressed_last = 1;
				// Anti-rebond
				_delay_ms(DEBOUNCE_TIME);
			}
		}
		else
		{
			button_pressed_last = 0;
		}
	}
	return 0;
}