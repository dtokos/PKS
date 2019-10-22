#include <iostream>
#include <unistd.h>
#include <libgen.h>
#include "App/ServerApp.hpp"
#include "App/ClientApp.hpp"

using namespace std;

void printMenu();

int main(int argc, const char * argv[]) {
	string choice;
	
	while (true) {
		printMenu();
		getline(cin, choice);
		if (choice.length() != 1)
			continue;
		
		switch (choice[0]) {
			case '1':
				ServerApp().run();
				break;
				
			case '2':
				ClientApp().run();
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
