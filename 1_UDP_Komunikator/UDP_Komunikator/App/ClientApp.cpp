#include "ClientApp.hpp"

void ClientApp::run() {
	IP ip = getIP();
	Port port = getPort();
	char choice;
	string message;
	
	try {
		cout << "try to connect" << endl;
		ClientSocket socket = ClientSocket::fromIPAndPort(ip, port);
		socket.connect();
		cout << "connected" << endl;
		
		readingThread = thread(&ClientApp::readFrom, this, ref(socket));
		
		while (isOpen) {
			printMenu();
			cout << "Your choice: ";
			cin >> choice;
			cin.ignore(1000, '\n');
			
			switch (choice) {
				case '1':
					getline(cin, message);
					cout << "-- Start Write --\n";
					socket.write(message.c_str(), message.length());
					cout << "-- End Write --\n";
					break;
					
				case '0':
					isOpen = false;
					break;
			}
		}
		
		readingThread.join();
		
	} catch (ClientSocket::SocketCreateError e) {
		cerr << "[ERR] Could not connect to address" << endl;
		cerr << "[ERR] " << e.what() << endl;
	} catch (ClientSocket::SocketConnectError e) {
		cerr << "[ERR] Could not connect to address" << endl;
		cerr << "[ERR] " << e.what() << endl;
	} catch (ClientSocket::SocketWriteError e) {
		cerr << "[ERR] " << e.what() << endl;
	}
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

void ClientApp::readFrom(Socket &socket) {
	char buffer[20];
	
	while (isOpen) {
		cout << "-- Reading loop --\n";
		int len = socket.read(buffer, 20);
		buffer[len] = '\0';
		cout << buffer << endl;
	}
}

void ClientApp::printMenu() {
	cout << "[1] Send Message" << endl
	<< "[0] Exit" << endl;
}
