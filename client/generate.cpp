#include "generate.h"
#include <utility>
#include <cryptopp/rsa.h>
#include <cryptopp/cryptlib.h>


std::pair<CryptoPP::RSA::PrivateKey,CryptoPP::RSA::PublicKey> generate_rsa_keys()
{
	
	CryptoPP::AutoSeededRandomPool rnd;
	CryptoPP::RSA::PrivateKey rsaPrivate;
	rsaPrivate.GenerateRandomWithKeySize(rnd,4096);
	
	CryptoPP::RSA::PublicKey rsaPublic(rsaPrivate);

//	SavePublicKey("private.key",rsaprivate);
		
	auto pair = std::make_pair(rsaPrivate,rsaPublic);	
	return pair;

	
}


