#ifndef STORAGE_H_
#define STORAGE_H_

/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/
#include "Rsa.h"

/***********************************************************************************************************************************
 *** CONSTANTES
 **********************************************************************************************************************************/
// Sectores y paginas de memoria
#define HASH_SECTOR 40
#define HASH_PAGE 0

#define STORAGE_SECTOR 40
#define STORAGE_PAGE 1

#define PASSWORDS_SECTOR 41
#define CONTACTS_SECTOR 42

#define EMPTY_PAGE -1
#define FILLED_PAGE 1

#define PSWD_STR_SIZE 32
#define CONT_STR_SIZE 32

/***********************************************************************************************************************************
 *** TYPEDEFS
 **********************************************************************************************************************************/
// Contiene numeros de paginas en las que podemos encontrar contraseñas y contactos
typedef struct {
	int8_t Passwords[16];
	int8_t Contacts[16];
} Storage_t;


/***********************************************************************************************************************************
 *** PROTOTIPOS FUNCIONES PUBLICAS
 **********************************************************************************************************************************/

/*
 * Lee la estructura Storage de memoria y realiza otras inicializaciones
 */
void Storage_Init(void);
/*
 * Imprime los registros internos de contraseñas y contactos
 */
void Storage_PrintStore(void);
/*
 * Devuelve la direccion de la memoria en la que esta almacenado el hash
 * de autenticacion
 */
uint8_t * Storage_GetHashAddr(void);
/*
 * Guarda un nuevo hash de autenticacion en memoria
 */
uint8_t Storage_SetHash(uint8_t * Hash);
/*
 * Devuelve la cantidad de contraseñas almacenadas
 */
uint8_t Storage_GetPswdQ(void);
/*
 * Devuelve puntero a la proxima direccion de memoria flash con
 * contraseña almacenada
 */
uint8_t * Storage_PopPswd(void);
/*
 * Guarda una contraseña en memoria flash.
 * En error devuele 0, si no 1.
 */
uint8_t Storage_SetPswd(uint8_t * password, uint8_t size);
/*
 * Elimina contraseña de nuestro indice interno
 */
uint8_t Storage_DeletePswd(uint8_t index);


/*
 * Devuelve la cantidad de contactos almacenados
 */
uint8_t Storage_GetContQ(void);
/*
 * Devuelve puntero a la proxima direccion de memoria flash con
 * contacto almacenado
 */
uint8_t * Storage_PopCont(void);
/*
 * Guarda un contacto en memoria flash.
 * En error devuele 0, si no 1.
 */
uint8_t Storage_SetCont(uint8_t * contact, uint8_t size);
/*
 * Elimina contacto de nuestro indice interno
 */
uint8_t Storage_DeleteCont(uint8_t index);

/**
 * Resetea el contador popped
 */
void Storage_Reset(void);

#endif /* STORAGE_H_ */
