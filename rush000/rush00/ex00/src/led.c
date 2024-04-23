#include "timer.h"
#include "uart.h"
#include "i2c.h"
#include <util/delay.h>

// void led_count(void (*f)(void))
// {
//     PORTB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4);
//     wait_one_second(f);
//     PORTB &= ~(1 << PB0);
//     wait_one_second(f);
//     PORTB &= ~(1 << PB1);
//     wait_one_second(f);
//     PORTB &= ~(1 << PB2);
//     wait_one_second(f);
//     PORTB &= ~(1 << PB4);
// }

void led_count()
{
    PORTB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4);
    _delay_ms(1000);
    PORTB &= ~(1 << PB0);
    _delay_ms(1000);
    PORTB &= ~(1 << PB1);
    _delay_ms(1000);
    PORTB &= ~(1 << PB2);
    _delay_ms(1000);
    PORTB &= ~(1 << PB4);
}

void led_win()
{
    PORTD |= (1 << PD6);  // Allume la LED sur PD5
    _delay_ms(10000);     // Attend 5 secondes
    PORTD &= ~(1 << PD6); // Éteint la LED sur PD5
}

void led_lose()
{
    PORTD |= (1 << PD5);  // Allume la LED sur PD6
    _delay_ms(10000);     // Attend 5 secondes
    PORTD &= ~(1 << PD5); // Éteint la LED sur PD6
}