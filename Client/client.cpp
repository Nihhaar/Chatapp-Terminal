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
int socket_thread;

void cleanup_handler(void *arg )
{
	// Close your socket
	close(socket_thread);
}

void startChat(string id, string chatid, int sock){

	string msg = "", protocol;
	cout<<"\033[1;35mType message to be sent. Type 'exit' to end the chat.\033[0m\n\n";
		while(msg != "exit"){
		cout<<"\033[1;33mYou: \033[0m";
		getline(cin, msg);
		while (msg.length()==0 )
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
	string id;
	id = ((struct ThreadArgs *)threadargs)->id;	
	
	pthread_cleanup_push(cleanup_handler, NULL);

	/* Connect to Server */
	while((socket_thread = connectToServer(SERVERIP, SERVERPORT)) == -1){
		continue;
	}

	pthread_detach(pthread_self());
	free(threadargs);

	recieveMessages(socket_thread,id);

	pthread_cleanup_pop(1);

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

	string cmd = "";	
	string id = "0";
	while(cmd != "y" && cmd != "n"){	
		cout<<"\033[1;35mAre you a new user? [y/n]:\033[0m ";
		getline(cin,cmd);
	}

	if(cmd == "n"){

		/* Login into server */
		id = login(sock);
		while(id == "0"){
	    	cout<<"\033[1;31mWrong Username/Password\033[0m\n\n";
			id = login(sock);
		}
		cout<<"\033[1;32mUser Authenticated\033[0m\n\n";
	}

	else{

		/* Register your Account */
		id = regist(sock);
		while(id == "0"){
			cout<<"\033[1;31mYour username/email has already been registered. Please try again\033[0m\n\n";
			id = regist(sock);
		}
		cout<<"\033[1;32mUser Registered\033[0m\n\n";
	}

	ThreadArgs* threadargs = new ThreadArgs;
	threadargs->id = id;
	if(pthread_create(&threadID, NULL, threadHandler, (void*)threadargs) != 0){
			cout<<"Error: pthread_create() failed"<<endl;
			return 0;
		}


	while(true){

	/* List of Commands */
	cout<<"\033[1;35mType the command [type 'help' to see list of commands]:\033[0m ";
	getline(cin,cmd);
    if(cmd=="help") {cout<<"You can try the following commands:"<<endl;
					 cout<<"\033[1;32monline:\033[0m  gives list of online friends"<<endl;
					 cout<<"\033[1;32mfriends:\033[0m gives list of your friends"<<endl;
					 cout<<"\033[1;32mlast_seen \033[3m[\033[1;35musername\033[1;32m]:\033[0m Gives last seen time \033[3m[\033[1;35mof your friend\033[1;0m]\033[0m"<<endl;
					 cout<<"\033[1;32mchat \033[1;35musername:\033[0m Start chat with\033[1;35m your friend\033[0m"<<endl;
					 cout<<"\033[1;32madd \033[1;35musername:\033[0m Send friend request/accept if he already sent\033[0m"<<endl;
					 cout<<"\033[1;32mpending :\033[0m See pending friend requests[add \033[1;35musername\033[0m to accept the request"<<endl;
					 cout<<"\033[1;32mlist:\033[0m gives list of members"<<endl;
					 cout<<"\033[1;32mgroups:\033[0m gives list of your groups"<<endl;
					 cout<<"\033[1;32msendfile \033[1;35musername\033[0m location_of_file: Sends the binary file to\033[1;35m your friend\033[0m"<<endl;					 
					 cout<<"\033[1;32mgroup \033[1;35musername1 username2[...] group_name:\033[0m Create groups with specified username and group name"<<endl;					 
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

	if(cmd == "pending"){

		sendDataToServer("REQUESTS " + id, sock);

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

	if(cmd == "list"){

		sendDataToServer("LIST", sock);

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

	if(cmd == "groups"){
		sendDataToServer("LISTGROUPS " + id, sock);

		unsigned int length = 0;
  		char* buffer = 0;
  		readXBytes(sock, sizeof(length), (void*)(&length));
  		length = ntohl(length);
  		buffer = new char[length];
  		readXBytes(sock, length, (void*)buffer);
  		buffer[length] = '\0';
		
		string temp = (string)buffer;
		if(temp == ""){
			cout<<"\033[1;31mYou are not part of any group\033[0m\n";
		}
		else{
			cout<<buffer<<endl;
		}

  		delete[] buffer;

		continue;
	}

	if(cmd=="exit") {
		sendDataToServer("EXIT " + id, sock);
		close(sock);
		pthread_cancel(threadID);
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

	if(v[0]=="sendfile") {
		string fileloc = v[2];

		char * buffer; //buffer to store file contents
		long size;     //file size
		ifstream file (filename, ios::in|ios::binary|ios::ate);     //open file in binary mode, get pointer at the end of the file (ios::ate)
		size = file.tellg();     //retrieve get pointer position
		file.seekg (0, ios::beg);     //position get pointer at the begining of the file
		buffer = new char [size];     //initialize the buffer
		file.read (buffer, size);     //read file to buffer
		file.close();     //close file

		string data = (string)buffer;
		sendDataToServer("SENDFILE " + v[1] + " " + id, sock);
		sendDataToServer(data);
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

	if(v[0]=="add"){
		if(v.size()==2){
			sendDataToServer("ADD " + v[1] + " " + id, sock);
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

		string tempid = (string) buffer;
		if(tempid =="0"){
			cout<<"\n\033[1;31mHe/She is not a member\033[0m\n\n";
		}
		else if(tempid == "-1"){
			cout<<"\n\033[1;33mFriend request accepted\033[0m\n\n";
		}
		else if(tempid == "-2"){
			cout<<"\n\033[1;31mHe is already your friend\033[0m\n\n";
		}
		else{
			cout<<"\n\033[1;33mFriend request sent\033[0m\n\n";
		}
		continue;
	}

	if(v[0]=="group"){
		int n = v.size();
		string msg = "GROUP " + v[n-1] +" " + id + " ";
		for(int i=1; i<=n-2; i++){
			msg+= v[i] + " ";
		}
		sendDataToServer(msg, sock);

		unsigned int length = 0;
  		char* buffer = 0;
  		readXBytes(sock, sizeof(length), (void*)(&length));
  		length = ntohl(length);
  		buffer = new char[length];
  		readXBytes(sock, length, (void*)buffer);
  		buffer[length] = '\0';

		string tempid = (string) buffer;
		if(tempid == "0"){
			cout<<"\n\033[1;31mA group with same name already exists.\033[0m\n\n";
		}
		else{
			cout<<"\n\033[1;33mSuccessfully created the group\033[0m\n\n";
		}

		continue;
	}

	cout<<"Use the command properly! Type 'help to see the list of commands"<<endl<<endl;
	}

	return 0;
}