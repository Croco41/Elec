#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf#page=114&zoom=100,45,68

/*When the INT0 or INT1 interrupts are enabled and are configured as low level triggered (Type 2),
the interrupts will trigger as long as…

The pin is held low.
The low level is held until the completion of the currently executing instruction.*/

/*Pour utiliser INT0, vous devez configurer plusieurs registres :

	EICRA (External Interrupt Control Register A) : Définit le type de signal qui déclenchera l'interruption
	(front montant, front descendant, etc.). (p54 /294)
	EIMSK (External Interrupt Mask Register) : Active l'interruption INT0.
	EIFR (External Interrupt Flag Register) : Permet de voir si une interruption a été déclenchée.
*/

/*Vecteur d'Interruption

Un vecteur d'interruption est essentiellement une adresse dans la table des vecteurs d'interruption
où se trouve le pointeur vers la routine de service d'interruption (ISR) correspondant
à un type spécifique d'interruption. Quand une interruption est déclenchée,
le microcontrôleur consulte cette table pour savoir quelle fonction appeler.

Chaque type d'interruption a son propre vecteur d'interruption dans cette table,
ce qui permet au système d'exécuter rapidement le bon gestionnaire sans avoir besoin de vérifier
quel type d'interruption a été déclenché. Pour INT0, le vecteur d'interruption spécifique est INT0_vect,
et c'est là que vous définissez la fonction ISR correspondante.*/

void setup()
{
	DDRB |= (1 << DDB0);
	// PORTD |= (1 << PD2);
	//  permet d'eviter les etats flottants.

	// EICRA |= (1 << ISC01) | (1 << ISC00); // The rising edge of INT0 generates an interrupt request
	EICRA |= (1 << ISC01); // quand on appuie (le descendant)
	EIMSK |= (1 << INT0);  // activer INT0
						   // EIFR |=
	sei();				   // set interrupts Bit I du SREG: Le bit 7 (appelé I pour "Interrupt") dans le registre d'état SREG est mis a 1
}

ISR(INT0_vect)
{
	// code qui s'execute quand on appuie sur sw1 (INTO_vect)
	uint8_t button_pressed = 0;
	if (!button_pressed)
	{
		PORTB ^= (1 << PB0);
		button_pressed = 1;
		_delay_ms(50);
	}

	button_pressed = 0;
}

int main(void)
{
	setup();
	while (1)
	{
	}
}