#ifndef ServerSocket_hpp
#define ServerSocket_hpp

#include <iostream>
#include <exception>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include "Port.hpp"
#include "Socket.hpp"

class ServerSocket : public Socket {
public:
	class SocketAcceptError : public SocketError {
	public:
		SocketAcceptError(string message) : SocketError(message) {}
	};
	
	static ServerSocket fromPort(Port port);
	Socket accept();
	void close();
	
private:
	ServerSocket(int fileDescriptor, sockaddr_in address);
	void receiveSYN();
	void sendSYNACK();
	void receiveACK();
};

#endif
