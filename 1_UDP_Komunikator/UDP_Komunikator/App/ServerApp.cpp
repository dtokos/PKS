#include "ServerApp.hpp"

void ServerApp::run() {
	Port port = getPort();
	char choice;
	string message;
	
	try {
		ServerSocket server = ServerSocket::fromPort(port);
		cout << "accepting" << endl;
		Socket s = server.accept();
		cout << "connected" << endl;
		
		readingThread = thread(&ServerApp::readFrom, this, ref(s));
		
		while (isOpen) {
			printMenu();
			cout << "Your choice: ";
			cin >> choice;
			cin.ignore(1000, '\n');
			
			switch (choice) {
				case '1':
					getline(cin, message);
					cout << "-- Start Write --\n";
					s.write(message.c_str(), message.length());
					cout << "-- End Write --\n";
					break;
					
				case '0':
					isOpen = false;
					break;
			}
		}
		
		readingThread.join();
		
		server.close();
		cout << "end" << endl;
	} catch (ServerSocket::SocketCreateError e) {
		cerr << "[ERR] Could not create socket" << endl;
		cerr << "[ERR] " << e.what() << endl;
	} catch (ServerSocket::SocketAcceptError e) {
		cerr << "[ERR] Could not accept socket " << endl;
		cerr << "[ERR] " << e.what() << endl;
	}
}

Port ServerApp::getPort() {
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

void ServerApp::readFrom(Socket &socket) {
	char buffer[20];
	
	while (isOpen) {
		cout << "-- Reading loop --\n";
		int len = socket.read(buffer, 20);
		buffer[len] = '\0';
		cout << buffer << endl;
	}
}

void ServerApp::printMenu() {
	cout << "[1] Send Message" << endl
		<< "[0] Exit" << endl;
}
