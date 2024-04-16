#include <avr/io.h>
#include <util/delay.h>

#define DEBOUNCE_TIME 50 // Temps de rebond en millisecondes

// DDRX (Data Direction Register),
// où X représente le port spécifique (par exemple, B ou D)

// Avec |, vous ajoutez ou activez des caractéristiques.
// Avec &, vous retirez ou désactivez des caractéristiques.
// Attention Activer un pin
// en entrée se fait par la "désactivation" de sa fonction de sortie.

void setup()
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

int main(void)
{
	setup();
	while (1)
	{
		if (!(PIND & (1 << PD2)))
		{
			// Allumer la led
			PORTB |= (1 << PB0);
			// Anti-rebond
			_delay_ms(DEBOUNCE_TIME);
		}
		else
		{
			// Eteindre la LED
			PORTB &= ~(1 << PB0);
		}
	}
	return 0;
}