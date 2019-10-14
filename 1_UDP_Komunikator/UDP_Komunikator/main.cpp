#include <iostream>
#include "App/ServerApp.hpp"
#include "App/ClientApp.hpp"

using namespace std;

void printMenu();
char getChoice();

int main(int argc, const char * argv[]) {
	char choice;
	
	do {
		printMenu();
		choice = getChoice();
		
		switch (choice) {
			case '1':
				ServerApp().run();
				break;
				
			case '2':
				ClientApp().run();
				break;
		}
		
	} while (choice != '0');
	
	return 0;
}

void printMenu() {
	cout << "[1] Lauch server" << endl
		<< "[2] Launch client" << endl
		<< "[0] Exit" << endl;
}

char getChoice() {
	char choice;
	cout << "Your choice: ";
	cin >> choice;
	
	return choice;
}
