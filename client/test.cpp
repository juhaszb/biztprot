#include <iostream>
#include <cryptopp/aes.h>
#include <cryptopp/osrng.h>
#include "crypto.h"

int main(){

    CryptoPP::AutoSeededRandomPool prng;
    byte key[CryptoPP::AES::MAX_KEYLENGTH];
    prng.GenerateBlock(key, CryptoPP::AES::MAX_KEYLENGTH);
    MyCrypto m(key);

    std::string plain = "Hello";
    std::string rplain;

    std::string cipher = m.encrypt(plain);

    std::cout << "Ciphertext: " << cipher << std::endl;

    std::cout << "The unciphered value: " << std::endl;

    rplain = m.decrypt(cipher);

    std::cout << rplain << std::endl;

    return 0;
}