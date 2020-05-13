#include "crypto.h"

MyCrypto::MyCrypto(byte k[CryptoPP::AES::DEFAULT_KEYLENGTH]){
    //init key
    for(int i = 0; i < CryptoPP::AES::DEFAULT_KEYLENGTH; i++){
        MyCrypto::key[i] = k[i];
    }
}

std::string MyCrypto::encrypt(std::string plaintext){
    std::string cipher;
    
    byte iv[CryptoPP::AES::BLOCKSIZE];
    prng.GenerateBlock(iv, sizeof(iv));
    
    try{
        CryptoPP::GCM<CryptoPP::AES>::Encryption e;
        e.SetKeyWithIV(MyCrypto::key, sizeof(MyCrypto::key), iv, sizeof(iv));
    }
    catch(const std::exception& e){
        std::cerr << e.what() << '\n';
    }
    
    return cipher;
}

std::string MyCrypto::decrypt(std::string ciphertext){
    std::string plain;
    byte iv[CryptoPP::AES::BLOCKSIZE];

    return plain;
}