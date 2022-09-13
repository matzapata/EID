
#ifdef __cplusplus
extern "C"
{
#endif

#ifndef RSA_H_
#define RSA_H_

#include <stdint.h>

typedef struct {
  int64_t  modulus;
  int64_t  exponent;
} public_key_class_t;

typedef struct {
  int64_t  modulus;
  int64_t  exponent;
} private_key_class_t;

// This function will encrypt the data pointed to by message. It returns a pointer to a heap
// array containing the encrypted data, or NULL upon failure. This pointer should be freed when
// you are finished. The encrypted data will be 8 times as large as the original data.
int64_t  *rsa_encrypt(const int8_t *message, const unsigned long message_size, const public_key_class_t *pub);

// This function will decrypt the data pointed to by message. It returns a pointer to a heap
// array containing the decrypted data, or NULL upon failure. This pointer should be freed when
// you are finished. The variable message_size is the size in bytes of the encrypted message.
// The decrypted data will be 1/8th the size of the encrypted data.
int8_t *rsa_decrypt(const int64_t  *message, const unsigned long message_size, const private_key_class_t *pub);


int64_t  rsa_modExp(int64_t  b, int64_t  e, int64_t  m);

#endif /* RSA_H_ */


#ifdef __cplusplus
}
#endif
