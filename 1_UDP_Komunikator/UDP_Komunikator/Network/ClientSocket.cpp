#include "ClientSocket.hpp"

ClientSocket::ClientSocket(int fileDescriptor, sockaddr_in address, size_t maxSegmentSize) : Socket(fileDescriptor, address, maxSegmentSize) {}

ClientSocket ClientSocket::fromIPAndPort(IP ip, Port port, size_t maxSegmentSize) {
	int fd;
	sockaddr_in address = sockaddr_in{
		.sin_family = AF_INET,
		.sin_addr = ip.getAddress(),
		.sin_port = port.getNormalizedNumber(),
	};
	
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		throw SocketCreateError("Could not get socket file descriptor");
	
	return ClientSocket(fd, address, maxSegmentSize);
}
void ClientSocket::connect() {
	readingTimeout = InitialReadingTimeout;
	
	sendSYN();
	receiveSYNACK();
	sendACKSYN();
	setState(State::ESTABLISHED);
}

void ClientSocket::sendSYN() {
	sendSegment(Segment(Segment::Type::SYN));
}

void ClientSocket::receiveSYNACK() {
	int retries = 0;
	int receivedResult;
	
	while (retries++ < maxRetries) {
		receivedResult = receiveSegment(readingTimeout);
		if (receivedResult && receivedSegment.type() == Segment::Type::SYNACK)
			return;
		
		increaseReadingTimeout();
		sendSYN();
	}
	
	throw SocketConnectError("RDP handshake failed");
}

void ClientSocket::sendACKSYN() {
	int retries = 0;
	
	while (retries++ < maxRetries) {
		sendSegment(Segment(Segment::Type::ACKSYN));
		
		if (!receiveSegment(readingTimeout))
			return;

		increaseReadingTimeout();
	}
	
	throw SocketConnectError("RDP handshake failed");
}
