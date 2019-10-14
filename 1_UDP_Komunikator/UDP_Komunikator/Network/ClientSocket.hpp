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
	class SocketError : public exception {
	public:
		string error;
		
		SocketError() : error(strerror(errno)) {};
		virtual const char * what () const throw () = 0;
	};
	
	class SocketConnectError : public SocketError {
	private:
		string message;
		
	public:
		SocketConnectError(string message) : message(message) {};
		
		const char * what () const throw () {
			return message.c_str();;
		}
	};
	
	static ClientSocket connect(IP ip, Port port);
	
private:
	ClientSocket(int fileDescriptor);
};

#endif
