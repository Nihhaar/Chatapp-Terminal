#include "authentication.h"

/**
	Takes a char input without echoing
	@return the int value of char
*/
int getch() {
    int ch;
    struct termios t_old, t_new;

    tcgetattr(STDIN_FILENO, &t_old);
    t_new = t_old;
    t_new.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t_new);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &t_old);
    return ch;
}

/**
	Returns input from terminal without echoing
	@return The masked input
*/
string getMaskedInput(){
	const char BACKSPACE=127;
  	const char RETURN=10;
  	string inp;
  	char ch=0;

  	while((ch=getch()) != RETURN){
  		if(ch == BACKSPACE){
  			if(inp.length()!=0){
  				inp.resize(inp.length()-1);
  			}
  		}
  		else{
  			inp+=ch;
  		}
  	}
  	cout << endl;
  	return inp;
}


string authenticate(Json::Value users) {
	string email, pwd;
	cout << "Enter your registered email: ";
	getline(cin,email);
	cout << "Enter the password: ";
	pwd = getMaskedInput();

	for(Json::Value::iterator it = users.begin(); it!=users.end(); it++){
		if((*it).get("email","0")==email){
			if((*it).get("password","0")==pwd){
				cout<<"\033[1;32mUser Authenticated\033[0m\n\n";
				return it.key().asString();
			}

			else{
				cout<<"\033[1;31mWrong Password\033[0m\n\n";
				return "0";
			}
		}
	}

	cout<<"\033[1;31mUser not found\033[0m\n\n";
	return "0";
}