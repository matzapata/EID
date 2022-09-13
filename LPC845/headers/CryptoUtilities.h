#ifndef CRYPTOUTILITIES_H_
#define CRYPTOUTILITIES_H_

/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/
#include <stdbool.h>
#include <stdint.h>

#include "Aes.h"
#include "Rsa.h"

/***********************************************************************************************************************************
 *** CONSTANTES
 **********************************************************************************************************************************/
#define AES_KEY_LENGHT 16

/***********************************************************************************************************************************
 *** PROTOTIPO DE FUNCIONES PUBLICAS
 **********************************************************************************************************************************/

/**
 * Guarda la clave de la session dinamicamente
 */
void Crypto_SetSessionKey(uint8_t * key);

/**
 * Genera una clave random de N bytes y la almacena en password que
 * previamente debe haber reservado el espacio de memoria necesario.
 */
void Crypto_KeyGen(uint8_t * password, uint32_t seed, uint8_t N);

/**
 *  Devuelve una instancia de la clave publica hardcodeada en memoria
 */
public_key_class_t * Crypto_GetPublicKey(void);

/**
 * Obtiene el hash del usuario de la memoria flash y lo compara
 * con el que nos pasaron
 */
bool Crypto_CmpUsrKeyHash(uint8_t * keyHash);

/**
 * Guarda el hash de la llave de login del usuario en la memoria flash
 */
void Crypto_SetUsrKeyHash(uint8_t * keyHash);

/**
 * Usando la clave seteada con setSessionKey desencripta
 * el buffer y lo pone en output.
 * output tiene que tener la memoria reservada.
 * el tamaño de output es multiplo de 16, si sobraba espacio
 * se lleno con 0.
 * En caso de que no halla una clave guardada u otro error devuelve false
 */
uint32_t Crypto_AesDecryptBuffer(uint8_t *buffer, uint8_t **output, uint32_t size);

/**
 * Usando la clave seteada con setSessionKey encripta
 * el buffer y lo pone en output.
 * output tiene que tener la memoria reservada.
 * el tamaño de buffer y output es multiplo de 16, si sobraba espacio
 * llenalo con 0.
 * En caso de que no halla una clave guardada u otro error devuelve false
 */
uint32_t Crypto_AesEncryptBuffer(uint8_t *buffer, uint8_t **output, uint32_t size);

/**
 * Usando la clave hardcodeada encripta
 * el buffer y devuelve un puntero a memoria dinamica.
 * Recordar liberar memoria con free
 * En caso de error devuelve NULL
 */
int64_t * Crypto_RsaEncryptBuffer(int8_t * buffer, uint32_t size);

/**
 * Usando la clave hardcodeada desencripta
 * el buffer y devuelve un puntero a memoria dinamica.
 * size es 8 veces el tamaño del buffer original.
 * Recordar liberar memoria con free
 * En caso de error devuelve NULL
 */
int8_t * Crypto_RsaDecryptBuffer(int64_t * encBuffer, uint32_t size);

/**
 * Imprime string como hex
 */
void phex(uint8_t *str, uint32_t size);

#endif /* CRYPTOUTILITIES_H_ */
