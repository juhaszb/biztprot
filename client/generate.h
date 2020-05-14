#ifndef GENERATE_H_
#define GENERATE_H_

#include <cryptopp/cryptlib.h>

#include <cryptopp/rsa.h>
#include <cryptopp/osrng.h>
#include <string>

#include <utility>

void LoadPublicKey(const std::string& filename,

std::pair<CryptoPP::RSA::PrivateKey,CryptoPP::RSA::PublicKey> generate_rsa_keys();

#endif
