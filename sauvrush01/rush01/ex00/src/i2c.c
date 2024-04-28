#include "i2c.h"
#include "uart.h"
#include <stdio.h>

// Pour set la fréquence de l'I2C le SCL avec un prescaler de 1
void i2c_init(void)
{
    TWSR = 0;                          // Set prescaler to 1 page 240
    TWBR = ((F_CPU / F_SCL) - 16) / 2; // (=72) // Set bit rate register page 239
    // TWCR = (1 << TWEN); //i2c material activation
}

void i2c_start(void)
{
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // Envoyer une condition de start
    while (!(TWCR & (1 << TWINT)))
        ; // Attendre la fin de la transmission de la condition de start
}

void i2c_stop(void)
{
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN); // Transmettre stop condition
                                                      // No waiting needed here as stop is an asynchronous operation
}

// Pour envoyer des données sur le bus I2C et attendre la fin de la transmission
void i2c_write(unsigned char data)
{
    TWDR = data;                       // Chargement de la donnée dans TWDR
    TWCR = (1 << TWINT) | (1 << TWEN); // Début de la transmission
    while (!(TWCR & (1 << TWINT)))
        ; // Attente de la fin de la transmission
}

void i2c_request_data(uint8_t ack)
{
    TWCR = (1 << TWINT) | (1 << TWEN) | (ack << TWEA); // Configure pour recevoir avec ACK ou NACK
}

// Pour lire les données du bus I2C et envoyer un accusé de réception
uint8_t i2c_read(uint8_t ack)
{
    if (ack)
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); // Recevoir le byte avec ACK
    else
        TWCR = (1 << TWINT) | (1 << TWEN); // Recevoir le byte sans ACK
    while (!(TWCR & (1 << TWINT)))
        ; // Attendre que la donnée soit reçue

    return (TWDR);
}

void setup_addr(uint8_t addr)
{
    TWAR = (addr << 1) | 1; // Set slave address
    // Mettre ce bit à 1 permet à l'esclave de répondre à l'adresse générale de broadcast 0x00.
    TWCR = (1 << TWEA) | (1 << TWEN); // Enable TWI and ack
}

void setup_addr2(uint8_t addr)
{
    TWAR = (addr << 1) | 0; // Set slave address
    // Mettre ce bit à 1 permet à l'esclave de répondre à l'adresse générale de broadcast 0x00.
    TWCR = (1 << TWEA) | (1 << TWEN); // Enable TWI and ack
}
void setup_addr_master(uint8_t addr)
{
    TWAR = (addr << 1) | 0; // Set master address
    // Mettre ce bit à 1 permet à l'esclave de répondre à l'adresse générale de broadcast 0x00.
    TWCR = (1 << TWINT) | (1 << TWEN); // Enable TWI and ack
}