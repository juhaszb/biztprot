Authors:
Czikó Zsolt - V8UE4A
Bak Márton - UJJMHJ
Juhász Balázs - BI3ULX



This document describes the changes from the design phase and documents how to compile the code.


The client and the server is written in C++ using CryptoPP and boost. 

To compile them you need three libs:

G++ version 9 or newer
CryptoPP 6.2 or newer
Boost

If you have all of these libs:

Server: Navigate into the server folder and issue the following command:
g++ -std=c++17 generate.cpp main.cpp crypto.cpp -g -o server.o -l:cryptopp

Client: Navigate into the client folder and issue the following command:
g++ -std=c++17 gemerate.cpp main.cpp crypto.cpp -g -o client.o -l:cryptopp


Before starting the client for the first time you have to copy the key from the server. (public.key)

(You have to start the server first)

Now you can start by starting the server with: ./server.o

After that you can start the client: ./client.o


The commands available from the client:

CWD arg : Change working directory, one argument needed
GWD : Get working directory( same as pwd in nix systems)
LST: List files and directories (same as ls)
RMD arg: Remove directory
RMF arg: Remove file
UPL arg: Upload file
DNL arg: Download file
EXIT : Exit the client
Login: Login, only available before loggin in, after logged in not available
Register: Register, onyl available before logging in, after logged in not available

Note: You can not navigate higher than your own "folder", one folder/user

Changes from the design:


instead of AES-256-CTR we are using AES-256-GCM. With this we dont need to use a separate MAC field.

We dont send the session id, because it provides no further security. When a message arrives we know which client it belongs to (to be precise a client belongs to a socket descriptor)

We maintain two separate timestamps. One server timestamp and one client. They are initialized to the unix time at start. The client sends its initial timestamp during the RSA key exchange.
The server checks the timestamp it got from the client if it is smaller than it's maintained timestamp the message is rejected. If the timestamp is bigger then the server sets the timestamp to the
client timestamp that we just got.


Message formats:
[Message type][Data]            [Timestamp]
[1 byte]      [variable lenght] [4 bytes]

Each message is encoded into hex and then sent through the network.

The files are sent unencrypted in the data field of the message. (the message is encrypted of course). This is beacuse of time constraints.



