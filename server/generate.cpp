#include <cryptopp/cryptlib.h>
#include <cryptopp/queue.h>
#include <cryptopp/rsa.h>
#include <cryptopp/osrng.h>
#include <string>
#include <cryptopp/files.h>
#include "generate.h"




void Save(const std::string& filename, const CryptoPP::BufferedTransformation& bt)
{
	CryptoPP::FileSink file(filename.c_str());
	bt.CopyTo(file);
	file.MessageEnd();
}




void SavePrivateKey(const std::string& filename, const CryptoPP::PrivateKey& key)
{
	CryptoPP::ByteQueue queue;
	key.Save(queue);
	Save(filename,queue);
}


void SavePublicKey(const std::string& filename, const CryptoPP::PublicKey& key)
{
	CryptoPP::ByteQueue queue;
	key.Save(queue);
	Save(filename,queue);
}

void generate_rsa_tofile(const std::string& publickey,const std::string& privatekey)
{
	
	CryptoPP::AutoSeededRandomPool rnd;
	CryptoPP::RSA::PrivateKey rsaprivate;
	rsaprivate.GenerateRandomWithKeySize(rnd,4096);

	CryptoPP::RSA::PublicKey rsapublic(rsaprivate);


	SavePrivateKey(privatekey,rsaprivate);
	SavePublicKey(publickey,rsapublic);
		


}



