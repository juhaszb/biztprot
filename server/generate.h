#ifndef GENERATE_H_
#define GENERATE_H_

#include <string>
#include <cryptopp/cryptlib.h>


void generate_rsa_tofile(const std::string& publickey, const std::string& privatekey);
 
void LoadPrivateKey(const std::string& filename,CryptoPP::PrivateKey& key);

#endif
