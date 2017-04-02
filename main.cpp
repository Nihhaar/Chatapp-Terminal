#include <iostream>
#include <ctime>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <json/json.h>
#include "authentication.h"
using namespace std;

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
    if(cmd=="help") {cout<<"You can try the following commands:"<<endl;
					 cout<<"\033[1;32monline:\033[0m  gives list of online friends"<<endl;
					 cout<<"\033[1;32mfriends:\033[0m gives list of your friends"<<endl;
					 cout<<"\033[1;32mlast_seen \033[3m[\033[1;35musername\033[1;32m]:\033[0m Gives last seen time \033[3m[\033[1;35mof your friend\033[1;0m]\033[0m"<<endl;
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
					  for(Json::Value::iterator it = users[id]["friends"].begin(); it!=users[id]["friends"].end(); it++){
					  	string id1 = (*it)["id"].asString();
					  	if(users[id1]["online"]=="true") cout<<users[id1]["name"]<<endl;
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
					 return 0;
					}

	if(cmd=="friends") {
			std::ifstream privatedb("private.json");
			bool parsingSuccessful = reader.parse(privatedb, users, false);
			if(!parsingSuccessful) {
				cout<<"\033[1;31mError Parsing Json File\033[0m\n\n";
				return 0;
			}
			privatedb.close();

			for(Json::Value::iterator it = users[id]["friends"].begin(); it!=users[id]["friends"].end(); it++){
			  cout<<users[(*it)["id"].asString()]["name"]<<endl;
			}

			cout<<"\033[1;35mType the command [type 'help' to see list of commands]:\033[0m ";
			getline(cin,cmd);
	}

	vector<string> v = split(cmd,' ');
	if(v[0]=="last_seen"){
		    std::ifstream privatedb("private.json");
			bool parsingSuccessful = reader.parse(privatedb, users, false);
			if(!parsingSuccessful) {
				cout<<"\033[1;31mError Parsing Json File\033[0m\n\n";
				return 0;
			}
			privatedb.close();

			if(v.size()==2){
			bool flag=1;
			for(Json::Value::iterator it = users[id]["friends"].begin(); it!=users[id]["friends"].end(); it++){
			  	string id1 = (*it)["id"].asString();
			  	if(users[id1]["online"]=="true" && users[id1]["name"]==v[1]) {cout<<"\033[1;32mActive now\033[0m"<<endl;flag=0;}
			  	else if(users[id1]["name"]==v[1]) {cout<<"\033[1;32mLast seen at \033[0m"<<users[id1]["last_seen"]<<endl;flag=0;}
			}
			if(flag) cout<<"\033[1;31mHe/She is not your friend\033[0m"<<endl;
			}

			else{
			for(Json::Value::iterator it = users[id]["friends"].begin(); it!=users[id]["friends"].end(); it++){
			  string id1 = (*it)["id"].asString();
			  if(users[id1]["online"]=="true")  cout<<users[id1]["name"]<<": \033[1;32mActive now\033[0m"<<endl;
			  else cout<<users[id1]["name"]<<": \033[1;32mLast seen at \033[0m"<<users[id1]["last_seen"]<<endl;
			}
			}

			cout<<"\033[1;35mType the command [type 'help' to see list of commands]:\033[0m ";
			getline(cin,cmd);

	}
	return 0; 	
}