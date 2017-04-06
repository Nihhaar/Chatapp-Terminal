#include "utils.h"

/* Split a string using delimitter */
vector<string> split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

string formatted_time(){

	time_t t = time(0);
	struct tm * now = localtime( & t );
	string time = to_string((now->tm_year + 1900)) + "-";
	if((now->tm_mon + 1)<10) time += "0" + to_string((now->tm_mon + 1)) + "-";
	else time+=to_string((now->tm_mon + 1)) + "-";

	if(now->tm_mday<10) time += "0" + to_string(now->tm_mday) + " ";
	else time+=to_string(now->tm_mday) + " ";

	if(now->tm_hour<10) time += "0" + to_string((now->tm_hour)) + ":";
	else time+=to_string((now->tm_hour)) + ":";

	if(now->tm_min<10) time += "0" + to_string((now->tm_min));
	else time+=to_string((now->tm_min));

	return time;
}

void *threadHandler(void* threadargs)
{
	int clientSock;

	pthread_detach(pthread_self());

	clientSock = ((struct ThreadArgs *)threadargs)->clientSock;
	free(threadargs);

	handleTCPClient (clientSock);

	return (NULL);
}

void readXBytes(int socket, unsigned int x, void* buffer)
{
    unsigned int bytesRead = 0;
    int result;
    while (bytesRead < x)
    {
        result = read(socket, (uint8_t*)buffer + bytesRead, x - bytesRead);
        if (result < 1 )
        {
            cout<<"Error: read() failed"<<endl;
            return;
        }

        bytesRead += result;
    }
}

void sendDataToClient(string str, int sock){

  unsigned int length = htonl(str.size());

  if(send(sock, &length, sizeof(length), 0) != sizeof(length)){
    cout<<"Error: Sent different number of bytes than expected"<<endl;
    return;
  }

  if(send(sock, str.c_str(), str.size(), 0) != (int)str.size()){
    cout<<"Error: Sent different number of bytes than expected"<<endl;
    return;
  }

  return;
}

void handleTCPClient(int clientSocket){
	
  /* READ COMMANDS */

  unsigned int length = 0;
  char* buffer = 0;
  Json::Value users;
  Json::Reader reader;

  while(true){

  readXBytes(clientSocket, sizeof(length), (void*)(&length));
  length = ntohl(length);
  buffer = new char[length];
  readXBytes(clientSocket, length, (void*)buffer);

  std::ifstream privatedb("private.json");
		bool parsingSuccessful = reader.parse(privatedb, users, false);
		if(!parsingSuccessful) {
		  	cout<<"\033[1;31mError Parsing Json File\033[0m\n\n";
		  	cout<<reader.getFormattedErrorMessages()<<endl;
		  	return;
		}
		
  privatedb.close();

  vector<string> v = split(buffer,' ');

  if(v[0] == "AUTHENTICATE")
  {
  	if(v.size()==3){

		string id = "0";

		for(Json::Value::iterator it = users.begin(); it!=users.end(); it++){
			if((*it).get("name","0") == v[1]){
				if((*it).get("password","0") == v[2]){
					id = it.key().asString();
					break;
				}
			}
	    }
		
		if(id!="0"){
			users[id]["online"]="true";
		
			Json::StyledStreamWriter writer;
			std::ofstream update("private.json");
			writer.write(update,users);
			update.close();
		}

		sendDataToClient(id, clientSocket);
  	}
  }

  if(v[0] == "ONLINE"){
  		string id = v[1];
  		string msg;
		for(Json::Value::iterator it = users[id]["friends"].begin(); it!=users[id]["friends"].end(); it++){
		  	string id1 = (*it)["id"].asString();
		  	if(users[id1]["online"]=="true") msg += users[id1]["name"].asString() + "\n";
		}

		sendDataToClient(msg, clientSocket);
  }

  if(v[0] == "FRIENDS"){
  		string id = v[1];
  		string msg;
  		
		for(Json::Value::iterator it = users[id]["friends"].begin(); it!=users[id]["friends"].end(); it++){
		  	msg += users[(*it)["id"].asString()]["name"].asString() + "\n";
		}

		sendDataToClient(msg, clientSocket);
  }

  if(v[0] == "LAST_SEEN_ALL"){
  		string id = v[1];
  		string msg;

		for(Json::Value::iterator it = users[id]["friends"].begin(); it!=users[id]["friends"].end(); it++){
		    string id1 = (*it)["id"].asString();
		    if(users[id1]["online"]=="true")  msg += users[id1]["name"].asString() + ": \033[1;32mActive now\033[0m\n";
		    else msg += users[id1]["name"].asString() + ": \033[1;32mLast seen at \033[0m" + users[id1]["last_seen"].asString() + "\n";
		}

		sendDataToClient(msg, clientSocket);
  }

  if(v[0] == "LAST_SEEN"){
  		string id = v[2];
  		string msg;
		bool flag=1;
		for(Json::Value::iterator it = users[id]["friends"].begin(); it!=users[id]["friends"].end(); it++){
		  	string id1 = (*it)["id"].asString();
		  	if(users[id1]["online"]=="true" && users[id1]["name"]==v[1]) {msg += v[1] + " :\033[1;32mActive now\033[0m\n";flag=0;}
		  	else if(users[id1]["name"]==v[1]) {msg += v[1] + " :\033[1;32mLast seen at \033[0m" + users[id1]["last_seen"].asString() + "\n";flag=0;}
		}
		
		if(flag) msg += "\033[1;31mHe/She is not your friend\033[0m\n";

		sendDataToClient(msg, clientSocket);
  }

  if(v[0] == "EXIT"){
  		string id = v[1];
  		string time = formatted_time();
		users[id]["last_seen"]=time;
		users[id]["online"]="false";
		std::ofstream update("private.json");
		Json::StyledStreamWriter writer;
		writer.write(update,users);
		update.close();
  }
}

  return;

}