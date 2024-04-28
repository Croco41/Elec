#include "timer.h"

void timer_init()
{
    // Configurer le timer pour déclencher une interruption toutes les 3 ms
    TCCR0A = (1 << WGM01); // CTC mode
    TCCR0B = (1 << CS01) | (1 << CS00); // Prescaler de 64
    OCR0A = ((F_CPU / 64) / 1000) - 1;
    TIMSK0 = (1 << OCIE0A); // Activer l'interruption de comparaison A
}