#include "generate.h"
#include <utility>
#include <cryptopp/rsa.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/files.h>
#include <string>


void Load(const std::string& filename,CryptoPP::BufferedTransformation& bt)
{
	CryptoPP::FileSource file(filename.c_str(),true);

	file.TransferTo(bt);
	bt.MessageEnd();
}


void LoadPublicKey(const std::string& filename,CryptoPP::PublicKey& key)
{
	CryptoPP::ByteQueue queue;
	Load(filename,queue);

	key.Load(queue);
}



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


