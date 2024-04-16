#include <avr/io.h>

/*TIMER 1
16 bits : Contrairement aux timers de 8 bits (comme TIMER0 et TIMER2 sur le même microcontrôleur),
TIMER1 est un timer de 16 bits, ce qui signifie qu'il peut compter jusqu'à 65535 (2^16 - 1)
avant de revenir à zéro (débordement).

Mode PWM (Pulse Width Modulation) : Génération de signaux PWM pour contrôler des moteurs, des LEDs, etc.
*/

// F_CPU 16000000UL // Fréquence du CPU en Hz, par exemple 16 MHz

// Mode CTC (Clear Timer on Compare Match) : Le compteur est réinitialisé à zéro lorsqu'il correspond à une certaine valeur.

void setup()
{
	DDRB |= (1 << DDB1);

	TCCR1A |= 1 << COM1A0;				  // PB1 on set OC1A (PB1)
	TCCR1B |= (1 << WGM12) | (1 << CS12); //  // pour WGM12=1 et WGM13=0, puis CS12=1/CS11=0/CS10=0
	// pour prédiviseur réglé sur division par 256
	//"CTC" (c'est à dire, en mode "comparaison timer <-> valeur de référence")

	TCNT1 = 0; // Mise du timer 0;
	// 16 000 000/ 256
	OCR1A = 31250; // Valeur correspondant à 500ms (car 31250 fois 16 µS donne bien 500ms ;
				   // pour rappel, ces 16 µS proviennent du calcul (16MHz/256/2) ,
				   // au réglage du prédiviseur du timer1
}

int main(void)
{
	setup();
	while (1)
	{
	}
}