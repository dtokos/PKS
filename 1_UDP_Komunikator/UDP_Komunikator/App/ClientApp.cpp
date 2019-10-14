#include "ClientApp.hpp"

void ClientApp::run() {
	IP ip = getIP();
	Port port = getPort();
	string msg;
	char buff[1024];
	try {
		cout << "try to connect" << endl;
		ClientSocket socket = ClientSocket::connect(ip, port);
		void *asd = (void *)msg.c_str();
		while (cin >> msg) {
			socket.write(asd, msg.length());
			socket.read(buff, 1024);
			cout << "Received message: " << buff << endl;
		}
		
		cout << "connected" << endl;
	} catch (ClientSocket::SocketConnectError e) {
		cerr << "[ERR] Could not connect to address" << endl;
		cerr << "[ERR] " << e.what() << endl;
		cerr << "[ERR] " << e.error << endl;
	}
}

IP ClientApp::getIP() {
	string ip;
	
	while (true) {
		cout << "Enter IPv4 address: ";
		cin >> ip;
		
		try {
			return IP::fromString(ip);
		} catch (IP::InvalidIP e) {
			cerr << "[ERR] IP " << e.ip << " is invalid" << endl;
		}
	}
}

Port ClientApp::getPort() {
	int portNumber;
	
	while (true) {
		cout << "Enter port(1024 - 65535): ";
		cin >> portNumber;
		
		try {
			return Port::fromNumber(portNumber);
		} catch (Port::PortOutOfRange e) {
			cerr << "[ERR] Port " << e.portNumber << " is invalid" << endl;
			
			if (cin.fail()) {
				cin.clear();
				cin.ignore(1000, '\n');
			}
		}
	}
}
