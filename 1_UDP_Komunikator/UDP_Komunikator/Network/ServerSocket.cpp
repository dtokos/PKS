#include "ServerSocket.hpp"

ServerSocket::ServerSocket(int fileDescriptor, sockaddr_in address) : Socket(fileDescriptor, address) {}

ServerSocket ServerSocket::fromPort(Port port) {
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
	
	return ServerSocket(fd, address);
}

Socket ServerSocket::accept() {
	cout << "-- Start Handshake --\n";
	receiveSYN();
	sendSYNACK();
	receiveACK();
	cout << "-- Handshake successfull --\n";
	
	return Socket(fileDescriptor, address);
}

void ServerSocket::receiveSYN() {
	cout << "-- Start receive SYN --\n";
	while (true) {
		receiveSegment();
		
		if (receivedSegment.type == Segment::Type::SYN) {
			cout << "-- End receive SYN --\n";
			break;
		}
		
		cout << "-- Not SYN segment --\n";
	}
}

void ServerSocket::sendSYNACK() {
	cout << "-- Start send SYN_ACK --\n";
	sendSegment(Segment::makeSYNACK());
	cout << "-- End send SYN_ACK --\n";
}

void ServerSocket::receiveACK() {
	int retries = 0;
	int timeout = 1;
	bool receiveResult;
	cout << "-- Start receive ACK --\n";
	while (retries++ < 10) {
		receiveResult = receiveSegment(timeout);
		
		if (!receiveResult || receivedSegment.type != Segment::Type::ACK) {
			cout << "-- Timeout or wrong segment --\n";
			sendSYNACK();
		} else {
			cout << "-- End receive ACK --\n";
			return;
		}
	}
	
	cout << "-- FAILED receive ACK --\n";
	throw Socket::SocketError("RDP handshake failed");
}

void ServerSocket::close() {
	::close(fileDescriptor);
}
