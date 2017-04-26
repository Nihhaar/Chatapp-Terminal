#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <cstring>
#include <unistd.h>

#include "utils.h"

using namespace std;

/* Server Info */
char SERVERIP[128];
unsigned short SERVERPORT;

void startChat(string id, string chatid, int sock){

	string msg = "", protocol;
	cout<<"\033[1;35mType message to be sent. Type 'exit' to end the chat.\033[0m\n\n";
		while(msg != "exit"){
		cout<<"\033[1;33mYou: \033[0m";
		getline(cin, msg);
		if(msg!="exit")
			protocol = "SEND " + msg + " TO " + chatid + " FROM " + id;
		sendDataToServer(protocol, sock);
	}

}

void recieveMessages(int sock, string id){

	sendDataToServer("MAP " + id, sock);
	sendDataToServer("PENDING " + id, sock);
	unsigned int length = 0;
  	char* buffer = 0;
  	while(true){
  		readXBytes(sock, sizeof(length), (void*)(&length));
  		length = ntohl(length);
  		buffer = new char[length];
  		readXBytes(sock, length, (void*)buffer);
  		buffer[length] = '\0';
  		cout<<buffer;
  	}
}

void *threadHandler(void* threadargs)
{
	/* Connect to Server */
	int sock;
	while((sock = connectToServer(SERVERIP, SERVERPORT)) == -1){
		continue;
	}

	string id;
	id = ((struct ThreadArgs *)threadargs)->id;

	pthread_detach(pthread_self());
	free(threadargs);

	recieveMessages(sock,id);

	return (NULL);
}

int main(int argc, char* argv[]){

	pthread_t threadID;

	if(argc!=3){
		cout<<"Usage: "<<argv[0]<<" <Server IP>"<<" <Server Port>"<<endl;
		return 0;
	}

	string IP = argv[1];
	strcpy(SERVERIP, IP.c_str());
	SERVERPORT = (unsigned short)stoi(argv[2]);

	/* Connect to Server */
	int sock;
	while((sock = connectToServer(SERVERIP, SERVERPORT)) == -1){
		continue;
	}

	/* Login into server */
	string id;
	id = login(sock);

	while(id == "0"){
	    cout<<"\033[1;31mWrong Username/Password\033[0m\n\n";
		id = login(sock);
	}
	cout<<"\033[1;32mUser Authenticated\033[0m\n\n";

	ThreadArgs* threadargs = new ThreadArgs;
	threadargs->id = id;
	if(pthread_create(&threadID, NULL, threadHandler, (void*)threadargs) != 0){
			cout<<"Error: pthread_create() failed"<<endl;
			return 0;
		}


	string cmd;
	while(true){

	/* List of Commands */
	cout<<"\033[1;35mType the command [type 'help' to see list of commands]:\033[0m ";
	getline(cin,cmd);
    if(cmd=="help") {cout<<"You can try the following commands:"<<endl;
					 cout<<"\033[1;32monline:\033[0m  gives list of online friends"<<endl;
					 cout<<"\033[1;32mfriends:\033[0m gives list of your friends"<<endl;
					 cout<<"\033[1;32mlast_seen \033[3m[\033[1;35musername\033[1;32m]:\033[0m Gives last seen time \033[3m[\033[1;35mof your friend\033[1;0m]\033[0m"<<endl;
					 cout<<"\033[1;32mchat \033[1;35musername:\033[0m Start chat \033[1;35m your friend\033[0m"<<endl;
					 cout<<"\033[1;32mexit:\033[0m Quits the chat client\n\n";
				     continue;
					}

	if(cmd=="online") {
		sendDataToServer("ONLINE " + id, sock);

		unsigned int length = 0;
  		char* buffer = 0;
  		readXBytes(sock, sizeof(length), (void*)(&length));
  		length = ntohl(length);
  		buffer = new char[length];
  		readXBytes(sock, length, (void*)buffer);
  		buffer[length] = '\0';
  		cout<<buffer<<endl;

  		delete[] buffer;

		continue;
	}

	if(cmd=="friends") {

		sendDataToServer("FRIENDS " + id, sock);

		unsigned int length = 0;
  		char* buffer = 0;
  		readXBytes(sock, sizeof(length), (void*)(&length));
  		length = ntohl(length);
  		buffer = new char[length];
  		readXBytes(sock, length, (void*)buffer);
  		buffer[length] = '\0';
  		cout<<buffer<<endl;

  		delete[] buffer;

		continue;
	}

	if(cmd=="exit") {
		sendDataToServer("EXIT " + id, sock);
		close(sock);
		return 0;
	}
	
	/* BEGIN 2 WORD COMMANDS */
	vector<string> v = split(cmd,' ');

	if(v[0]=="last_seen") {
		if(v.size()==1){
			sendDataToServer("LAST_SEEN_ALL " + id, sock);

			unsigned int length = 0;
  			char* buffer = 0;
  			readXBytes(sock, sizeof(length), (void*)(&length));
  			length = ntohl(length);
  			buffer = new char[length];
  			readXBytes(sock, length, (void*)buffer);
  			buffer[length] = '\0';
  			cout<<buffer<<endl;
	  		delete[] buffer;

		}
		else if(v.size()==2){
			sendDataToServer("LAST_SEEN " + v[1] + " " + id, sock);

			unsigned int length = 0;
  			char* buffer = 0;
  			readXBytes(sock, sizeof(length), (void*)(&length));
  			length = ntohl(length);
  			buffer = new char[length];
  			readXBytes(sock, length, (void*)buffer);
  			buffer[length] = '\0';
  			cout<<buffer<<endl;
  			delete[] buffer;
	
		}
		else{
			cout<<"Use the command properly!"<<endl<<endl;
		}
		
		continue;
	}

	if(v[0]=="chat") {
		if(v.size()==2){
			sendDataToServer("CHAT " + v[1] + " " + id, sock);
		}
		else{
			cout<<"Use the command properly!"<<endl<<endl;
		}

		unsigned int length = 0;
  		char* buffer = 0;
  		readXBytes(sock, sizeof(length), (void*)(&length));
  		length = ntohl(length);
  		buffer = new char[length];
  		readXBytes(sock, length, (void*)buffer);
  		buffer[length] = '\0';
		
		string chatid = (string)buffer;
		if(chatid =="0"){
			cout<<"\033[1;31mHe/She is not your friend\033[0m\n";
			continue;
		}

		startChat(id, chatid, sock);
		continue;
	}

	cout<<"Use the command properly! Type 'help to see the list of commands"<<endl<<endl;
	}

	return 0;
}