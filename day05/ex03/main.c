#include <avr/io.h>
#include <avr/eeprom.h>

#define EEPROM_SIZE 1024 // Suppose 1KB of EEPROM

typedef struct
{
	uint8_t magic_number;
	uint16_t id;	 // Identifiant unique pour les données
	uint16_t length; // Longueur des données
	uint8_t is_used; // Indicateur si le bloc est utilisé (1) ou libre (0)
} EEPROMBlock;

/*    eepromalloc_write
		Recherchez un bloc où les données peuvent être écrites :
			Si un bloc existant avec le même ID est trouvé et que l'espace est suffisant, réécrivez les données.
			Sinon, trouvez un espace libre pour écrire de nouvelles données.
		Si suffisamment d'espace est disponible, écrivez le Magic Number, l'ID, la longueur, et enfin les données.
		Mettez à jour un bit ou un indicateur pour montrer que cet espace est utilisé.

	eepromalloc_read
		Parcourez les blocs de données en commençant par le début de l'EEPROM.
		Lisez les ID et comparez-les avec l'ID fourni.
		Si l'ID correspond, lisez les données et les copiez dans le buffer fourni.

	eepromalloc_free
		Trouvez le bloc de données avec l'ID spécifié.
		Marquez le bloc comme libre pour réutilisation future sans réellement effacer les données (pour économiser les cycles d'écriture de l'EEPROM).*/