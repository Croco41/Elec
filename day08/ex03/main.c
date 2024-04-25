#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define RED_LED_PIN PD5
#define GREEN_LED_PIN PD6
#define BLUE_LED_PIN PD3

#define DD_MOSI PB3 // Master Output, Slave Input (généré par le maître).
#define DD_SCK PB5	// Serial Clock, Horloge (généré par le maître).
#define DD_SS PB2	// Slave Select, Actif à l'état bas (généré par le maître). SPI Bus Master Slave select

#define DDR_SPI DDRB

// #define pin_MISO PB4 // output Master Input, Slave Output (généré par l'esclave).

#define UBRR 8 // Normal mode p165

typedef struct s_RGB
{
	uint8_t g;
	uint8_t r;
	uint8_t b;
} t_RGB;

//-------------------------------------------------------------------------------

void init_rgb()
{
	DDRD |= (1 << RED_LED_PIN) | (1 << GREEN_LED_PIN) | (1 << BLUE_LED_PIN);
	// Configurer les LED en sortie (pour le portB)
	DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB4);
	// timers pour le mode Fast PWM
	// Par exemple, pour le Timer0 (associé à PD5 et PD6 pour OC0A et OC0B respectivement)
	/*Timer0 en mode Fast PWM, ce qui signifie que le compteur du timer
	s'incrémente jusqu'à atteindre la valeur maximale (0xFF ou 255 pour un timer 8 bits), puis recommence à 0.*/
	TCCR0A |= (1 << WGM01) | (1 << WGM00); // Mode Fast PWM

	/*Configure les sorties OC0A (PD5) et OC0B (PD6) pour qu'elles passent à 0
	lorsqu'une correspondance est trouvée entre TCNT0 (la valeur du timer) et
	OCR0A/OCR0B, et qu'elles repassent à 1 lorsque le timer repasse à 0*/
	TCCR0A |= (1 << COM0A1) | (1 << COM0B1); // Clear OC0A/OC0B on Compare Match, set OC0A/OC0B at BOTTOM
	// TCCR0B |= (1 << CS01);					 // Prescaler de 8, peut-être ajusté pour une fréquence différente
	TCCR0B |= (1 << CS00);

	// Timer 2 (pour PD3 - OC2B)
	TCCR2A |= (1 << WGM21) | (1 << WGM20); // Mode Fast PWM
	TCCR2A |= (1 << COM2B1);			   // Clear OC2B on Compare Match, set at BOTTOM (non-inverting mode)
	// TCCR2B |= (1 << CS21);				   // Prescaler de 8, ajuster pour la fréquence désirée
	TCCR2B |= (1 << CS20);
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
	/*ces registres contrôle la durée pendant laquelle une LED spécifique est allumée
	au cours d'un cycle de PWM, permettant ainsi de moduler l'intensité perçue
	de chaque couleur de la LED RGB*/
	/*Définit la durée pendant laquelle la LED rouge est allumée dans chaque cycle de PWM.
	Une valeur de 0 signifie que la LED est toujours éteinte,
	une valeur de 255 signifie qu'elle est toujours allumée,
	et des valeurs intermédiaires donnent différents niveaux de luminosité*/

	// Assumer que OC0A est pour rouge, OC0B pour vert et un autre Timer/Compare pour bleu
	OCR0A = g; // Définir la largeur de l'impulsion pour rouge
	OCR0B = r; // Définir la largeur de l'impulsion pour vert
	OCR2B = b; // ... Et ainsi pour bleu avec le bon Timer/OCR
}

void uart_init()
{
	// configurer le baud rate //p149
	UBRR0H = (uint8_t)(UBRR >> 8);
	UBRR0L = (uint8_t)UBRR;

	// activer le recepteur et transmetteur
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);

	// formats de donnees (8bits, pas de parite, 1 bit de stop)
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_tx(char c)
{
	// attendre que le transmit buffer soit vide
	while (!(UCSR0A & (1 << UDRE0)))
	{
	}
	// data dans le buffer, envoi du buffer
	UDR0 = c;
}

void print_hexa(uint8_t num)
{
	char hex_digits[] = "0123456789ABCDEF";
	uart_tx(hex_digits[num >> 4]);	 // Envoie le chiffre des quatre bits de poids fort
	uart_tx(hex_digits[num & 0x0F]); // Envoie le chiffre des quatre bits de poids faible
}

unsigned char *avr_itoa(int i)
{
	static char str[8];
	char length = (i <= 0);
	int j = i;
	while (j)
	{
		j /= 10;
		length++;
	}
	str[length--] = '\0';
	str[0] = i < 0 ? '-' : '0';
	i *= (i > 0) - (i < 0);
	while (i)
	{
		str[length--] = i % 10 + '0';
		i /= 10;
	}
	return str;
}

void uart_print(const char *str)
{
	while (*str)
	{
		uart_tx(*str++);
	}
}

/*+++++++++++++++++++++++++++++++++++++++++++*/

void init_ADC()
{

	/*   ADLAR (ADC Left Adjust Result) est un bit dans le registre ADMUX. Lorsque ce bit est défini (ADLAR = 1), le résultat de la conversion ADC est ajusté à gauche dans les registres ADCH et ADCL.
	Si ADLAR est réglé à 0, le résultat est ajusté à droite.*/
	// Configurer PC0 comme entrée analogique
	ADMUX |= (1 << REFS0);								  // Référence de tension sur AVCC
	ADMUX |= (1 << ADLAR);								  // Lecture sur ADCH
	ADCSRA |= (1 << ADEN);								  // Activer le convertisseur analogique-numérique
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Prédiviseur de fréquence 128 (64:ADPS2,ADPS1)
}

// Fonction pour lire la valeur du potentiomètre
uint8_t read_ADC()
{
	// Lancer la conversion
	ADCSRA |= (1 << ADSC);
	// Attendre la fin de la conversion
	while (ADCSRA & (1 << ADSC))
		;
	// Lire la valeur du convertisseur analogique-numérique
	return ADCH; // result sur 8bits
}

/*------------------------------------------*/

void SPI_masterInit(void)
{
	/* Set MOSI and SCK output, all others input */
	DDR_SPI = (1 << DD_MOSI) | (1 << DD_SCK) | (1 << DD_SS);
	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

void SPI_MasterTransmit(char cData)
{
	/*Start transmission */
	SPDR = cData;
	/* Wait for transmission complete */
	while (!(SPSR & (1 << SPIF)))
		;
}

void set_leds(t_RGB *led_array, uint16_t nb_leds, uint8_t bright)
{
	bright = bright & 0x1F; // brightness en  5 bits

	SPI_MasterTransmit(0x00);
	SPI_MasterTransmit(0x00);
	SPI_MasterTransmit(0x00);
	SPI_MasterTransmit(0x00);

	for (uint16_t i = 0; i < nb_leds; ++i)
	{
		SPI_MasterTransmit(0xE0 | bright);	// Brithness
		SPI_MasterTransmit(led_array[i].b); // Blue
		SPI_MasterTransmit(led_array[i].g); // Green
		SPI_MasterTransmit(led_array[i].r); // Red
	}

	SPI_MasterTransmit(0xFF);
	SPI_MasterTransmit(0xFF);
	SPI_MasterTransmit(0xFF);
	SPI_MasterTransmit(0xFF);
}

int main()
{
	t_RGB leds[3];

	init_rgb();
	init_ADC();
	uart_init();
	SPI_masterInit();

	// led? D6 C14 a quelle addresse sont ces 3 leds?
	while (1)
	{
		uint8_t pot_value = read_ADC();

		if (pot_value > 85)
		{
			leds[0] = (t_RGB){0x00, 0x08, 0x00};
			leds[1] = (t_RGB){0x00, 0x00, 0x00};
			leds[2] = (t_RGB){0x00, 0x00, 0x00};
		}
		if (pot_value > 170)
		{
			leds[0] = (t_RGB){0x00, 0x00, 0x00};
			leds[1] = (t_RGB){0x00, 0x08, 0x00};
			leds[2] = (t_RGB){0x00, 0x00, 0x00};
		}
		if (pot_value == 255)
		{

			leds[0] = (t_RGB){0x00, 0x00, 0x00};
			leds[1] = (t_RGB){0x00, 0x00, 0x00};
			leds[2] = (t_RGB){0x00, 0x08, 0x00};
		}

		set_leds(leds, 3, 5);
		_delay_ms(20);
	}
	return 0;
}
