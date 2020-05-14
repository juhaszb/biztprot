#include <cryptopp/filters.h>
#include <cryptopp/osrng.h>
#include <cryptopp/rsa.h>
#include <cryptopp/aes.h>
#include <exception>
#include <iostream>
#include "generate.h"
#include "connection.h"
#include <string>
#include "message.h"
#include "ui.h"
#include "crypto.h"

#ifndef byte
typedef unsigned char byte; 
#endif




int main(int argc, char*argv[])
{
	CryptoPP::AutoSeededRandomPool rng;
	//std::pair<CryptoPP::RSA::PrivateKey,CryptoPP::RSA::PublicKey> keys = generate_rsa_keys();	
	CryptoPP::RSA::PublicKey pkey;
	LoadPublicKey("public.key",pkey);
	
	std::string cipher;

	CryptoPP::AutoSeededRandomPool prng;
	byte key[CryptoPP::AES::MAX_KEYLENGTH];
	prng.GenerateBlock(key,CryptoPP::AES::MAX_KEYLENGTH);
	MyCrypto mm(key);
	
	std::string plain = std::string{reinterpret_cast<char*>(key)};
	

	CryptoPP::RSAES_OAEP_SHA_Encryptor e(pkey);


	CryptoPP::StringSource ss(plain,true,new CryptoPP::PK_EncryptorFilter(rng,e,new CryptoPP::StringSink(cipher)));
	//std::cout<<cipher<<std::endl;
	//Message m("ab","baaab",12,25);
	//std::cout<<m.toByteStream()<<std::endl;
		
	bool loggedin = false;

	std::cout<<"Welcome to our simple FTP program!"<<std::endl;
	std::cout<<"Please enter whether you would like to Register or Login" <<std::endl;
	std::string in;
	std::cin >> in;
	UICommand ui{};
	Message m = ui.commandcall(in);
	std::cout <<m.toByteStream() <<std::endl;
	try{
		Connection c;
		std::string ciph = mm.encrypt(m.toByteStream());
		c.Send(cipher);
	}
	catch(std::exception& e)
	{
		std::cout<<"Exception caught"<<std::endl;
	}
	return 0;
}
