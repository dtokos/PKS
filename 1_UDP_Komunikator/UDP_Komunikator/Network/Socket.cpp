#include "Socket.hpp"

Socket::Socket(int fileDescriptor, sockaddr_in address) :
	fileDescriptor(fileDescriptor),
	address(address),
	addressLength(sizeof(address)) {}

void Socket::sendSegment(const Segment &segment) {
	::sendto(fileDescriptor, &segment, segment.length(), 0, (sockaddr *)&address, addressLength);
}

void Socket::receiveSegment() {
	::recvfrom(fileDescriptor, &receivedSegment, receivedSegment.maxLength(), MSG_WAITALL, (sockaddr *)&address, &addressLength);
}

bool Socket::receiveSegment(int timeout) {
	pollfd pollFD = pollfd {
		.fd = fileDescriptor,
		.events = POLLIN,
	};
	
	switch (poll(&pollFD, 1, timeout)) {
		case 0:
			return false;
			
		default:
			receiveSegment();
			return true;
	}
}

Socket::Segment::Type Socket::peakSegmentType() {
	pollfd pollFD = pollfd {
		.fd = fileDescriptor,
		.events = POLLIN,
	};
	
	poll(&pollFD, 1, 1000);
	
	::recvfrom(fileDescriptor, &peakBuffer, SegmentHeaderLength, MSG_PEEK, (sockaddr *)&address, &addressLength);
	
	return ((Segment *)peakBuffer)->type;
}
