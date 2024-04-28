#include "capteur.h"
#include "expander.h"
#include "i2c.h"
#include "uart.h"
#include <util/delay.h>

void aht20_config()
{
    i2c_start(); // Démarrer la communication I2C
    i2c_write(AHT20_WRITE); // Écrire l'adresse du capteur
    i2c_write(AHT20_CMD_TEMPERATURE);
    i2c_write(0x33);
    i2c_write(0x00);
    i2c_stop(); // Arrêter la communication I2C
}

void aht20_read(uint8_t data[5])
{
    i2c_start(); // Démarrer la communication I2C
    i2c_write(AHT20_WRITE); // Écrire l'adresse du capteur
    i2c_write(AHT20_CMD_TEMPERATURE);
    i2c_start(); // Démarrer la communication I2C
    i2c_write(AHT20_READ); // Écrire l'adresse du capteur
    i2c_read(1); // Lire les données du capteur
    for (uint8_t i = 0; i < 5; i++)
    {
        data[i] = i2c_read(i == 4 ? 0 : 1); // Lire les données du capteur
    }
    i2c_stop(); // Arrêter la communication I2C
}

void aht20_convert(uint8_t data[5], double *tempe, double *humi)
{
    static double   temp_history[3] = {0.0, 0.0, 0.0};
    static double   hum_history[3] = {0.0, 0.0, 0.0};
    static int      index = 0;
    static int      count = 0;
    const uint32_t  size_bit = 1048576; // 2^20
    double          temp, hum, temp_avg, hum_avg;
    char            str[6];
    int             i;

    hum = ((uint32_t)data[0] << 12) | ((uint32_t)data[1] << 4) | ((uint32_t)data[2] >> 4);
    temp = (((uint32_t)data[2] & 0x0F) << 16) | ((uint32_t)data[3] << 8) | data[4];
    temp = (temp / size_bit) * 200.0 - 50.0;
    hum = (hum / size_bit) * 100.0;

    temp_history[index] = temp;
    hum_history[index] = hum;
    index = (index + 1) % 3;
    count = count < 3 ? count + 1 : 3;

    temp_avg = hum_avg = 0.0;
    for (i = 0; i < count; i++)
    {
        temp_avg += temp_history[i];
        hum_avg += hum_history[i];
    }
    temp_avg /= count;
    hum_avg /= count;
    *tempe = temp_avg;
    *humi = hum_avg;
}