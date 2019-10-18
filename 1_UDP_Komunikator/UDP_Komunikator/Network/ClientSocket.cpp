#include "ClientSocket.hpp"

ClientSocket::ClientSocket(int fileDescriptor, sockaddr_in address) : Socket(fileDescriptor, address) {}

ClientSocket ClientSocket::fromIPAndPort(IP ip, Port port) {
	int fd;
	sockaddr_in address = sockaddr_in{
		.sin_family = AF_INET,
		.sin_addr = ip.getAddress(),
		.sin_port = port.getNormalizedNumber(),
	};
	
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		throw SocketConnectError("Could not get socket file descriptor");
	
	return ClientSocket(fd, address);
}
void ClientSocket::connect() {
	cout << "-- Start Handshake --\n";
	sendSYN();
	receiveSYNACK();
	sendACK();
	cout << "-- Handshake successfull --\n";
}

void ClientSocket::sendSYN() {
	cout << "-- Start send SYN --\n";
	sendSegment(Segment::makeSYN());
	cout << "-- End send SYN --\n";
}

void ClientSocket::receiveSYNACK() {
	cout << "-- Start receive SYN_ACK --\n";
	int retries = 0;
	int timeout = 1;
	int receivedResult;
	
	while (retries++ < 10) {
		cout << "Attempt: " << retries << endl;
		receivedResult = receiveSegment(timeout);
		
		if (receivedResult == 0 || receivedSegment.type != Segment::Type::SYNACK) {
			cout << "-- Timeout or wrong segment --\n";
			sendSYN();
		} else {
			cout << "-- End receive SYN_ACK --\n";
			return;
		}
		
		timeout *= 2;
	}
	
	cout << "-- FAILED receive SYN_ACK --\n";
	throw Socket::SocketError("RDP handshake failed");
}

void ClientSocket::sendACK() {
	cout << "-- Start send ACK --\n";
	int retries = 0;
	
	while (retries++ < 10) {
		sendSegment(Segment::makeACK());
		
		if (!receiveSegment(5)) {
			cout << "-- ACK successfull --\n";
			return;
		}
	}
	
	cout << "-- Failed send ACK --\n";
	throw Socket::SocketError("RDP handshake failed");
}
