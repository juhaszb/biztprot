#include <cryptopp/filters.h>
#include <cryptopp/osrng.h>
#include <cryptopp/rsa.h>
#include <exception>
#include <iostream>
#include "generate.h"
#include "connection.h"
#include <string>
#include "message.h"
#include "ui.h"


int main(int argc, char*argv[])
{
	CryptoPP::AutoSeededRandomPool rng;
	std::pair<CryptoPP::RSA::PrivateKey,CryptoPP::RSA::PublicKey> keys = generate_rsa_keys();	
	
	std::string plain="Hello";
	std::string cipher;

	CryptoPP::RSAES_OAEP_SHA_Encryptor e(keys.second);


	CryptoPP::StringSource ss(plain,true,new CryptoPP::PK_EncryptorFilter(rng,e,new CryptoPP::StringSink(cipher)));
	//std::cout<<cipher<<std::endl;
	Message m("ab","baaab",12,25);
	std::cout<<m.toByteStream()<<std::endl;
	UICommand ui{};
	ui.commandcall("Login");
	/*try{
		Connection c;
	}
	catch(std::exception& e)
	{
		std::cout<<"Exception caught"<<std::endl;
	}*/
	return 0;
}
