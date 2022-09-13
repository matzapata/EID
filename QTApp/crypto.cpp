#include "crypto.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <QByteArray>
#include <ctime>
#include <QDebug>
#include <QFile>
#include <QCryptographicHash>
#include <math.h>


Crypto::Crypto() {

}

uint8_t * Crypto::KeyGen(int len) {
    uint8_t * password = new uint8_t[len];
    const uint8_t alphanum[] = "0123456789!@$%^&*abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    int string_length = sizeof(alphanum)-1;

    srand(time(0));
    for(uint8_t i = 0; i < len-1; i++) {
        password[i] = (alphanum[rand() % string_length]);
    }
    password[len-1] = '\0';
    return password;
}

void Crypto::SetAesKey(uint8_t * key) {
    strcpy((char *)AesKey, (char *)key);
    AesKeyInitialized = true;

    // qDebug() << "AesKey" << QString((char *)AesKey);
}
void Crypto::SetRsaPubKey(const int64_t modulus, const int64_t exponent) {
    pubKey -> modulus = modulus;
    pubKey -> exponent = exponent;
}
void Crypto::SetRsaPrivKey(const int64_t modulus, const int64_t exponent) {
    privKey -> modulus = modulus;
    privKey -> exponent = exponent;
}



uint32_t Crypto::AesDecryptBuffer(uint8_t *buffer, uint8_t **output, uint32_t size) {
    if (!AesKeyInitialized)
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

        AES128_ECB_decrypt(block, AesKey, out);
        memcpy((*output)+blockNum*16, out, 16);
    }

    return q_blocks*16;
}

uint32_t Crypto::AesEncryptBuffer(uint8_t *buffer, uint8_t **output, uint32_t size) {
    if (!AesKeyInitialized)
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

        AES128_ECB_encrypt(block, AesKey, out);

        memcpy((*output)+blockNum*16, out, 16);

    }

    return q_blocks*16;
}

int64_t * Crypto::RsaEncryptBuffer(int8_t *buffer, uint32_t size) {
    int64_t *encrypted = rsa_encrypt(buffer, size, pubKey);
    if (!encrypted) {
        return NULL;
    }
    return encrypted;
}

int8_t * Crypto::RsaDecryptBuffer(int64_t *encBuffer, uint32_t size) {
    int8_t *decrypted = rsa_decrypt(encBuffer, size, privKey);
    if (!decrypted) {
        return NULL;
    }
    return decrypted;
}

QByteArray Crypto::HashFile(QString &fileName)
{
    QFile f(fileName);
    if (f.open(QFile::ReadOnly)) {
        QCryptographicHash hash(QCryptographicHash::Md5);
        if (hash.addData(&f)) {
            return hash.result();
        }
    }
    return QByteArray();
}

QByteArray Crypto::HashPswd(QString Username, QString Password) {
    QByteArray AuthHash;
    AuthHash.append(Username.toUtf8());
    AuthHash.append(Password.toUtf8());
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(AuthHash); // Add data to the encrypted hash value
    return hash.result(); // return the final hash value
}

public_key_class_t * Crypto::GetRsaPubKey(void) {
    return pubKey;
}
