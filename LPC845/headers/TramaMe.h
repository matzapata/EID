#ifndef TRAMAME_H_
#define TRAMAME_H_

/********************************************************************************************
 * INCLUDES
 ********************************************************************************************/
#include "inttypes.h"
#include <stdbool.h>

/********************************************************************************************
 * CONSTANTES
 ********************************************************************************************/
#define BUFF_MAX_SIZE 250
#define LIMIT_CHAR 35 // #
#define NO_COMMAND '-'

/********************************************************************************************
 * PROTOTIPOS FUNCIONES PUBLICAS
 ********************************************************************************************/

/**
 * Lee el serial y va actualizando las variables globales de
 * command, newCommandAvailable, arg, newArgAvailable
 */
void TramaMe_Update(void);

/**
 * Devuelve -1 si newCommandAvailable == false
 * o el comando disponible en commandAvailable y en tal caso
 * actualiza la primer variable
 */
int8_t TramaMe_GetCommandAvailable(void);

/**
 * Devuelve vector dinamico con data que recibimos por serial
 * post comando o NULL si no recibimos nada;
 * Recordar liberar la memoria pedida
 */
uint8_t TramaMe_GetDataAvailable(uint8_t **data);

uint8_t TramaMe_GetDecryptedData(uint8_t **data);

void TramaMe_SendEncryptedData(uint8_t command, uint8_t * data, uint32_t dataSize);

void TramaMe_SendComand(uint8_t command);

#endif /* TRAMAME_H_ */
