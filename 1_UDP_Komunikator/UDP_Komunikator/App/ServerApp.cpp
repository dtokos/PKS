#include "ServerApp.hpp"

void ServerApp::run() {
	Port port = getPort();
	char buffer[1024];
	int valread;
	
	cout << "we have port " << port.getNumber() << endl;
	try {
		cout << "creating server "<< endl;
		ServerSocket server = ServerSocket::fromPort(port);
		cout << "opening server "<< endl;
		server.open();
		cout << "accepting server "<< endl;
		int fd = server.accept();
		cout << "accepted " << fd << endl;
		//cout << "reading " << fd << endl;
		while (valread = read(fd, buffer, 1024)) {
			cout << "got " << valread << " " << buffer << endl;
			cout << "replying " << fd << endl;
			send(fd, buffer, valread, 0);
		}
		cout << "done " << fd << endl;
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
