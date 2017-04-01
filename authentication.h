#include <iostream>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <json/json.h>
using namespace std;
/**
	Takes a char input without echoing
	@return the int value of char
*/
int getch();

/**
	Returns input from terminal without echoing
	@return The masked input
*/
string getMaskedInput();

/**
	Authenticates the user
	@return The id of user
*/
string authenticate(Json::Value users);