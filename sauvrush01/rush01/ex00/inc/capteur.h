#ifndef CAPTEUR_H
#define CAPTEUR_H

#include <avr/io.h>
#include <util/twi.h>

# define AHT20_ADDR 0x38 // Found in datasheet of the sensor
# define AHT20_WRITE (AHT20_ADDR << 1) // 0x70
# define AHT20_READ ((AHT20_ADDR << 1) | 0x01) // 0x71
# define AHT20_CMD_TEMPERATURE 0xAC // Found in datasheet of the sensor

void aht20_config();
void aht20_read(uint8_t data[5]);
void aht20_convert(uint8_t data[5], double *temp, double *hum);
double aht20_convert_temp_celcius(uint8_t data[5]);
double aht20_convert_temp_fahrenheit(uint8_t data[5]);
double aht20_convert_humidity(uint8_t data[5]);

#endif