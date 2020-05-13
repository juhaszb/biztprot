#ifndef __message_h_
#define __message_h_

#include <string>
#include <iostream>
#include <vector>
#include "message.h"
#include <boost/algorithm/string.hpp>

uint64_t constexpr mix(char m, uint64_t s)
 {
  return ((s<<7) + ~(s>>3)) + ~m;
 }
 
uint64_t constexpr hash(const char * m)
 {
  return (*m) ? mix(*m,hash(m+1)) : 0;
 }

class UICommand
{
    std::string command;


    public:
    UICommand(std::string&& com):command{com}{}
    Message commandcall(std::string&& command) 
    {
        std::vector<std::string> results;
        boost::split(results, command, [](char c){return c == ' ';});

        switch (hash(results[0]))
        {
        case hash("Login"):
            //Message m = new Message...
            //return m;
            break;
        case hash("Register"):
            //Message m = new Message...
            //return m;
            break;
        case hash("MKD"):
            if(results.size()==1){
                std::cout<<"Missing operand!";
            }
            else{
                //Message m = new Message...
                //return m;
            }
            break;
        case hash("RMD"):
            if(results.size()==1){
                std::cout<<"Missing operand!";
            }
            else{
                //Message m = new Message...
                //return m;
            }
            break;
        case hash("GWD"):
            //Message m = new Message...
            //return m;
            break;
        case hash("CWD"):
            if(results.size()==1){
                std::cout<<"Missing operand!";
            }
            else{
                //Message m = new Message...
                //return m;
            }
            break;
        case hash("LST"):
            //Message m = new Message...
            //return m;
            break;
        case hash("UPL"):
            if(results.size()==1){
                std::cout<<"Missing operand!";
            }
            else{
                //Message m = new Message...
                //return m;
            }
            break;
        case hash("DNL"):
            if(results.size()==1){
                std::cout<<"Missing operand!";
            }
            else{
                //Message m = new Message...
                //return m;
            }
            break;
        case hash("RMF"):
            if(results.size()==1){
                std::cout<<"Missing operand!";
            }
            else{
                //Message m = new Message...
                //return m;
            }
            break;
        case hash("EXIT"):
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
