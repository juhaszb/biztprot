#include <cryptopp/filters.h>
#include <cryptopp/osrng.h>
#include <cryptopp/rsa.h>
#include <cryptopp/aes.h>
#include <exception>
#include <iostream>
#include "generate.h"
#include <fstream>
#include "connection.h"
#include <iterator>
#include <string>
#include "message.h"
#include "ui.h"
#include "crypto.h"
#include <filesystem>
#include <cryptopp/hex.h>

#ifndef byte
typedef unsigned char byte; 
#endif

byte key[CryptoPP::AES::MAX_KEYLENGTH];
std::string EncryptAndEncode(Message m)
{
	std::string enc;
	MyCrypto mc(key);
	enc = mc.encrypt(m.toByteStream());
	std::string hex;
	CryptoPP::StringSource(enc,true,new CryptoPP::HexEncoder(new CryptoPP::StringSink(hex)));
	return hex;
}


Message DecryptAndDecode(std::string hex)
{
	std::string dec;
	CryptoPP::StringSource(hex,true,new CryptoPP::HexDecoder(new CryptoPP::StringSink(dec)));
	MyCrypto mc(key);
	std::string plain = mc.decrypt(dec);
	return Message::fromString(plain);
}


void Sendfile(Connection* c, Message m)
{
	if(!std::filesystem::exists(m.getData()) || !std::filesystem::is_regular_file(m.getData()))
	{
		std::cout<<m.getData() <<std::endl;
		std::cout<<"File doesnt exist or isnt a regular file"<<std::endl;
		return;
	}
		std::ifstream inf{m.getData()};
	int linecount = std::count(std::istreambuf_iterator<char>(inf),std::istreambuf_iterator<char>(),'\n');
	inf.close();
	
	std::ifstream readf{m.getData()};
	c->incerementclientTS();
	std::string enc = EncryptAndEncode(Message(std::string{UPL},m.getData()+";"+std::to_string(linecount),c->getclientTS()));
	c->Send(enc);
	for(int i = 0; i< linecount; i++)
	{
	std::string line;
	std::getline(readf,line);
	std::string resp = c->Read();
	Message res = DecryptAndDecode(resp);
	if(res.getType() == ERROR)
	{
		std::cout<<"Error with uploading... stopping"<<std::endl;
		std::cout<<res.getData()<<std::endl;
		readf.close();
		return;
	}
	std::cout<<"UPL response "<<res.getData()<<std::endl;
	std::string linehex;
	
	CryptoPP::StringSource(line,true,new CryptoPP::HexEncoder(new CryptoPP::StringSink(linehex)));
	c->incerementclientTS();	
	Message sends(std::string{UPL},linehex,c->getclientTS());
	std::string sendenc = EncryptAndEncode(sends);
	c->Send(sendenc);

	}
	std::string resp = c->Read();
	std::cout<<DecryptAndDecode(resp).getData()<<std::endl;
	readf.close();

}
void DownloadFile(Connection * c, Message m )
{
	if(std::filesystem::exists(m.getData()))
	{
		std::cout<<"File already exists"<<std::endl;
		return;
	}
	//std::ofstream ofs(m.getData());

	c->incerementclientTS();

	c->Send(EncryptAndEncode(Message(std::string{DNL},m.getData(),c->getclientTS())));

	std::string start = c->Read();
	Message resp= DecryptAndDecode(start);
	if(resp.getType() == ERROR)
	{
		std::cout<<"Something happened with downloading file"<<std::endl;
		return;
	}
	std::cout<<"First response from server:"<<resp.toByteStream()   <<std::endl;
	std::vector<std::string> spl = split(resp.getData(),";");
	int linecount = std::stoi(spl[1]);
	std::ofstream ofs(spl[0]);
	for(int i = 0 ; i<linecount ; i++)
	{

		c->incerementclientTS();
		c->Send(EncryptAndEncode(Message(std::string{DNL},"DNL",c->getclientTS())));

		std::string encrepl = c->Read();
		Message decrep = DecryptAndDecode(encrepl);
		if(decrep.getType() == ERROR)
		{
			std::cout<<"There was some error";
			ofs.close();
			return;
		}
		std::string decrepnohex;
		CryptoPP::StringSource(decrep.getData(),true,new CryptoPP::HexDecoder(new CryptoPP::StringSink(decrepnohex)));
		ofs.write(decrepnohex.c_str(),strlen(decrepnohex.c_str()));
		ofs.write("\n",1);

	}
	c->incerementclientTS();
	c->Send(EncryptAndEncode(Message(std::string{DNL},"DONE",c->getclientTS())));
	ofs.close();
	std::string ressp = c->Read();
	std::cout<<DecryptAndDecode(ressp).getData()<<std::endl;
	
	//TODO: split... get linecount... for while linecount >0 read and send resp ... 


}

int main(int argc, char*argv[])
{

	if(argc < 3){
		std::cout << "Not enough arguments provided, needed arguments are portnumber and IP address!" << std::endl;
		std::exit(1);
	}

	int portnum;
	std::string address;

	try{
	portnum = atoi(argv[1]);
	address = argv[2];
	}
	catch(std::exception& e){
		std::cout << "Exception caught: " << e.what() << std::endl;
		std::cout << "Provided arguments: 1.: Portnum: " << argv[1] << " 2.: Address: " << argv[2] << std::endl;
		std::exit(1);
	}
	
	Connection c(portnum, address.c_str());
	CryptoPP::AutoSeededRandomPool rng;
	//std::pair<CryptoPP::RSA::PrivateKey,CryptoPP::RSA::PublicKey> keys = generate_rsa_keys();	
	CryptoPP::RSA::PublicKey pkey;
	LoadPublicKey("public.key",pkey);
	
	std::string cipher;

	CryptoPP::AutoSeededRandomPool prng;
	
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
			continue;
		}
		else
		{
			if(read.getType() == UPL)
			{
				Sendfile(&c,read);
			}
			else if(read.getType() == DNL)
			{
				DownloadFile(&c,read);
			}
			else
			{
			//std::cout <<read.getType() <<std::endl;
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

	}
	catch(std::exception& e)
	{
		std::cout<<"Exception caught"<<std::endl;
	}
	return 0;
}
