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

	Connection c;
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

	std::string keyinhex;
	CryptoPP::StringSource(plain,true,new CryptoPP::HexEncoder(new CryptoPP::StringSink(keyinhex)));

        Message keym(std::string{1,KEX},keyinhex,c.getclientTS());
	c.incerementclientTS();

	std::string test="test message";

	CryptoPP::RSAES_OAEP_SHA_Encryptor e(pkey);


	CryptoPP::StringSource ss(keym.toByteStream(),true,new CryptoPP::PK_EncryptorFilter(rng,e,new CryptoPP::StringSink(cipher)));

	

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
		
		
		
	



		m.setTimestamp(c.getclientTS());
		c.incerementclientTS();
		//std::cout<<"key is :" <<std::string{reinterpret_cast<char*>(key)}<<std::endl;
		std::string ciph = mm.encrypt(m.toByteStream());
		//std::cout<<ciphex.length()<<std::endl;
		//std::cout<<ciphex<<std::endl;
		//std::cout<<"length of ciphex"<< ciphex.length()<< std::endl;
		c.Send(ciphex);
		
		//std::cout<<ciphex<<std::endl;

		std::cout<<c.Read()<<std::endl;

		std::string loginenc = mm.encrypt(m.toByteStream());
		std::string login;
		CryptoPP::StringSource(loginenc,true,new CryptoPP::HexEncoder(new CryptoPP::StringSink(login)));

		c.Send(login);
		
		

		std::string loginresp;
		std::string loginresphex = c.Read();
	
		CryptoPP::StringSource(loginresphex,true,new CryptoPP::HexDecoder(new CryptoPP::StringSink(loginresp)));


		std::string loginrespdecoded = mm.decrypt(loginresp);
		//std::cout<<loginrespdecoded<<std::endl;
		//std::cout<< "The type we got "<<Message::fromString(loginrespdecoded).getType()<<std::endl;
		if(Message::fromString(loginrespdecoded).getType() == ERROR)
			return 0;
		

		while(true){
		std::string readstring;
		//std::cin>>readstring;
		std::getline(std::cin,readstring);
		Message read = ui.commandcall(readstring);
		if(read.getType() == ERROR)
			std::cout<<read.getData()<<std::endl;
		else if(read.getType() == EXIT)
			break;
		else if(read.getType() == LOGIN || read.getType() == REGISTER)
		{
			std::cout <<"You are already logged in!"<<std::endl;
			break;
		}
		else
		{
			std::string enc  = mm.encrypt(read.toByteStream());
	
			std::string stringsend;
			CryptoPP::StringSource s3(enc,true,new CryptoPP::HexEncoder(new CryptoPP::StringSink(stringsend)));
	
			//std::cout<<stringsend<<std::endl;

			//std::cout<<mm.decrypt(enc)<<std::endl;
			c.Send(stringsend);

			std::string resp = c.Read();
			std::string respdec;

			CryptoPP::StringSource(resp,true,new CryptoPP::HexDecoder(new CryptoPP::StringSink(respdec)));

			//std::cout<<mm.decrypt(respdec)<<std::endl;
			//std::cout<<"response"<<resp<<std::endl;
			std::string decrypted = mm.decrypt(respdec);
			std::cout<<Message::fromString(decrypted).getData()<<std::endl;
		}
		}

	}
	catch(std::exception& e)
	{
		std::cout<<"Exception caught"<<std::endl;
	}
	return 0;
}
