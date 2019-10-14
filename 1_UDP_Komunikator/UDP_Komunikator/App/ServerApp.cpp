#include "ServerApp.hpp"

void ServerApp::run() {
	Port port = getPort();
	cout << "we have port " << port.getNumber() << endl;
	cout << "Listen" << endl;
	// listen
}

Port ServerApp::getPort() {
	int portNumber;
	
	while (true) {
		cout << "Enter port(1024 - 65535): ";
		cin >> portNumber;
		
		try {
			return Port::fromNumber(portNumber);
		} catch (Port::PortOutOfRange e) {
			cout << "Port " << e.portNumber << " is invalid" << endl;
			
			if (cin.fail()) {
				cin.clear();
				cin.ignore(1000, '\n');
			}
		}
	}
}
