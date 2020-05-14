#ifndef __ui_h_
#define __ui_h_

#include <string>
#include <iostream>
#include <vector>
#include "message.h"
#include <boost/algorithm/string.hpp>
#include <map>
#include "messagetype.h"
/*
uint64_t constexpr mix(char m, uint64_t s)
 {
  return ((s<<7) + ~(s>>3)) + ~m;
 }
 
uint64_t constexpr hash(const char * m)
 {
  return (*m) ? mix(*m,hash(m+1)) : 0;
 }
*/


int getch() {
    int ch;
    struct termios t_old, t_new;

    tcgetattr(STDIN_FILENO, &t_old);
    t_new = t_old;
    t_new.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t_new);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &t_old);
    return ch;
}





string getpass(const char *prompt, bool show_asterisk=true)
{
  const char BACKSPACE=127;
  const char RETURN=10;

  string password;
  unsigned char ch=0;

  cout <<prompt<<endl;

  while((ch=getch())!=RETURN)
    {
       if(ch==BACKSPACE)
         {
            if(password.length()!=0)
              {
                 if(show_asterisk)
                 cout <<"\b \b";
                 password.resize(password.length()-1);
              }
         }
       else
         {
             password+=ch;
             if(show_asterisk)
                 cout <<'*';
         }
    }
  cout <<endl;
  return password;
}




class UICommand
{
    std::string command;

    std::map<std::string,int> options;
    public:
    UICommand()
    {
	options.emplace(std::make_pair("Login",LOGIN));
	options.emplace(std::make_pair("Register",REGISTER));
	options.emplace(std::make_pair("MKD",MKD));
	options.emplace(std::make_pair("RMD",RMD));
	options.emplace(std::make_pair("GWD",GWD));
	options.emplace(std::make_pair("CWD",CWD));
	options.emplace(std::make_pair("LST",LST));
	options.emplace(std::make_pair("UPL",UPL));
	options.emplace(std::make_pair("DNL",DNL));
	options.emplace(std::make_pair("RMF",RMF));
	options.emplace(std::make_pair("EXIT",EXIT));
    };
    UICommand(std::string&& com):command{com}{}



   void commandcall(std::string& command) 
    {
        std::vector<std::string> results;
        boost::split(results, command, [](char c){return c == ' ';});

        switch (options[results[0]])
        {
        case LOGIN:
	    {
	    std::cout<<"Please enter your username"<<std::endl;
            std::string username;
	    std::cin>>username;

            //Message m = new Message...
            //return m;
	    std::cout<<"login volt"<<std::endl;
            break;
	    }
	case REGISTER:
            //Message m = new Message...
            //return m;
            break;
        case MKD:
            if(results.size()!=2){
                std::cout<<"Missing or too much operand!";
            }
            else{
                //Message m = new Message...
                //return m;
            }
            break;
        case RMD:
            if(results.size()!=2){
                std::cout<<"Missing or too much operand!";
            }
            else{
                //Message m = new Message...
                //return m;
            }
            break;
        case GWD:
            //Message m = new Message...
            //return m;
            break;
        case CWD:
            if(results.size()!=2){
                std::cout<<"Missing or too much operand!";
            }
            else{
                //Message m = new Message...
                //return m;
            }
            break;
        case LST:
            //Message m = new Message...
            //return m;
            break;
        case UPL:
            if(results.size()!=2){
                std::cout<<"Missing or too much operand!";
            }
            else{
                //Message m = new Message...
                //return m;
            }
            break;
        case DNL:
            if(results.size()!=2){
                std::cout<<"Missing or too mcuh operand!";
            }
            else{
                //Message m = new Message...
                //return m;
            }
            break;
        case RMF:
            if(results.size()!=2){
                std::cout<<"Missing or too much operand!";
            }
            else{
                //Message m = new Message...
                //return m;
            }
            break;
        case EXIT:
            //Message m = new Message...
            //return m;
            break;
        default:
            std::cout<<"Command not found";
            break;
        }
    }
};



#endif
