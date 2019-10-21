#include "ServerApp.hpp"

void ServerApp::run() {
	Port port = getPort();
	char choice;
	string message;
	
	try {
		ServerSocket server = ServerSocket::fromPort(port);
		cout << "Waiting for connection" << endl;
		Socket s = server.accept();
		cout << "A client connected" << endl;
		
		readingThread = thread(&ServerApp::readFrom, this, ref(s));
		
		while (isOpen) {
			printMenu();
			cout << "Your choice: ";
			cin >> choice;
			cin.ignore(1000, '\n');
			
			switch (choice) {
				case '1':
					cout << "Enter your message: ";
					getline(cin, message);
					s.write(message.c_str(), message.length());
					break;
					
				case '0':
					isOpen = false;
					break;
			}
		}
		
		s.disconnect();
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
