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
            i2c_start();
            i2c_write(ARDUINO_OB, 0, 0);
            i2c_write(0x41, 0, 0);
            i2c_stop();
            _delay_ms(1000);
        }
        else
        {
            i2c_start();
            i2c_write(((ARDUINO_CG << 1) | 1), 0, 0);
            i2c_read(1, 0);
            i2c_read(1, 0);
            print_hex_value(TWDR);
            uart_printstr("\r\n");
            i2c_stop();
            _delay_ms(1000);
        }
    }
}