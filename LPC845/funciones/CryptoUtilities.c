/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "CryptoUtilities.h"
#include "Config.h"
#include <math.h>

/***********************************************************************************************************************************
 *** CONSTANTES
 **********************************************************************************************************************************/


/***********************************************************************************************************************************
 *** VARIABLES GLOBALES PRIVADAS AL MODULO
 **********************************************************************************************************************************/
static uint8_t sessionKey[AES_KEY_LENGHT];
static bool sessionKeyInitialized = false;

static public_key_class_t pub_key = { PUBLIC_KEY_MODULUS,
PUBLIC_KEY_EXPONENT };
static public_key_class_t *pub = &pub_key;
static private_key_class_t priv_key = { PRIVATE_KEY_MODULUS,
PRIVATE_KEY_EXPONENT };
static private_key_class_t *priv = &priv_key;

/***********************************************************************************************************************************
 *** PROTOTIPO DE FUNCIONES PRIVADAS
 **********************************************************************************************************************************/

void Crypto_SetSessionKey(uint8_t *key) {
	strncpy(sessionKey, key, AES_KEY_LENGHT);
	sessionKeyInitialized = true;
}

void KeyGen(uint8_t * password, uint32_t seed, uint8_t N) {

    const uint8_t alphanum[] = "0123456789!@#$%^&*abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int string_length = sizeof(alphanum)-1;

    srand(seed);
    for(uint8_t i = 0; i < N; i++) password[i] = (alphanum[rand() % string_length]);
}

public_key_class_t* Crypto_GetPublicKey(void) {
	return pub;
}

bool Crypto_CmpUsrKeyHash(uint8_t *keyHash) {
	return true;
}

void Crypto_SetUsrKeyHash(uint8_t *keyHash) {
}


uint32_t Crypto_AesDecryptBuffer(uint8_t *buffer, uint8_t **output, uint32_t size) {
    if (!sessionKeyInitialized)
        return 0;

    uint8_t part_size = 16;
    uint8_t block[part_size];
    uint32_t q_blocks;

    q_blocks = (size%part_size == 0)? (int)size/part_size : (int) ceil(size/part_size)+1;

    (*output) = (uint8_t *)malloc(q_blocks*16);

    uint32_t strIndex=0;
    uint8_t out[16];
    for (uint32_t blockNum = 0; blockNum< q_blocks; blockNum++) {

        for (int j=0; j<part_size; j++) {
            block[j] = buffer[strIndex];
            strIndex++;
        }
        AES128_ECB_decrypt(block, sessionKey, out);
        memcpy((*output)+blockNum*16, out, 16);
    }

    return q_blocks*16;
}

uint32_t Crypto_AesEncryptBuffer(uint8_t *buffer, uint8_t **output, uint32_t size) {
    if (!sessionKeyInitialized)
        return 0;

    uint8_t part_size = 16;
    uint8_t block[part_size];
    uint32_t q_blocks;

    q_blocks = (size%part_size == 0)? (int)size/part_size : (int) ceil(size/part_size)+1;

    (*output) = (uint8_t *)malloc(q_blocks*16);

    uint32_t strIndex=0;
    uint8_t out[16];
    for (uint32_t blockNum = 0; blockNum< q_blocks; blockNum++) {
        memset(block, '\0', part_size);

        for (int j=0; j<part_size; j++) {
            if (strIndex >= size ) break;
            block[j] = buffer[strIndex];
            strIndex++;
        }
        AES128_ECB_encrypt(block, sessionKey, out);
        memcpy((*output)+blockNum*16, out, 16);
    }

    return q_blocks*16;
}

int64_t* Crypto_RsaEncryptBuffer(int8_t *buffer, uint32_t size) {
	int64_t *encrypted = rsa_encrypt(buffer, size, pub);
	if (!encrypted) {
		return NULL;
	}
	return encrypted;
}

int8_t* Crypto_RsaDecryptBuffer(int64_t *encBuffer, uint32_t size) {
	int8_t *decrypted = rsa_decrypt(encBuffer, size, priv);
	if (!decrypted) {
		return NULL;
	}
	return decrypted;
}

void phex(uint8_t *str, uint32_t size)
{
  unsigned char i;
  for (i = 0; i < size; ++i)
    printf("%.2x", str[i]);
  printf("\n");
}
