#include <iostream>
#include <unistd.h>
#include <libgen.h>
#include <mach-o/dyld.h>
#include "App/ServerApp.hpp"
#include "App/ClientApp.hpp"

using namespace std;

void printMenu();
string getDirname();

int main() {
	string choice;
	
	while (true) {
		printMenu();
		getline(cin, choice);
		if (choice.length() != 1)
			continue;
		
		switch (choice[0]) {
			case '1':
				ServerApp(getDirname()).run();
				break;
				
			case '2':
				ClientApp(getDirname()).run();
				break;
				
			case '0':
				return 0;
		}
	}
}

void printMenu() {
	cout << "### Main Menu ###" << endl
		<< "[1] Lauch server" << endl
		<< "[2] Launch client" << endl
		<< "[0] Exit" << endl
		<< "Your choice: ";
	cout.flush();
}

string getDirname() {
	char path[PATH_MAX];
	uint32_t pathSize = 1024;
	
	if (_NSGetExecutablePath(path, &pathSize) == -1) {
		cout << "[ERR][APP] Could not get executable path" << endl;
		exit(EXIT_FAILURE);
	}
	
	char realPath[PATH_MAX];
	
	if (realpath(path, realPath) == NULL) {
		cout << "[ERR][APP] Could real path of executable" << endl;
		exit(EXIT_FAILURE);
	}
	
	string dirPath(dirname(realPath));
	dirPath += '/';
	
	return dirPath;
}
