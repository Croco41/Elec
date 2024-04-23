#include "uart.h"
#include "i2c.h"
#include <util/delay.h>

volatile uint8_t master = 0;

void who_am_i(void)
{
    DDRC |= (1 << PC0);
    PORTC &= ~(1 << PC0);

    if (PINC & (1 << PC0))
    {
        master = 1;
        uart_printstr("I am master\r\n");
    }
    else
    {
        master = 0;
        uart_printstr("I am slave\r\n");
    }
}

int main(void)
{
    uart_init();
    i2c_init();
    uart_printstr("UART and I2C initialized\r\n");
    who_am_i();

    _delay_ms(1000);
    while (1)
    {
        if (master)
        {
            i2c_start_to_write(ARDUINO_OB);
            i2c_write(0x4F, 1);
            // i2c_stop();
            _delay_ms(100);

            // Partie lecture
            // Re-démarrage ou 'repeated start' pour la lecture
            i2c_start_to_read((ARDUINO_OB << 1)); // Envoyer l'adresse avec le bit de lecture
            print_hex_value(i2c_read(0));         // Lire les données, envoyer ACK si d'autres octets sont attendus, sinon NACK
            i2c_stop();                           // Arrêter la communication après la lecture
        }
        else
        {
            _delay_ms(1000);
            i2c_start_to_read(ARDUINO_CG);
            // i2c_read(0);
            // print_hex_value(TWDR);
            print_hex_value(i2c_read(0));
            uart_printstr("\r\n");
            i2c_stop();
            _delay_ms(1000);
        }
    }
}