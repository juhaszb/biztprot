#include "crypto.h"

MyCrypto::MyCrypto(byte k[CryptoPP::AES::MAX_KEYLENGTH]){
    //init key
    for(int i = 0; i < CryptoPP::AES::MAX_KEYLENGTH; i++){
        MyCrypto::key[i] = k[i];
    }
}

std::string MyCrypto::encrypt(std::string plaintext){
    std::string cipher;

    byte iv[CryptoPP::AES::BLOCKSIZE];
    prng.GenerateBlock(iv, sizeof(iv));
    
   // try{
        CryptoPP::GCM<CryptoPP::AES>::Encryption e;
        e.SetKeyWithIV(MyCrypto::key, sizeof(MyCrypto::key), iv, sizeof(iv));

        
        CryptoPP::StringSource(plaintext, true, new CryptoPP::AuthenticatedEncryptionFilter(e, new CryptoPP::StringSink(cipher)));
    //}
   // catch(const std::exception& e){
    //    std::cerr << e.what() << '\n';
  //  }
    
    //converting byte[] to string
    std::string iv_s(reinterpret_cast<const char*>(iv), CryptoPP::AES::BLOCKSIZE);

    //std::cout << iv_s.length() << std::endl;

    return iv_s + cipher;
}

std::string MyCrypto::decrypt(std::string ciphertext){
    std::string plain;
    byte iv[CryptoPP::AES::BLOCKSIZE];

    std::string iv_s = ciphertext.substr(0, CryptoPP::AES::BLOCKSIZE);
    for(int i = 0; i < CryptoPP::AES::BLOCKSIZE; i++){
        iv[i] = (byte)iv_s[i];
    }

    ciphertext.erase(0, CryptoPP::AES::BLOCKSIZE);

    //try
    //{
        CryptoPP::GCM<CryptoPP::AES>::Decryption d;
        d.SetKeyWithIV(MyCrypto::key, sizeof(MyCrypto::key), iv, sizeof(iv));
        CryptoPP::StringSource s(ciphertext, true, new CryptoPP::AuthenticatedDecryptionFilter(d, new CryptoPP::StringSink(plain)));
    //}
    /*catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }*/
    
    
    return  plain;
}
