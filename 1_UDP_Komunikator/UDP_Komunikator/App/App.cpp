#include "App.hpp"


void App::readFrom(Socket &socket) {
	char buffer[20];
	
	while (isOpen) {
		//cout << "-- Reading loop --\n";
		int len = socket.read(buffer, 20);
		if (len > 0) {
			buffer[len] = '\0';
			cout << "Received message: " << buffer << endl;
		}
	}
}

void App::printMenu() {
	cout << "[1] Send Message" << endl
	<< "[0] Exit" << endl;
}
