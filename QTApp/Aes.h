
#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _AES_H_
#define _AES_H_

#include <stdint.h>

#define ECB 1
#define AES128_KEY_SIZE 16

void AES128_ECB_encrypt(uint8_t *input, const uint8_t *key, uint8_t *output);
void AES128_ECB_decrypt(uint8_t *input, const uint8_t *key, uint8_t *output);

#endif //_AES_H_



#ifdef __cplusplus
}
#endif
