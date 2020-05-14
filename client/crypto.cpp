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

        //TODO: output the ciphertext from AES-GCM to cipher
    }
    catch(const std::exception& e){
        std::cerr << e.what() << '\n';
    }
    
    //converting byte[] to string
    std::string iv_s(reinterpret_cast<const char*>(iv), CryptoPP::AES::BLOCKSIZE);

    return iv_s + cipher;
}

std::string MyCrypto::decrypt(std::string ciphertext){
    std::string plain;
    byte iv[CryptoPP::AES::BLOCKSIZE];

    //TODO: cut off iv from the begining of ciphertext

    try
    {
        CryptoPP::GCM<CryptoPP::AES>::Decryption d;
        d.SetKeyWithIV(MyCrypto::key, sizeof(MyCrypto::key), iv, sizeof(iv));

        //TODO use decrypt and push output to plain
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    
    return  plain;
}