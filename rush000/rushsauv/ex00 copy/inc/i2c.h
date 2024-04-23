#ifndef I2C_H
#define I2C_H

#include <avr/io.h>
#include <util/twi.h>

#define F_SCL 100000UL // 100kHz I2C clock speed

#define ARDUINO_CG 0x20
#define ARDUINO_OB 0x27

void i2c_init(void);
uint8_t i2c_start_to_read(uint8_t address);
uint8_t i2c_start_to_write(uint8_t address);
void i2c_stop(void);

void i2c_write(uint8_t data, uint8_t ack);
uint8_t i2c_read(uint8_t ack);

// FOUND ALL DEVICE I2C ADDRESS
// uint8_t i2c_start(uint8_t address);
// void    i2c_scan();

#endif

/* I2C Communication
L'I2C (Inter-Integrated Circuit) est un protocole de communication qui permet à plusieurs dispositifs de communiquer entre eux via un bus de données bidirectionnel. Il est souvent utilisé pour la communication entre les microcontrôleurs et divers périphériques tels que les capteurs, les écrans LCD, etc.

Registres I2C :

TWBR (Two Wire Bit Rate Register) : Ce registre est utilisé pour définir la fréquence de l'I2C. Pour une fréquence de 100 kHz, vous pouvez calculer la valeur à mettre dans ce registre en utilisant la formule suivante : TWBR = (F_CPU / F_SCL - 16) / 2, où F_CPU est la fréquence du microcontrôleur et F_SCL est la fréquence de l'I2C.

TWSR (Two Wire Status Register) : Ce registre contient le statut de l'I2C après chaque opération. Vous pouvez le lire pour vérifier si l'opération a réussi ou non.

TWCR (Two Wire Control Register) : Ce registre est utilisé pour contrôler l'I2C. Vous pouvez l'utiliser pour démarrer ou arrêter une transmission, envoyer un accusé de réception, etc.

TWDR (Two Wire Data Register) : Ce registre contient les données à envoyer ou reçues.
*/