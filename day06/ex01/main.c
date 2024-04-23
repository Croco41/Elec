
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define RED_LED_PIN PD5
#define GREEN_LED_PIN PD6
#define BLUE_LED_PIN PD3

void init_rgb_led()
{
	DDRD |= (1 << RED_LED_PIN) | (1 << GREEN_LED_PIN) | (1 << BLUE_LED_PIN);
}

void set_color(uint8_t red, uint8_t green, uint8_t blue)
{
	if (red)
		PORTD |= (1 << RED_LED_PIN);
	else
		PORTD &= ~(1 << RED_LED_PIN);

	if (green)
		PORTD |= (1 << GREEN_LED_PIN);
	else
		PORTD &= ~(1 << GREEN_LED_PIN);

	if (blue)
		PORTD |= (1 << BLUE_LED_PIN);
	else
		PORTD &= ~(1 << BLUE_LED_PIN);
}

int main(void)
{
	init_rgb_led(); // Initialize the LED pins

	while (1)
	{
		set_color(1, 0, 0); // Red
		_delay_ms(1000);

		set_color(0, 1, 0); // Green
		_delay_ms(1000);

		set_color(0, 0, 1); // Blue
		_delay_ms(1000);

		set_color(1, 1, 0); // Yellow
		_delay_ms(1000);

		set_color(0, 1, 1); // Cyan
		_delay_ms(1000);

		set_color(1, 0, 1); // Magenta
		_delay_ms(1000);

		set_color(1, 1, 1); // White
		_delay_ms(1000);
		set_color(0, 0, 0);
	}
}

// void init_setup()
// {
// 	DDRD |= (1 << PD5) | (1 << PD6) | (1 << PD3);
// 	// PCINT19
// 	// PCINT21
// 	// PCINT22
// }

// int main(void)
// {
// 	init_setup();
// 	while (1)
// 	{
// 		// rouge
// 		PORTD |= (1 << PD5);
// 		_delay_ms(1000);
// 		PORTD &= ~(1 << PD5);
// 		// vert
// 		PORTD |= (1 << PD6);
// 		_delay_ms(1000);
// 		PORTD &= ~(1 << PD6);
// 		// bleue
// 		PORTD |= (1 << PD3);
// 		_delay_ms(1000);
// 		PORTD &= ~(1 << PD3);

// 		// yellow =rouge et vert
// 		PORTD |= (1 << PD5) | (1 << PD6);
// 		_delay_ms(1000);
// 		PORTD &= ~(1 << PD5) | (1 << PD6);

// 		// cyan =vert et bleue
// 		PORTD |= (1 << PD3) | (1 << PD6);
// 		_delay_ms(1000);
// 		PORTD &= ~(1 << PD3) | (1 << PD6);

// 		// magenta =rouge et bleue
// 		PORTD |= (1 << PD3) | (1 << PD5);
// 		_delay_ms(1000);
// 		PORTD &= ~(1 << PD3) | (1 << PD5);

// 		// blanc
// 		PORTD |= (1 << PD3) | (1 << PD5) | (1 << PD6);
// 		_delay_ms(1000);
// 		PORTD &= ~(1 << PD3) | (1 << PD5) | (1 << PD6);
// 	}
// }
