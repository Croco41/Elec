#include <avr/io.h>

/*TIMER 1
16 bits : Contrairement aux timers de 8 bits (comme TIMER0 et TIMER2 sur le même microcontrôleur),
TIMER1 est un timer de 16 bits, ce qui signifie qu'il peut compter jusqu'à 65535 (2^16 - 1)
avant de revenir à zéro (débordement).

Mode PWM (Pulse Width Modulation) : Génération de signaux PWM pour contrôler des moteurs, des LEDs, etc.
*/

// F_CPU 16000000UL // Fréquence du CPU en Hz, par exemple 16 MHz

void setup()
{
	DDRB |= (1 << DDB1);

	// Parametrer le prescaler
	// les 16 MHz du quartz qui cadence l’ATmega328P seront divisés par 4,
	// ce qui nous donnera une fréquence de 4 MHz en sortie du prescaler
	// CLKPR = 0b10000000; // activation du prescaler (on met son1bit a 1)
	// CLKPR = 0b00000010; // on desire une division par 4 de la frequence principale

	// TCCR1A = 0b00000000; // pour mettre WGM10 et WGM11 à 0
	TCCR1A |= 1 << COM1A0; // PB1
	TCCR1B = 0b00001100;   //  // pour WGM12=1 et WGM13=0, puis CS12=1/CS11=0/CS10=0
	// pour prédiviseur réglé sur division par 256
	//"CTC" (c'est à dire, en mode "comparaison timer <-> valeur de référence")

	/*régler le PRÉDIVISEUR du timer1 pour une division par 64.
	Ainsi, la fréquence qui cadencera le timer1 sera 64 fois plus lente
	que celle sortant du prescaler, qui pour rappel était de 4 MHz.
	Ainsi, la fréquence d’avancement du timer1 sera de 4 MHz divisé par 64,
	soit 62500 Hz. Nota : à cette fréquence, le compteur mettra 16 µs
	pour avancer « d’un pas » (car 1 / 62500 Hz = 0,000016 seconde)*/

	TCNT1 = 0;	   // Mise du timer 0;
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