#ifndef CRYPTO_H_
#define CRYPTO_H_


#include <string>
#include <cryptopp/gcm.h>
#include <cryptopp/osrng.h>
#include <cryptopp/aes.h>
#include <iostream>

class MyCrypto {
private:
CryptoPP::AutoSeededRandomPool prng;
byte key[CryptoPP::AES::DEFAULT_KEYLENGTH];
const int TAG_SIZE = 12;
public:
MyCrypto(byte key[CryptoPP::AES::DEFAULT_KEYLENGTH]);
std::string encrypt(std::string plaintext);
std::string decrypt(std::string ciphertext);
};

#endif