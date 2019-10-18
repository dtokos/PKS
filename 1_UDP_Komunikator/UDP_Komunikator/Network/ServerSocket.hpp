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
	class SocketError : public exception {
	public:
		string error;
		
		SocketError() : error(strerror(errno)) {};
		virtual const char * what () const throw () = 0;
	};
	
	class SocketCreateError : public SocketError {
	private:
		string message;
		
	public:
		SocketCreateError(string message) : message(message) {}
		
		const char * what () const throw () {
			return message.c_str();
		}
	};
	
	class SocketOpenError : public SocketError {
	public:
		const char * what () const throw () {
			return "Could not open socket";
		}
	};
	
	class SocketAcceptError : public SocketError {
	public:
		const char * what () const throw () {
			return "Could not accept socket";
		}
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
