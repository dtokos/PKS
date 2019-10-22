#include "ClientApp.hpp"

void ClientApp::run() {
	IP ip = getIP();
	Port port = getPort();
	
	try {
		cout << "Connecting" << endl;
		ClientSocket socket = ClientSocket::fromIPAndPort(ip, port);
		socket.connect();
		cout << "Connectd" << endl;
		
		comunicate(socket);
		
		socket.disconnect();
	} catch (ClientSocket::SocketCreateError e) {
		cerr << "[APP][ERR] Could not connect to address" << endl;
		cerr << "[RDP][ERR] " << e.what() << endl;
	} catch (ClientSocket::SocketConnectError e) {
		cerr << "[APP][ERR] Could not connect to address" << endl;
		cerr << "[RDP][ERR] " << e.what() << endl;
	}
	
	cout << "Disconnected" << endl;
}

IP ClientApp::getIP() {
	string ip;
	
	while (true) {
		cout << "Enter IPv4 address: ";
		cout.flush();
		//cin >> ip;
		ip = "127.0.0.1";
		
		try {
			return IP::fromString(ip);
		} catch (IP::InvalidIP e) {
			cerr << "[APP][ERR] IP " << e.ip << " is invalid" << endl;
		}
	}
}

Port ClientApp::getPort() {
	int portNumber;
	
	while (true) {
		cout << "Enter port(1024 - 65535): ";
		cout.flush();
		//cin >> portNumber;
		portNumber = 9001;
		
		try {
			return Port::fromNumber(portNumber);
		} catch (Port::PortOutOfRange e) {
			cerr << "[APP][ERR] Port " << e.portNumber << " is invalid" << endl;
			
			if (cin.fail()) {
				cin.clear();
				cin.ignore(1000, '\n');
			}
		}
	}
}
