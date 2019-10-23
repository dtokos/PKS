#include "ClientApp.hpp"

ClientApp::ClientApp(string directory) : App(directory) {}
ClientApp::~ClientApp() {}

void ClientApp::run() {
	IP ip = getIP();
	Port port = getPort();
	size_t maxSegmentSize = getMaxSegmentSize();
	
	try {
		cout << "Connecting" << endl;
		ClientSocket socket = ClientSocket::fromIPAndPort(ip, port, maxSegmentSize);
		socket.connect();
		cout << "Connected" << endl;
		
		comunicate(socket);
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
		cin >> ip;
		
		try {
			return IP::fromString(ip);
		} catch (IP::InvalidIP e) {
			cerr << "[APP][ERR] IP " << e.ip << " is invalid" << endl;
		}
	}
}
