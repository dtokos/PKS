#include "ClientSocket.hpp"

ClientSocket::ClientSocket(int fileDescriptor) : Socket(fileDescriptor) {}

ClientSocket ClientSocket::connect(IP ip, Port port) {
	int fd;
	sockaddr_in address = sockaddr_in{
		.sin_family = AF_INET,
		.sin_addr = ip.getAddress(),
		.sin_port = port.getNormalizedNumber(),
	};
	
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw SocketConnectError("Could not get socket file descriptor");
	
	if (::connect(fd, (struct sockaddr *)&address, sizeof(address)) < 0)
		throw SocketConnectError("Could not connect to specified address");
	
	return ClientSocket(fd);
}
