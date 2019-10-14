#include "ServerSocket.hpp"

ServerSocket::ServerSocket(int fileDescriptor) : fileDescriptor(fileDescriptor) {}

ServerSocket ServerSocket::fromPort(Port port) {
	int fd, options = 1;
	sockaddr_in address = sockaddr_in {
		.sin_family = AF_INET,
		.sin_addr.s_addr = INADDR_ANY,
		.sin_port = port.getNormalizedNumber(),
	};
	
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		throw SocketCreateError("Could not get socket file descriptor");
	
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &options, sizeof(options)))
		throw SocketCreateError("Could not set options to socket");
	
	if (::bind(fd, (struct sockaddr *)&address, sizeof(address)) < 0)
		throw SocketCreateError("Could not bind socket to address");
	
	return ServerSocket(fd);
}

void ServerSocket::open() {
	if (::listen(fileDescriptor, 1) < 0)
		throw SocketOpenError();
}

Socket ServerSocket::accept() {
	int clientFD, addressLength = sizeof(sockaddr);
	sockaddr address;
	
	if ((clientFD = ::accept(fileDescriptor, (struct sockaddr *)&address, (socklen_t*)&addressLength)) < 0)
		throw SocketAcceptError();
	
	return Socket(clientFD);
}

void ServerSocket::close() {
	::close(fileDescriptor);
}
