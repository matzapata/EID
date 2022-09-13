
/********************************************************************************************
 * INCLUDES
 ********************************************************************************************/
#include "Flash.h"
#include "Storage.h"


/********************************************************************************************
 * DEFINES
 ********************************************************************************************/

/********************************************************************************************
 * VARIABLES GLOBALES LOCALES AL MODULO
 ********************************************************************************************/
static Storage_t * storage_p;
static Storage_t storage;
static uint8_t PswdQ = 0;
static uint8_t ContQ = 0;
static uint16_t PoppedPswd = 0; // bit registry
static uint16_t PoppedCont = 0; // bit registry
static uint8_t i2hex[] = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'}; // Simple conversion a hexa

/********************************************************************************************
 * PROPOTIPOS FUNCIONES PRIVADAS AL MODULO
 *******************************************************************************************/

/********************************************************************************************
 * IMPLEMENTACION DE FUNCIONES PUBLICAS
 ********************************************************************************************/

/** Obtenemos los indicies de las paginas en las que tenermos guardadas claves
 */
void Storage_Init(void) {
	// Leemos storage
	storage_p = (Storage_t*)Flash_GetPageAddress(STORAGE_SECTOR, STORAGE_PAGE);

	PswdQ=0;
	ContQ=0;

	// Calculamos cantidad de data valida
	for (uint8_t i = 0; i<16; i++ ) {
		if (storage_p->Passwords[i] == FILLED_PAGE) {PswdQ++;}
		if (storage_p->Contacts[i] == FILLED_PAGE) {ContQ++;}
	}

}
/**
 *
 */
void Storage_PrintStore(void) {
	// Imprimimos la data que leemos
	printf("Passwords: ");
	for (uint8_t i = 0; i<16; i++ ) printf("%d ", storage_p->Passwords[i]);
	printf("\nContacts: ");
	for (uint8_t i = 0; i<16; i++ ) printf("%d ", storage_p->Contacts[i]);
	printf("\n");
}
/** Devuelve la direccion de memoria del Hash de autenticacion. No una copia de memoria
 * asi que no es editable.
 */
uint8_t * Storage_GetHashAddr(void) {
	return (uint8_t *)Flash_GetPageAddress(HASH_SECTOR, HASH_PAGE);
}
/** Guarda el hash de autenticacion en la flash y devuelve la cantidad de bytes del hash
 */
uint8_t Storage_SetHash(uint8_t * Hash) {
	return Flash_WritePage(HASH_SECTOR, HASH_PAGE, (uint32_t*)Hash, 32);
}
/**
 *
 */
uint8_t Storage_GetPswdQ(void) {
	return PswdQ;
}
/**
 *
 */
uint8_t * Storage_PopPswd(void) {
	if (PswdQ == 0) return NULL;

	uint8_t * retval = NULL;

	for(int i = 0; i < 16; i++) {
		uint8_t bit = (PoppedPswd & (1 << i)) != 0;
		if (storage_p->Passwords[i] != EMPTY_PAGE && bit == 0) {
			PoppedPswd |= (1<<i);
			retval = (uint8_t*)Flash_GetPageAddress(PASSWORDS_SECTOR, i);
			break;
		}
	}

	return retval;
}
/**
 *
 */
uint8_t Storage_SetPswd(uint8_t * password, uint8_t size) {
	if (PswdQ == 16) return 0; // Exedemos capacidad maxima
	if (size > PSWD_STR_SIZE) return 0; // Exedemos tamaño pagina

	uint8_t CleanPassword[PSWD_STR_SIZE];
	memset(CleanPassword, '\0', PSWD_STR_SIZE);

	printf("SetPswd: size = %d\n", size);

	// Encontramos un lugar disponible
	for (uint8_t i = 0; i < 16; i++) {
		if (storage_p->Passwords[i] == EMPTY_PAGE) {
			printf("Posicion disponible: %d\n", i);
			// Hacemos una copia a memoria para trabajo local
			memcpy(&storage, storage_p, sizeof(Storage_t));
			storage.Passwords[i] = FILLED_PAGE;

			// Guardamos el indice en la string y copiamos la data que nos pasan
			sprintf(CleanPassword, "%c#%s", i2hex[i], password);

			Flash_WritePage(PASSWORDS_SECTOR, i, (uint32_t*)CleanPassword, PSWD_STR_SIZE);
			printf("Nueva contraseña almacenada: %s\n", Flash_GetPageAddress(PASSWORDS_SECTOR, i));

			Flash_WritePage(STORAGE_SECTOR, STORAGE_PAGE, (uint32_t*)&storage, sizeof(Storage_t));
			printf("Passwords[%d]: %d\n", i, storage_p->Passwords[i]);

			PswdQ++;
			break;
		}
	}
	return 1;
}
/**
 *
 */
uint8_t Storage_DeletePswd(uint8_t index) {
	if (index > 15) return 0;

	//Simplemente lo eliminamos de nuestro indice
	memcpy(&storage, storage_p, sizeof(Storage_t));
	storage.Passwords[index] = EMPTY_PAGE;
	PoppedPswd &= ~(1 << index);

	Flash_WritePage(STORAGE_SECTOR, STORAGE_PAGE, (uint32_t*)&storage, sizeof(Storage_t));
	printf("Passwords[%d]: %d\n", index, storage_p->Passwords[index]);

	PswdQ--;
}
/**
 *
 */
uint8_t Storage_GetContQ(void) {
	return ContQ;
}
/**
 *
 */
uint8_t * Storage_PopCont(void) {
	if (ContQ == 0) return NULL;

	uint8_t * retval = NULL;

	for(int i = 0; i < 16; i++) {
		uint8_t bit = (PoppedCont & (1 << i)) != 0;
		if (storage_p->Contacts[i] != EMPTY_PAGE && bit == 0) {
			PoppedCont |= (1<<i);
			retval = (uint8_t*)Flash_GetPageAddress(CONTACTS_SECTOR, i);
			break;
		}
	}

	return retval;
}
/**
 *
 */
uint8_t Storage_SetCont(uint8_t * contact, uint8_t size) {
	if (ContQ == 16) return 0; // Exedemos capacidad maxima
	if (size > CONT_STR_SIZE) return 0; // Exedemos tamaño pagina

	uint8_t CleanContact[CONT_STR_SIZE];
	memset(CleanContact, '\0', CONT_STR_SIZE);

	// Encontramos un lugar disponible
	for (uint8_t i = 0; i < 16; i++) {
		if (storage_p->Contacts[i] == EMPTY_PAGE) {
			printf("Posicion disponible: %d\n", i);
			// Hacemos una copia a memoria para trabajo local
			memcpy(&storage, storage_p, sizeof(Storage_t));
			storage.Contacts[i] = FILLED_PAGE;

			// Guardamos el indice en la string y copiamos la data que nos pasan
			sprintf(CleanContact, "%c#%s", i2hex[i], contact);

			Flash_WritePage(CONTACTS_SECTOR, i, (uint32_t*)CleanContact, CONT_STR_SIZE);
			printf("Nuevo contacto almacenada: %s\n", Flash_GetPageAddress(CONTACTS_SECTOR, i));

			Flash_WritePage(STORAGE_SECTOR, STORAGE_PAGE, (uint32_t*)&storage, sizeof(Storage_t));
			printf("Contacts[%d]: %d\n", i, storage_p->Contacts[i]);

			ContQ++;
			break;
		}
	}
	return 1;
}
/**
 *
 */
uint8_t Storage_DeleteCont(uint8_t index) {
	if (index > 15) return 0;

	//Simplemente lo eliminamos de nuestro indice
	memcpy(&storage, storage_p, sizeof(Storage_t));
	storage.Contacts[index] = EMPTY_PAGE;
	PoppedCont &= ~(1 << index);

	Flash_WritePage(STORAGE_SECTOR, STORAGE_PAGE, (uint32_t*)&storage, sizeof(Storage_t));
	printf("Contacts[%d]: %d\n", index, storage_p->Contacts[index]);

	ContQ--;
}


void Storage_Reset(void) {
	PoppedCont = 0;
	PoppedPswd = 0;
}
/********************************************************************************************
 * IMPLEMENTACION DE FUNCIONES PRIVADAS AL MODULO
 ********************************************************************************************/
