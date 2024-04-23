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
    setup_addr_master(MASTER_ADDR);
    // setup_addr(MASTER_ADDR);
    i2c_start();
    i2c_write(BROADCAST_MSG);
    // i2c_write((SLAVE_ADDR << 1));
    i2c_write(START_GAME);
    i2c_stop();
    // led_count(&master_button_pressed); // ici pour lancer le compte a rebours du jeu
    led_count(); // compte a rebours

    while (1)
    {

        _delay_ms(50);
        // TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
        // _delay_us(10);
        uart_printstr("Je suis dans la boucle de jeu\r\n");
        // Vérification si le maître a gagné
        if (!(PIND & (1 << PD2)))
        { // Supposons PD2 est connecté au bouton du maître
            uart_printstr("Master win\r\n");
            master_win = 1;
            break;
        }

        // Envoi d'une requête ou vérification d'une réponse I2C
        // i2c_stop();
        i2c_start();
        uart_printstr("coince dans le start?\r\n");
        i2c_write((SLAVE_ADDR << 1) | 0);
        uart_printstr("write1?");
        i2c_write(DO_YOU_HAVE_PRESS_SW1); // Demande de lecture à l'adresse de l'esclave
        uart_printstr("write2?");
        i2c_stop();
        _delay_ms(10);

        uart_printstr("avt 2eme start?");
        i2c_start();
        uart_printstr("ICI?");
        i2c_write((SLAVE_ADDR << 1) | 1); // Adresse esclave avec bit de lecture pour lire la réponse
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
        while (!(TWCR & (1 << TWINT)))
            ;
        // if ((TWCR & (1 << TWINT)))
        //     continue;
        uart_printstr("LA?\r\n");

        // uint8_t response2 = i2c_read_ack();
        uint8_t response2 = TWDR;
        // TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWSTO);
        i2c_stop();
        uart_printstr("Response: 0x");
        print_hex_value(response2); // Assurez-vous que cette fonction imprime un uint8_t en hexadécimal
        uart_printstr("\r\n");

        if (TWDR == SLAVE_PRESS_SW1)
        {
            uart_printstr("Slave win\r\n");
            master_win = 0;
            break;
        }

        _delay_ms(100); // Délai pour réduire la charge du CPU
    }

    i2c_start();
    uart_printstr("j'arrive dans le dernier start?\r\n");
    i2c_write(BROADCAST_MSG);
    i2c_write(master_win == 1 ? YOU_LOSE : YOU_WIN);
    if (master_win == 1)
    {
        uart_printstr("TU as gagne!!!");
        led_win();
    }
    if (master_win == 0)
    {
        uart_printstr("TU as perdu!!!");
        led_lose();
    }
    i2c_stop();
    _delay_ms(1000);

    master_win = 2;
    i2c_start();
    i2c_write(SLAVE_ADDR << 1);
    i2c_write(END_GAME);
    i2c_stop();
    _delay_ms(1000);
}
