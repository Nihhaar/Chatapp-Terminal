#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <cstring>
#include <unistd.h>
#include <ctime>
#include <fstream>
#include <vector>
#include <sstream>
#include "json/json.h"
#include <unordered_map>
#include <pthread.h>

using namespace std;

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

#define RCVBUFSIZE 32 /* Size of recieved buffers */

/* Arguments to pass to client thread */
struct ThreadArgs{
	int clientSock;
};

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

string formatted_time();

void *threadHandler(void *arg);

void handleTCPClient(int clientSocket);

void sendDataToClient(string str, int sock);

#endif