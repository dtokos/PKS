#include "ServerApp.hpp"

void ServerApp::run() {
	Port port = getPort();
	char buffer[1024];
	char inputAction = 0;
	string message;
	char *msgPtr;
	int messageLength;
	
	cout << "we have port " << port.getNumber() << endl;
	try {
		cout << "creating server "<< endl;
		ServerSocket server = ServerSocket::fromPort(port);
		cout << "accepting server "<< endl;
		Socket s = server.accept();
		cout << "accepted " << endl;
		//cout << "reading " << fd << endl;
		
		
		
		
		cout << "done " << endl;
		cout << "closing server" << endl;
		server.close();
		cout << "end" << endl;
	} catch (ServerSocket::SocketCreateError e) {
		cerr << "[ERR] Could not create socket" << endl;
		cerr << "[ERR] " << e.what() << endl;
		cerr << "[ERR] " << e.error << endl;
	} catch (ServerSocket::SocketOpenError e) {
		cerr << "[ERR] Could not open socket" << e.what() << endl;
		cerr << "[ERR] " << e.what() << endl;
		cerr << "[ERR] " << e.error << endl;
	} catch (ServerSocket::SocketAcceptError e) {
		cerr << "[ERR] Could not accept socket " << endl;
		cerr << "[ERR] " << e.what() << endl;
		cerr << "[ERR] " << e.error << endl;
	}
	
	cout << "Listen" << endl;
	// listen
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
