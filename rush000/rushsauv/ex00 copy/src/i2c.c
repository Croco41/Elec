#include "i2c.h"
#include "uart.h"
#include <stdio.h>

// Pour set la fréquence de l'I2C le SCL avec un prescaler de 1
void i2c_init(void)
{
    TWSR = 0;                          // Set prescaler to 1 page 240
    TWBR = ((F_CPU / F_SCL) - 16) / 2; // (=72) // Set bit rate register page 239
}

// Pour envoyer un start condition et attendre que le bus soit libre
// doc page 225
uint8_t i2c_start(void)
{
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // Envoie du signal de démarrage
    while (!(TWCR & (1 << TWINT)))
        uart_printstr("Start\r\n"); // Attente de l'achèvement de l'opération
    if ((TWSR & 0xF8) != TW_START && (TWSR & 0xF8) != TW_REP_START)
        return 1;
    return 0;
}

uint8_t i2c_start_to_read(uint8_t address)
{
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // Envoie du signal de démarrage
    while (!(TWCR & (1 << TWINT)))
        uart_printstr("Start\r\n"); // Attente de l'achèvement de l'opération
    if ((TWSR & 0xF8) != TW_START && (TWSR & 0xF8) != TW_REP_START)
        return 1;

    TWDR = ((address << 1) | 1);
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)))
        uart_printstr("Start to read\r\n"); // Attente de l'achèvement de l'opération
    // if ((TWSR & 0xF8) != TW_MT_SLA_ACK && (TWSR & 0xF8) != TW_MR_SLA_ACK)
    //     return 1;
    if ((TWSR & 0xF8) != TW_MR_SLA_ACK)
    {
        uart_printstr("Slave did not acknowledge read request.\r\n");
        return 1;
    }
    return 0;
}

uint8_t i2c_start_to_write(uint8_t address)
{
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // Envoie du signal de démarrage
    while (!(TWCR & (1 << TWINT)))
        uart_printstr("Start\r\n"); // Attente de l'achèvement de l'opération
    if ((TWSR & 0xF8) != TW_START && (TWSR & 0xF8) != TW_REP_START)
        return 1;

    TWDR = (address << 1);
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)))
        uart_printstr("Start to write\r\n"); // Attente de l'achèvement de l'opération
    // if ((TWSR & 0xF8) != TW_MT_SLA_ACK && (TWSR & 0xF8) != TW_MR_SLA_ACK)
    //     return 1;
    if ((TWSR & 0xF8) != TW_MT_SLA_ACK)
    {
        uart_printstr("Slave did not acknowledge write request.\r\n");
        return 1;
    }
    return 0;
}

// Pour envoyer un stop condition et libérer le bus
// doc page 225
void i2c_stop(void)
{
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

// Pour envoyer des données sur le bus I2C et attendre la fin de la transmission
void i2c_write(uint8_t data, uint8_t ack)
{
    TWDR = data; // Charger la donnée dans le registre de données TWDR
    // TWCR = (1 << TWINT) | (1 << TWEN) | (ack << TWEA); // Déclencher la transmission
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)))
        uart_printstr("Write\r\n"); // Attente de l'achèvement de l'opération
}

// Pour lire les données du bus I2C et envoyer un accusé de réception
// et print les valeurs en hexa
uint8_t i2c_read(uint8_t ack)
{
    TWCR = (1 << TWINT) | (1 << TWEN) | (ack << TWEA); // Déclencher la lecture
    while (!(TWCR & (1 << TWINT)));
    // uart_printstr("Read\r\n"); // Attente de l'achèvement de l'opération
    return TWDR;
}

// FOUND ALL DEVICE I2C ADDRESS

// uint8_t i2c_start(uint8_t address)
// {
//     TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // Send start condition
//     while (!(TWCR & (1 << TWINT))); // Wait for TWINT flag set

//     // Check value of TWI status register. Mask prescaler bits.
//     uint8_t twst = TW_STATUS & 0xF8;
//     if ((twst != TW_START) && (twst != TW_REP_START)) return 1;

//     // Send device address
//     TWDR = address;
//     TWCR = (1 << TWINT) | (1 << TWEN);

//     // Wait for transmission to complete
//     while (!(TWCR & (1 << TWINT)));

//     // Check value of TWI status register. Mask prescaler bits.
//     twst = TW_STATUS & 0xF8;
//     if ((twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK)) return 1;

//     return 0;
// }

// void i2c_scan()
// {
//     char buffer[50];
//     for(uint8_t addr = 0; addr <= 0x7F; addr++)
//     {
//         if(i2c_start(addr << 1) == 0)
//         {
//             sprintf(buffer, "Device found at address 0x%02X\r\n", addr);
//             uart_printstr(buffer);
//             i2c_stop();
//         }
//     }
// }