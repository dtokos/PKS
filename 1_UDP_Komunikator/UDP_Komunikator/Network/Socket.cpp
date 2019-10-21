#include "Socket.hpp"

Socket::Socket(int fileDescriptor, sockaddr_in address, int readingTimeout) :
	fileDescriptor(fileDescriptor),
	address(address),
	addressLength(sizeof(address)),
	peakedSegment((Segment *)peakBuffer),
	readingTimeout(readingTimeout) {}

void Socket::write(const void *data, size_t length) {
	Segment s;
	s.setType(Segment::Type::DATA);
	char *d = (char *)data;
	int retries = 0;
	
	while (length > 0) {
		s.setSequenceNumber(leastNotAcknowledged);
		s.setData(d, min(Segment::MaxDataLength, length));
		s.setChecksum();
		length -= s.dataLength();
		
		while (++retries < 10) {
			cout << retries << endl;
			sendSegment(s);
			receiveSegment(readingTimeout);
			
			if (receivedSegment.type() == Segment::Type::ACK && receivedSegment.acceptanceNumber() == leastNotAcknowledged) {
				leastNotAcknowledged++;
				break;
			}
			
			increaseReadingTimeout();
		}
		cout << retries << " == 10" << endl;
		if (retries == 10)
			throw SocketWriteError("Could not send data");
		
		d += s.dataLength();
		s.setSequenceNumber(leastNotAcknowledged);
	}
}

int Socket::read(void *buffer, int length) {
	if (readBuffer.hasData())
		return readBuffer.read((char *)buffer, length);
	
	while (true) {
		receiveSegment();
		
		if (receivedSegment.type() == Segment::Type::DATA && lastAcknowledged + 1 == receivedSegment.sequenceNumber()) {
			sendSegment(Segment(receivedSegment.sequenceNumber()));
			lastAcknowledged++;
			readBuffer.add(receivedSegment);
			break;
		} else if (receivedSegment.type() == Segment::Type::DATA && lastAcknowledged == receivedSegment.sequenceNumber()) {
			sendSegment(Segment(receivedSegment.sequenceNumber()));
		}
	}
	
	
	
	return readBuffer.read((char *)buffer, length);
}

void Socket::sendSegment(const Segment &segment) {
	::sendto(fileDescriptor, (const void *)&segment, segment.length(), 0, (sockaddr *)&address, addressLength);
}

void Socket::receiveSegment() {
	do {
		::recvfrom(fileDescriptor, &receivedSegment, Segment::MaxLength, MSG_WAITALL, (sockaddr *)&address, &addressLength);
	} while (!receivedSegment.isValid());
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

bool Socket::peakSegment(int timeout) {
	pollfd pollFD = pollfd {
		.fd = fileDescriptor,
		.events = POLLIN,
	};
	
	switch (poll(&pollFD, 1, timeout)) {
		case 0:
			return false;
			
		default:
			::recvfrom(fileDescriptor, &peakBuffer, Segment::HeaderLength, MSG_PEEK, (sockaddr *)&address, &addressLength);
			return true;
	}
}

void Socket::increaseReadingTimeout() {
	if (readingTimeout < 5000)
		readingTimeout *= 2;
	else
		readingTimeout += readingTimeout / 2;
}
