#include <iostream>
#include <ctime>
#include <string>
#include <fstream>
#include <json/json.h>
#include "authentication.h"
using namespace std;

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

int main() {

	/**** JSON Parser ***/	
	Json::Value users;
	Json::Reader reader;
	std::ifstream privatedb("private.json");
	bool parsingSuccessful = reader.parse(privatedb, users, false);
	if(!parsingSuccessful) {
		cout<<"\033[1;31mError Parsing Json File\033[0m\n\n";
		return 0;
	}
	privatedb.close();

	string id;

	while((id = authenticate(users))=="0"){
		continue;
	}
	users[id]["online"]="true";

	Json::StyledStreamWriter writer;
	std::ofstream update("private.json");
	writer.write(update,users);
	update.close();


	/*** List of Commands ***/
	cout<<"\033[1;35mType the command [type 'help' to see list of commands]:\033[0m ";
	string cmd;
	getline(cin,cmd);
	while(cmd!="exit"){
    if(cmd=="help") {cout<<"You can try the following commands:"<<endl;
					 cout<<"\033[1;32monline:\033[0m  gives list of online friends"<<endl;
					 cout<<"\033[1;32mfriends:\033[0m gives list of your friends"<<endl;
					 cout<<"\033[1;32mexit:\033[0m    quits the chat client"<<endl;
				     cout<<"\033[1;35mType the command [type 'help' to see list of commands]:\033[0m ";
					 getline(cin,cmd);
					}

	if(cmd=="online") {std::ifstream privatedb("private.json");
					  bool parsingSuccessful = reader.parse(privatedb, users, false);
					  if(!parsingSuccessful) {
					  	cout<<"\033[1;31mError Parsing Json File\033[0m\n\n";
					  	return 0;
					  }
					  privatedb.close();
					  for(Json::Value::iterator it = users.begin(); it!=users.end(); it++){
					  	if((*it)["online"]=="true") cout<<(*it)["name"]<<endl;
					  }
					  cout<<"\033[1;35mType the command [type 'help' to see list of commands]:\033[0m ";
					  getline(cin,cmd);
					  }

	if(cmd=="exit") {std::ifstream privatedb("private.json");
					 bool parsingSuccessful = reader.parse(privatedb, users, false);
					 if(!parsingSuccessful) {
					 	cout<<"\033[1;31mError Parsing Json File\033[0m\n\n";
					 	return 0;
					 }
					 privatedb.close();
					 
					 string time = formatted_time();
					 users[id]["last_seen"]=time;
					 users[id]["online"]="false";
					 std::ofstream update("private.json");
					 writer.write(update,users);
					 update.close();
					}
    }
	return 0; 	
}