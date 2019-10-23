#include "ServerApp.hpp"

ServerApp::ServerApp(string directory) : App(directory) {}

void ServerApp::run() {
	Port port = getPort();
	size_t maxSegmentSize = getMaxSegmentSize();
	
	try {
		ServerSocket server = ServerSocket::fromPort(port, maxSegmentSize);
		cout << "Waiting for connection" << endl;
		Socket client = server.accept();
		cout << "A client connected" << endl;
		
		comunicate(client);
	} catch (ServerSocket::SocketCreateError e) {
		cerr << "[APP][ERR] Could not create socket" << endl;
		cerr << "[RDP][ERR] " << e.what() << endl;
	} catch (ServerSocket::SocketAcceptError e) {
		cerr << "[APP][ERR] Could not accept socket " << endl;
		cerr << "[RDP][ERR] " << e.what() << endl;
	}
	
	cout << "Disconnected" << endl;
}
