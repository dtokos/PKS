#include "ServerApp.hpp"

void ServerApp::run() {
	Port port = getPort();
	char buffer[20];
	
	try {
		ServerSocket server = ServerSocket::fromPort(port);
		Socket s = server.accept();
		cout << "connected " << endl;
		while (true) {
			int len = s.read(buffer, 20);
			buffer[len] = '\0';
			cout << buffer << endl;
		}
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
