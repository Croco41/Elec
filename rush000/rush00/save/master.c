#include "i2c.h"
#include "uart.h"
#include "timer.h"
#include "game.h"
#include <util/delay.h>

uint8_t master_win = 2;

void master_button_pressed(void)
{
    if (PIND & (1 << PD2) == 0)
    {
        master_win = 0;
    }
}

void ft_master(void)
{
    uart_printstr("Master\r\n");
    setup_addr(MASTER_ADDR);
    i2c_start();
    i2c_write(BROADCAST_MSG);
    i2c_stop();
    _delay_ms(1000);
    while (1)
    {
        i2c_start();
        i2c_write(START_GAME);
        i2c_stop();
        led_count(&master_button_pressed);
        while (master_win == 2)
        {
            i2c_stop();
            print_hex_value(TWDR);
            uart_printstr("\r\n");
            if (TWDR == SLAVE_PRESS_SW1)
            {
                uart_printstr("Slave win\r\n");
                master_win = 0;
                break;
            }
            if (PIND & (1 << PD2) == 0)
            {
                uart_printstr("Master win\r\n");
                master_win = 1;
                break;
            }
            i2c_start();
            i2c_write(SLAVE_ADDR << 1 | 1);
            TWCR = (1 << TWINT) | (1 << TWEN);
            if ((TWCR & (1 << TWINT)))
                continue;
            _delay_ms(1000);
        }
        if (master_win == 0)
        {
            i2c_start();
            i2c_write(YOU_WIN);
            i2c_stop();
        }
        else
        {
            i2c_start();
            i2c_write(SLAVE_ADDR << 1);
            i2c_write(YOU_LOSE);
            i2c_stop();
            PORTB |= (1 << PB4) | (1 << PB2) | (1 << PB1) | (1 << PB0);
            _delay_ms(1000);
        }
        master_win = 2;
        i2c_start();
        i2c_write(SLAVE_ADDR << 1);
        i2c_write(END_GAME);
        i2c_stop();
        _delay_ms(1000);
    }
}