//***********************************************************************************************************************************
// *** INCLUDES
// **********************************************************************************************************************************/
#include <stdio.h>
#include <stdbool.h>
#include "Timer.h"
#include "SerialPort.h"
#include "Keyboard.h"
#include "Utilities.h"
#include "Rgb.h"
#include "MainMe.h"
#include "TramaMe.h"
#include "Config.h"
#include "Storage.h"
#include "Flash.h"

// Board Init
#include "board.h"
#include "pin_mux.h"
#include "LPC845.h"
#include "fsl_debug_console.h"
#include "inttypes.h"
/***********************************************************************************************************************************
 *** DEFINES
 **********************************************************************************************************************************/
#define INICIALIZACION_MEMORIA false
#define IMPRIMIR_MEMORIA false

/***********************************************************************************************************************************
 *** MACROS
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** VARIABLES GLOBALES PRIVADAS AL MODULO
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** PROTOTIPO DE FUNCIONES
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** IMPLEMENTACION DE FUNCIONES PUBLICAS
 **********************************************************************************************************************************/

int main(void) {

	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitDebugConsole();

	SerialPort_Init(COM_PORT, BAUD_RATE);
	Keyboard_Init(STANDARD_KEYBOARD);
	TIMER_Systick_Init();
	TIMER_Start(KEYBOARD_TIMER, 5, PERIODIC, &Keyboard_Update);
	Rgb_Init();
	Flash_Init();
	Storage_Init();

#if INICIALIZACION_MEMORIA
	// Seteamos los valores por default de la memoria
	uint8_t * Hash = DEFAULT_HASH;
	Storage_SetHash(Hash);

	Storage_t storage;
	memset(storage.Passwords, EMPTY_PAGE, 16);
	memset(storage.Contacts, EMPTY_PAGE, 16);
	Flash_WritePage(STORAGE_SECTOR, STORAGE_PAGE, (uint32_t*)&storage, sizeof(Storage_t));

	uint8_t pswd[32] = "AWS#6OMKeBV*M0HQ&VB#";
	uint8_t pswd_2[32] = "Spotify#6OMKeBV*M0HQ&VB#";
	Storage_SetPswd(pswd, strlen(pswd));
	Storage_SetPswd(pswd_2, strlen(pswd_2));

	uint8_t contacto[32] = "Alumnos#1888261861:131073#";
	Storage_SetCont(contacto, strlen(contacto));
#endif

#if IMPRIMIR_MEMORIA
	printf("Hash almacenado: %p : %s\n", Storage_GetHashAddr(), Storage_GetHashAddr());

	Storage_Init(); // Reinicializamos porque reescribimos el registro
	Storage_PrintStore();
	uint8_t PswdQ = Storage_GetPswdQ();
	printf("%d contraseñas disponibles\n", PswdQ);
	for (uint8_t i = 0; i < PswdQ; i++ ) printf("Contraseña almacenada: %s\n", Storage_PopPswd());
	uint8_t ContQ = Storage_GetContQ();
	printf("%d contactos disponibles\n", ContQ);
	for (uint8_t i = 0; i < ContQ; i++ ) printf("Contacto almacenado: %s\n", Storage_PopCont());
	Storage_Reset();

#endif

	while (1) {
		TIMER_ProcessEvent();
		TramaMe_Update();
		MainMe_Update();
	}

	return 0;
}

/***********************************************************************************************************************************
 *** IMPLEMNTACION DE FUNCIONES PRIVADAS
 **********************************************************************************************************************************/
