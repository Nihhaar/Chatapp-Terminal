#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <cstring>
#include <unistd.h>
#include <pthread.h>

#include "utils.h"

using namespace std;

#define MAXPENDING 5 /* Maximum number of pending connection requests */

int main(int argc, char* argv[])
{
	int servSock;
	int clientSock;
	struct sockaddr_in servAddr;
	struct sockaddr_in clientAddr;
	unsigned short servPort;
	pthread_t threadID;
	struct ThreadArgs *threadargs;
	unsigned int clientLen;

	if(argc!=2){
		cout<<"Usage: "<<argv[0]<<" <Server Port>"<<endl;
		return 0;
	}

	servPort = stoi(argv[1]);

	if((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
		cout<< "Error: socket() failed"<<endl;
		return 0;
	}

	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(servPort);

	if (bind(servSock, (struct sockaddr *) &servAddr,sizeof(servAddr)) < 0){
		cout<<"Error: bind() failed"<<endl;
		return 0;
	}

	if(listen(servSock, MAXPENDING) < 0){
		cout<<"Error: listen() failed"<<endl;
		return 0;
	}

	for(;;){
		
		clientLen = sizeof(clientAddr);

		if((clientSock = accept(servSock, (struct sockaddr*) &clientAddr, &clientLen)) < 0){
			cout<<"Error: accept() failed"<<endl;
			return 0;
		}

		threadargs = new ThreadArgs;
		threadargs->clientSock = clientSock;

		/* Create a thread */
		if(pthread_create(&threadID, NULL, threadHandler, (void*)threadargs) != 0){
			cout<<"Error: pthread_create() failed"<<endl;
			return 0;
		}

		cout<<"Created thread with thread ID: "<<threadID<<endl;
	}

}