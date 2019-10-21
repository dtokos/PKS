#include "ClientApp.hpp"

void ClientApp::run() {
	IP ip = getIP();
	Port port = getPort();
	char choice;
	string message;
	
	try {
		cout << "Connecting" << endl;
		ClientSocket socket = ClientSocket::fromIPAndPort(ip, port);
		socket.connect();
		cout << "Connectd" << endl;
		
		readingThread = thread(&ClientApp::readFrom, this, ref(socket));
		
		while (isOpen) {
			printMenu();
			cout << "Your choice: ";
			cin >> choice;
			cin.ignore(1000, '\n');
			
			switch (choice) {
				case '1':
					cout << "Enter your message: ";
					getline(cin, message);
					socket.write(message.c_str(), message.length());
					break;
					
				case '0':
					isOpen = false;
					break;
			}
		}
		
		socket.disconnect();
	} catch (ClientSocket::SocketCreateError e) {
		cerr << "[ERR] Could not connect to address" << endl;
		cerr << "[ERR] " << e.what() << endl;
	} catch (ClientSocket::SocketConnectError e) {
		cerr << "[ERR] Could not connect to address" << endl;
		cerr << "[ERR] " << e.what() << endl;
	} catch (ClientSocket::SocketWriteError e) {
		cerr << "[ERR] " << e.what() << endl;
	}
	
	isOpen = false;
	readingThread.join();
}

IP ClientApp::getIP() {
	string ip;
	
	while (true) {
		cout << "Enter IPv4 address: ";
		//cin >> ip;
		ip = "127.0.0.1";
		
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
		//cin >> portNumber;
		portNumber = 9001;
		
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
