/***********************************************************************
 * INCLUDES
 ***********************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Config.h"
#include "Timer.h"
#include "TramaMe.h"
#include "inttypes.h"
#include "SerialPort.h"
#include "CryptoUtilities.h"

/***********************************************************************
 * DEFINES
 ***********************************************************************/
// Estados
#define S_RESET 0
#define S_INIT 1
#define S_LOAD_COMMAND 2
#define S_LOAD_DATA 3

/***********************************************************************
 * VARIABLES PRIVADAS AL MODULO
 ***********************************************************************/

// Variables de estados
static uint8_t State = S_RESET;
static uint8_t Buffer[BUFF_MAX_SIZE];
static uint8_t BufferPos = 0;

// Variables comandos disponibles
static bool NewCommandAvailable = false;
static bool NewDataAvailable = false;
static uint8_t CommandAvailable = 0;
static uint8_t DataAvailable[BUFF_MAX_SIZE];
static uint32_t DataAvailableSize = 0;

/**********************************************************************
 * PROTOTIPOS FUNCIONES PRIVADAS AL MODULO
 **********************************************************************/
/**
 * Resetea el timer de procesarTrama usado para determinar
 * el fin de la recepcion de datos
 */
static void ResetTimer(void);

/**
 * setea el estado de la maquina de estados de procesar trama a
 * S_RESET cosa de que deje de guardar data y queda a la espera
 * de un nuevo inicio de trama
 */
static void EndReception(void);

/**********************************************************************
 * IMPLEMENTACION DE FUNCIONES PUBLICAS
 **********************************************************************/

void TramaMe_Update() {
	static uint8_t ByteAvailable, IsByteAvailable;

	IsByteAvailable = SerialPort_IsByteAvailable(COM_PORT);
	if (IsByteAvailable) {
		ByteAvailable = SerialPort_GetByte(COM_PORT);
		SerialPort_Send(COM_PORT, "0", 1); // Enviamos confirmacion de recepcion
	}

	switch (State) {
	case S_RESET:
		BufferPos = 0;
		printf("StateTrama = S_INIT\n");
		State = S_INIT;
		break;
	case S_INIT:
		if (IsByteAvailable) {
			if (ByteAvailable == LIMIT_CHAR) {
				ResetTimer();
				printf("StateTrama = S_LOAD_COMMAND\n");
				State = S_LOAD_COMMAND;
			}
		}
		break;
	case S_LOAD_COMMAND:
		if (IsByteAvailable) {
			// Nuevo comando disponible, lo guardamos en las variables globales y esperamos data
			// newCommand available lo habilitamos al final cuando hacemos el timeout de la recepcion de data
			CommandAvailable = ByteAvailable;
			ResetTimer();
			printf("StateTrama = S_LOAD_DATA : %d\n", CommandAvailable - '0');
			State = S_LOAD_DATA;
		}
		break;
	case S_LOAD_DATA:
		if (IsByteAvailable) {
			// Seguimos recibiendo data, guardamos, reseteamos el timeout y seguimos
			Buffer[BufferPos] = ByteAvailable;
			//			printf("%"PRIu8":%d\n", ByteAvailable, BufferPos);
			BufferPos++;
			ResetTimer();
		}
		break;
	default:
		State = S_RESET;
		break;
	}
}

int8_t TramaMe_GetCommandAvailable(void) {
	if (NewCommandAvailable == true) {
		NewCommandAvailable = false;
		return CommandAvailable;
	}

	return NO_COMMAND;
}

uint8_t TramaMe_GetDataAvailable(uint8_t **data) {
	int RetVal = DataAvailableSize;

	if (NewDataAvailable && DataAvailableSize) {
		printf("TramaMe_GetDataAvailable: DataAvailableSize = %u\n", DataAvailableSize);

		(*data) = (uint8_t*) malloc(DataAvailableSize);
		if ((*data) != NULL)  {
			memcpy((*data), DataAvailable, DataAvailableSize);
		}
		NewDataAvailable = false;
		DataAvailableSize = 0;
	}

	return RetVal;
}

uint8_t TramaMe_GetDecryptedData(uint8_t **data) {
	int RetVal = DataAvailableSize;

	if (NewDataAvailable && DataAvailableSize) {
		printf("TramaMe_GetDataAvailable: DataAvailableSize = %u\n", DataAvailableSize);

		(*data) = (uint8_t*) malloc(DataAvailableSize);
		if ((*data) != NULL)  {
			Crypto_AesDecryptBuffer(DataAvailable, data, DataAvailableSize);
		}

		NewDataAvailable = false;
		DataAvailableSize = 0;
	}

	return RetVal;
}

void TramaMe_SendEncryptedData(uint8_t command, uint8_t * data, uint32_t dataSize) {
	static uint8_t tmp[500];

	uint8_t * EncData = NULL;
	Crypto_AesEncryptBuffer(data, &EncData, dataSize);

	memset(tmp, '\0', 500);
	sprintf(tmp, "#%c%s", command, EncData);
	// SerialPort_Send(COM_PORT, tmp, (uint32_t) strlen(tmp));
	SerialPort_Send(COM_PORT, tmp, (uint32_t) dataSize+2);

	free(EncData);
}

void TramaMe_SendComand(uint8_t command) {
	static uint8_t tmp[2];
	tmp[0] = '#';
	tmp[1] = command;

	SerialPort_Send(COM_PORT, tmp, 2);
}

/**********************************************************************
 * IMPLEMENTACION FUNCIONES PRIVADAS
 **********************************************************************/

static void EndReception() {
	if (State == S_LOAD_DATA) {
		NewCommandAvailable = true;
		NewDataAvailable = false;

		if (BufferPos > 0) {
			NewDataAvailable = true;
			DataAvailableSize = BufferPos;
			memcpy(DataAvailable, Buffer, DataAvailableSize);
		}
	}

	printf("StateTrama = S_RESET\n");
	State = S_RESET;
}

static void ResetTimer(void) {
	TIMER_Stop(TIMER_TRAMA);
	TIMER_Start(TIMER_TRAMA, 500, SINGLE_SHOT, &EndReception);
}

