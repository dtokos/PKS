#include "ServerApp.hpp"

void ServerApp::run() {
	Port port = getPort();
	
	try {
		ServerSocket server = ServerSocket::fromPort(port);
		cout << "Waiting for connection" << endl;
		Socket client = server.accept();
		cout << "A client connected" << endl;
		
		comunicate(client);
		
		server.close();
	} catch (ServerSocket::SocketCreateError e) {
		cerr << "[APP][ERR] Could not create socket" << endl;
		cerr << "[RDP][ERR] " << e.what() << endl;
	} catch (ServerSocket::SocketAcceptError e) {
		cerr << "[APP][ERR] Could not accept socket " << endl;
		cerr << "[RDP][ERR] " << e.what() << endl;
	}
	
	cout << "Disconnected" << endl;
}

Port ServerApp::getPort() {
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
