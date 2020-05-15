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
#include <cryptopp/hex.h>

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
	


	std::string plain="";
	for(int i = 0; i < CryptoPP::AES::MAX_KEYLENGTH ;i ++ )
	{
		plain+=key[i];
	}

	std::string test="test message";

	CryptoPP::RSAES_OAEP_SHA_Encryptor e(pkey);


	CryptoPP::StringSource ss(plain,true,new CryptoPP::PK_EncryptorFilter(rng,e,new CryptoPP::StringSink(cipher)));

	
	std::string ciphex;
        CryptoPP::StringSource s(cipher, true,new CryptoPP::HexEncoder(new CryptoPP::StringSink(ciphex)));	

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
		//std::cout<<"key is :" <<std::string{reinterpret_cast<char*>(key)}<<std::endl;
		std::string ciph = mm.encrypt(m.toByteStream());
		//std::cout<<ciphex.length()<<std::endl;
		//std::cout<<ciphex<<std::endl;
		c.Send(ciphex);
		
		std::cout<<ciphex<<std::endl;

		std::cout<<c.Read()<<std::endl;
		std::string enc  = mm.encrypt(m.toByteStream());
	
		std::string stringsend;
		CryptoPP::StringSource s3(enc,true,new CryptoPP::HexEncoder(new CryptoPP::StringSink(stringsend)));
	
		std::cout<<stringsend<<std::endl;

		std::cout<<mm.decrypt(enc)<<std::endl;
		c.Send(stringsend);

	}
	catch(std::exception& e)
	{
		std::cout<<"Exception caught"<<std::endl;
	}
	return 0;
}
