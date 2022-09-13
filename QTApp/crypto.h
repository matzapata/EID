#ifndef CRYPTO_H
#define CRYPTO_H

#include "Aes.h"
#include "Rsa.h"
#include <QString>

class Crypto
{
public:
    Crypto();


    uint8_t * KeyGen(int len);
    void SetAesKey(uint8_t * key);
    void SetRsaPubKey(const int64_t modulus, const int64_t exponent);
    public_key_class_t * GetRsaPubKey(void);
    void SetRsaPrivKey(const int64_t modulus, const int64_t exponent);
    uint32_t AesDecryptBuffer(uint8_t *buffer, uint8_t **output, uint32_t size);
    uint32_t AesEncryptBuffer(uint8_t *buffer, uint8_t **output, uint32_t size);
    int64_t * RsaEncryptBuffer(int8_t * buffer, uint32_t size);
    int8_t * RsaDecryptBuffer(int64_t * encBuffer, uint32_t size);
    QByteArray HashFile(QString &Filename);
    QByteArray HashPswd(QString Username, QString Password);


private:
    uint8_t AesKey[AES128_KEY_SIZE];
    bool AesKeyInitialized = false;
    public_key_class_t pubKey_obj;
    public_key_class_t * pubKey = &pubKey_obj;
    private_key_class_t privKey_obj;
    private_key_class_t * privKey = &privKey_obj;
};

#endif // CRYPTO_H
