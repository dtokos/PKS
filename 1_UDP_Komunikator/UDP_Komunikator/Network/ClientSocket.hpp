#ifndef ClientSocket_hpp
#define ClientSocket_hpp

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include "IP.hpp"
#include "Port.hpp"
#include "Socket.hpp"

using namespace std;

class ClientSocket : public Socket {
public:
	class SocketConnectError : public SocketError {
	public:
		SocketConnectError(string message) : SocketError(message) {}
	};
	
	static ClientSocket fromIPAndPort(IP ip, Port port);
	void connect();
	
private:
	ClientSocket(int fileDescriptor, sockaddr_in address);

	void sendSYN();
	void receiveSYNACK();
	void sendACK();
};

#endif
