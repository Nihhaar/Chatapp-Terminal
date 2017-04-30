#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <vector>
#include <sstream>
#include <pthread.h>
#include <cstring>
#include <fstream>

using namespace std;

# define RCVBUFSIZE 32

/* Arguments to pass to client thread */
struct ThreadArgs{
	string id;
};


// These typedefs will vary across different platforms
// such as linux, win32, OS/X etc, but the idea
// is that a Int8 is always 8 bits, and a UInt32 is always
// 32 bits regardless of the platform you are on.
// These vary from compiler to compiler, so you have to 
// look them up in the compiler documentation.
typedef char Int8;
typedef short int Int16;
typedef int Int32;

typedef unsigned char UInt8;
typedef unsigned short int UInt16;
typedef unsigned int UInt32;

/**
	Splits a string into tokens
	@param s string to be split
	@param delim delimitter used to split the string
	@return The vector of tokens
*/
vector<string> split(const string &s, char delim);

/**
	Tells if a string is an integer
	@param s string to be examined
	@return boolean representing whether the string is integer
*/
inline bool isInteger(const std::string & s);

/**
	Takes a char input without echoing
	@return the int value of char
*/
int getch();

/**
	Reads input from terminal without echoing
	@return The masked input
*/
string getMaskedInput();

/**
	Connects to the server
	@param servIP IP address of server
	@param servPort Port number of server
	@return The socket descriptor
*/
int connectToServer(char* servIP, unsigned short servPort);

/**
	Reads x bytes from the TCP byte stream using socket and stores into buffer
	@param socket socketdescriptor of the client
	@param x number of bytes to read
	@param buffer buffer to store the data into
	@return NULL
*/
void readXBytes(int socket, unsigned int x, void* buffer);

/**
	Tries to login into the server
	@param sock socketdescriptor of the client
	@return "id" of the user if authenticated, "0" if not
*/
string login(int sock);

/**
	Tries to register into the server
	@param sock socketdescriptor of the client
	@return "id" of the user if registered, "0" if not
*/
string regist(int sock);

/**
	Sends string data to server
	@param str string to be sent
	@param sock socketdescriptor of the client
	@return NULL
*/
void sendDataToServer(string str, int sock);

#endif