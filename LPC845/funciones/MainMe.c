/***********************************************************************
 * INCLUDES
 ***********************************************************************/
#include <stdio.h>
#include <string.h>
#include "Config.h"
#include "MainMe.h"
#include "TramaMe.h"
#include "Keyboard.h"
#include "inttypes.h"
#include "Utilities.h"
#include "CryptoUtilities.h"
#include <stdlib.h>
#include "Storage.h"

/***********************************************************************
 * CONSTANTES
 ***********************************************************************/
// Estados maquina de estados
#define S_IDLE 0
#define S_PING 1
#define S_AUTH 2
#define S_INIT 3
#define S_RESET 4
#define S_LOAD_DATA 5
#define S_LOAD_COMMAND 6
#define S_NO_CONNECTION 7
#define S_AWAIT_SESSION_KEY 8
#define S_AWAIT_CONNECTION_CONFIRMATION 9

// Commands
#define C_GET_PK '0'
#define C_POST_SESSION_KEY '1'
#define C_POST_PING '2'
#define C_PUT_PK '3'
#define C_FAILED_PING '4'
#define C_OK_PING '5'
#define C_AUTH '6'
#define C_CREATE_SIGNATURE '7'
#define C_FAILED_AUTH '8'
#define C_AUTH_SUCCESS '9'
#define C_AUTH_POST_NEW_HASH 'a'
#define C_END_SESSION 'b'
#define C_LOAD_KEY 'c'
#define C_NO_KEY_AVAILABLE 'd'
#define C_NO_CONTACT_AVAILABLE 'e'
#define C_LOAD_CONTACT 'f'
#define C_DELETE_KEY 'g'
#define C_NEW_KEY 'h'
#define C_NEW_CONTACT 'i'
#define C_DELETE_CONTACT 'j'
#define C_ERROR 'x'


/***********************************************************************
 * VARIABLES PRIVADAS AL MODULO
 ***********************************************************************/
static uint8_t tmp[50];
static uint32_t DataSize;
static bool isAdmin = false;
static uint8_t State = S_RESET;
static int8_t command = NO_COMMAND;
static uint8_t * DataAvailable = NULL;
/***********************************************************************
 * PROTOTIPOS FUNCIONES PRIVADAS AL MODULO
 ***********************************************************************/

/**
 * Setea el state de mdeMain a S_RESET
 */
static void ResetState(void);
static void C_GetPk(void);
static void C_SendPk(void);
static void C_PostSessionKey(void);
static void C_PostPing(void);
static void C_Auth(void);
static void C_AuthPostNewHash(void);
static void C_CreateSignature(void);
static void C_LoadKey(void);
static void C_NewKey(void);
static void C_DeleteKey(void);
static void C_LoadContact(void);
static void C_NewContact(void);
static void C_DeleteContact(void);
static void C_EndSession(void);


/***********************************************************************
 * IMPLEMENTACION DE FUNCIONES PUBLICAS
 ***********************************************************************/

void MainMe_Update(void) {

	command = TramaMe_GetCommandAvailable();

	// if (command != NO_COMMAND) printf("State: %d Command: %c\n", State, command);

	switch (State) {
	case S_RESET:
		isAdmin = false;
		State = S_NO_CONNECTION;
		printf("S_NO_CONNECTION");
		break;

	case S_NO_CONNECTION:
		// Esperamos pedido de clave publica
		if (command == C_GET_PK) C_GetPk();
		break;

	case S_AWAIT_CONNECTION_CONFIRMATION:
		// Esperamos la confirmacion del usuario
		if (Keyboard_GetKey() == CONFIRMATION_KEY) C_SendPk();
		break;

	case S_AWAIT_SESSION_KEY:
		// Esperamos a que nos envien la clave de la conexion encriptada con RSA
		DataSize = TramaMe_GetDataAvailable(&DataAvailable);
		if (command == C_POST_SESSION_KEY && DataAvailable != NULL) C_PostSessionKey();
		break;

	case S_PING:
		// Esperamos a que nos confirmen una conexion exitosa
		DataSize = TramaMe_GetDecryptedData(&DataAvailable);
		if (command == C_POST_PING && DataAvailable != NULL) C_PostPing();
		break;

	case S_IDLE:
		if(command == C_AUTH) C_Auth();
		else if(command == C_AUTH_POST_NEW_HASH) C_AuthPostNewHash();
		else if (command == C_CREATE_SIGNATURE) C_CreateSignature();
		else if(command == C_LOAD_KEY) C_LoadKey();
		else if(command == C_DELETE_KEY) C_DeleteKey();
		else if(command == C_NEW_KEY) C_NewKey();
		else if(command == C_LOAD_CONTACT) C_LoadContact();
		else if(command == C_DELETE_CONTACT) C_DeleteContact();
		else if(command == C_NEW_CONTACT) C_NewContact();
		else if (command == C_END_SESSION) C_EndSession();
		break;

	default:
		State = S_RESET;
		break;
	}

	// Liberamos memoria
	if (DataAvailable) free(DataAvailable);
}

/***********************************************************************
 * IMPLEMENTACION DE FUNCIONES PRIVADAS AL MODULO
 ***********************************************************************/
static void ResetState(void) {
	AlertIdleState();

	if (State == S_AWAIT_CONNECTION_CONFIRMATION) {
		TramaMe_SendComand(C_ERROR);
	}

	State = S_RESET;
}

static void C_GetPk(void) {
	AlertPending();
	TIMER_Start(TIMER_MDEMAIN, CONFIRMATION_TIMEOUT, SINGLE_SHOT, &ResetState);

	State = S_AWAIT_CONNECTION_CONFIRMATION;
	printf("State: S_AWAIT_CONNECTION_CONFIRMATION\n");
}

static void C_SendPk(void) {
	TIMER_Stop(TIMER_MDEMAIN);
	AlertOK();

	public_key_class_t *pub = Crypto_GetPublicKey();

	memset(tmp, '\0', 50);
	sprintf(tmp, "#%c%" PRId64 "#%" PRId64, C_PUT_PK, pub->modulus,
			pub->exponent);
	printf("%s\n", tmp);
	SerialPort_Send(COM_PORT, (uint8_t*) tmp, (uint32_t) strlen(tmp));

	State = S_AWAIT_SESSION_KEY;
	printf("State: S_AWAIT_SESSION_KEY\n");
}

static void C_PostSessionKey(void) {
	int8_t * EncSessionKey = Crypto_RsaDecryptBuffer((int64_t *)DataAvailable, DataSize);

	if (EncSessionKey == NULL) {
		printf("No pudimos desencriptar la clave.\n");
		TramaMe_SendComand(C_ERROR);
	}
	else {
		printf("Decrypted AES key: %s\n", EncSessionKey);
		Crypto_SetSessionKey(EncSessionKey);
		free(EncSessionKey);

		uint8_t Ping[16] = "PING"; // AES requiere bloques de 16 bytes
		TramaMe_SendEncryptedData(C_POST_PING, Ping, 16);

		State = S_PING;
		printf("State: S_PING\n");
	}

}

static void C_PostPing(void) {
	// Desencriptamos el dato con AES
	uint8_t *DecPing = DataAvailable;
	printf("PING value: %s\n", DataAvailable);

	if (memcmp(DecPing, "PING", 4) == 0) {
		TIMER_Start(TIMER_MDEMAIN, SESSION_TIMEOUT, SINGLE_SHOT, &ResetState);

		uint8_t Username[16] = USERNAME; // AES requiere bloques de 16 bytes
		TramaMe_SendEncryptedData(C_OK_PING, Username, 16);

		printf("State: S_IDLE\n");
		State = S_IDLE;
	} else {
		printf("State: S_RESET\n");
		TramaMe_SendComand(C_FAILED_PING);
		State = S_RESET;
	}
}

static void C_Auth(void) {
	printf("Validate Hash\n");

	uint8_t * AuthHashDec = NULL;
	TramaMe_GetDecryptedData(&AuthHashDec);
	printf("Received HASH: %s\n", AuthHashDec);
	printf("Storage HASH: %s\n", Storage_GetHashAddr());

	int8_t Match = memcmp(Storage_GetHashAddr(), AuthHashDec, 15);
	if (Match == 0) {
		printf("Admin permisions granted\n");
		TramaMe_SendComand('9');
		isAdmin = true;
	}
	else {
		AlertFAIL();
		printf("AdminPermissions dennied\n");
		TramaMe_SendComand(C_FAILED_AUTH);
		State = S_RESET;
		isAdmin = false;
	}
}

static void C_AuthPostNewHash(void) {
	printf("Post new hash\n");

	uint8_t * AuthHashDec = NULL;
	TramaMe_GetDecryptedData(&AuthHashDec);
	printf("Received HASH: %s\n", AuthHashDec);

	if (isAdmin) {
		printf("Prev hash: %p : %s\n", Storage_GetHashAddr(), Storage_GetHashAddr());
		Storage_SetHash(AuthHashDec);
		printf("Hash: %p : %s\n", Storage_GetHashAddr(), Storage_GetHashAddr());

		TramaMe_SendComand(C_AUTH_SUCCESS);
		printf("Guardamos nuevo hash (NOTA: VERIFICARLO)\n");
	}
	else {
		printf("AdminPermissions dennied\n");
		TramaMe_SendComand(C_FAILED_AUTH);
	}
}

static void C_CreateSignature(void) {
	printf("SignFile\n");
	if (isAdmin){
		uint8_t * FileHash = NULL;
		uint8_t FileHashSize = TramaMe_GetDecryptedData(&FileHash);
		printf("%s\n", FileHash);

		uint8_t * FileSignature = (uint8_t *)Crypto_RsaEncryptBuffer(FileHash, 32);

		TramaMe_SendEncryptedData(C_CREATE_SIGNATURE, FileSignature, 32*8);
		free(FileSignature);

	}
	else {
		AlertFAIL();
		TramaMe_SendComand(C_ERROR);
		printf("Missing permisions\n");
	}
}

static void C_LoadKey(void) {
	printf("Load key\n");

	if (Storage_GetPswdQ() > 0) {
		uint8_t * Password_p = Storage_PopPswd();

		if (Password_p==NULL) {
			printf("Ya enviamos todos los contactos.\n");
			TramaMe_SendComand(C_NO_KEY_AVAILABLE);
		} else {
			uint8_t Password[PSWD_STR_SIZE];
			memcpy(Password, Password_p, PSWD_STR_SIZE);
			TramaMe_SendEncryptedData(C_LOAD_KEY, Password, PSWD_STR_SIZE);
		}
	}
	else {
		printf("No hay claves disponibles.\n");
		TramaMe_SendComand(C_NO_KEY_AVAILABLE);
	}

}

static void C_DeleteKey(void) {
	DataSize = TramaMe_GetDataAvailable(&DataAvailable);
	printf("Delete key\n");
	printf("Index: %d\n", DataAvailable[0]);

	if (isAdmin) {
		Storage_DeletePswd(DataAvailable[0]);
		TramaMe_SendComand(C_DELETE_KEY); // Confirmacion
	} else TramaMe_SendComand(C_ERROR);

}

static void C_LoadContact(void) {
	printf("Load contact\n");

	if (Storage_GetContQ() > 0) {
		uint8_t * Contact_p = Storage_PopCont();

		if (Contact_p == NULL) {
			printf("Ya enviamos todos los contactos.\n");
			TramaMe_SendComand(C_NO_CONTACT_AVAILABLE);
		} else {
			uint8_t Contact[CONT_STR_SIZE];
			memcpy(Contact,Contact_p, PSWD_STR_SIZE);
			TramaMe_SendEncryptedData(C_LOAD_CONTACT, Contact, CONT_STR_SIZE);
		}
	}
	else {
		printf("No hay claves contactos.\n");
		TramaMe_SendComand(C_NO_CONTACT_AVAILABLE);
	}

}

static void C_DeleteContact(void) {
	DataSize = TramaMe_GetDataAvailable(&DataAvailable);

	printf("Delete Contact\n");
	printf("Index: %d\n", DataAvailable[0]);

	if (isAdmin) {
		Storage_DeleteCont(DataAvailable[0]);
		TramaMe_SendComand(C_DELETE_CONTACT); // confirmacion
	} else TramaMe_SendComand(C_ERROR);
}

static void C_NewKey(void) {
	DataSize = TramaMe_GetDecryptedData(&DataAvailable);
	printf("New Key: %s\n", DataAvailable);
	if (Storage_SetPswd(DataAvailable, strlen(DataAvailable))) {
		TramaMe_SendComand(C_NEW_KEY);
		printf("Stored");
	} else TramaMe_SendComand(C_ERROR);
}

static void C_NewContact(void) {
	DataSize = TramaMe_GetDecryptedData(&DataAvailable);
	printf("New contact: %s\n", DataAvailable);
	if (Storage_SetCont(DataAvailable, strlen(DataAvailable))) {
		TramaMe_SendComand(C_NEW_CONTACT);
		// printf("Stored");
	} else TramaMe_SendComand(C_ERROR);
}

static void C_EndSession(void) {
	TramaMe_SendComand(C_END_SESSION);
	Storage_Reset();
	State = S_RESET;
}
