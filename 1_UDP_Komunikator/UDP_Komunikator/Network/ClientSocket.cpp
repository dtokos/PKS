#include "ClientSocket.hpp"

#define maxRetries 10

ClientSocket::ClientSocket(int fileDescriptor, sockaddr_in address) : Socket(fileDescriptor, address) {}

ClientSocket ClientSocket::fromIPAndPort(IP ip, Port port) {
	int fd;
	sockaddr_in address = sockaddr_in{
		.sin_family = AF_INET,
		.sin_addr = ip.getAddress(),
		.sin_port = port.getNormalizedNumber(),
	};
	
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		throw SocketCreateError("Could not get socket file descriptor");
	
	return ClientSocket(fd, address);
}
void ClientSocket::connect() {
	readingTimeout = 1;
	
	sendSYN();
	receiveSYNACK();
	sendACK();

}

void ClientSocket::sendSYN() {
	sendSegment(Segment::makeSYN());
}

void ClientSocket::receiveSYNACK() {
	int retries = 0;
	int receivedResult;
	
	while (retries++ < maxRetries) {
		receivedResult = receiveSegment(readingTimeout);
		if (receivedResult && receivedSegment.type() == Segment::Type::SYNACK)
			return;
		
		readingTimeout *= 2;
		sendSYN();
	}
	
	throw SocketConnectError("RDP handshake failed");
}

void ClientSocket::sendACK() {
	int retries = 0;
	
	while (retries++ < maxRetries) {
		sendSegment(Segment::makeACK());
		
		if (!receiveSegment(readingTimeout))
			return;

		readingTimeout += 2;
	}
	
	throw SocketConnectError("RDP handshake failed");
}
