#include "ServerSocket.hpp"

ServerSocket::ServerSocket(int fileDescriptor, sockaddr_in address,  size_t maxSegmentSize) : Socket(fileDescriptor, address, maxSegmentSize) {}

ServerSocket ServerSocket::fromPort(Port port, size_t maxSegmentSize) {
	int fd, options = 1;
	sockaddr_in address = sockaddr_in {
		.sin_family = AF_INET,
		.sin_addr.s_addr = INADDR_ANY,
		.sin_port = port.getNormalizedNumber(),
	};
	
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == 0)
		throw SocketCreateError("Could not get socket file descriptor");
	
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &options, sizeof(options)))
		throw SocketCreateError("Could not set options to socket");
	
	if (::bind(fd, (struct sockaddr *)&address, sizeof(address)) < 0)
		throw SocketCreateError("Could not bind socket to address");
	
	return ServerSocket(fd, address, maxSegmentSize);
}

Socket ServerSocket::accept() {
	readingTimeout = InitialReadingTimeout;
	
	receiveSYN();
	sendSYNACK();
	receiveACKSYN();
	
	return Socket(fileDescriptor, address, maxSegmentSize, readingTimeout, State::ESTABLISHED);
}

void ServerSocket::receiveSYN() {
	while (true) {
		receiveSegment();
		if (receivedSegment.type() == Segment::Type::SYN)
			return;
	}
}

void ServerSocket::sendSYNACK() {
	sendSegment(Segment(Segment::Type::SYNACK));
}

void ServerSocket::receiveACKSYN() {
	int retries = 0;
	bool receiveResult;
	
	while (retries++ < maxRetries) {
		receiveResult = receiveSegment(readingTimeout);
		
		if (receiveResult && receivedSegment.type() == Segment::Type::ACKSYN)
			return;
		
		increaseReadingTimeout();
		sendSYNACK();
	}
	
	throw SocketAcceptError("RDP handshake failed");
}
