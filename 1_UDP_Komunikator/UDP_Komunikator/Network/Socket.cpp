#include "Socket.hpp"

Socket::Socket(int fileDescriptor, sockaddr_in address) :
	fileDescriptor(fileDescriptor),
	address(address),
	addressLength(sizeof(address)) {}

void Socket::write(const void *data, size_t length) {
	Segment s;
	s.setType(Segment::Type::DATA);
	s.setSequenceNumber(0);
	char *d = (char *)data;
	
	while (length > 0) {
		s.setSequenceNumber(leastNotAcknowledged);
		s.setData(d, min(SegmentMaxLength, length));
		s.setChecksum();
		length -= s.dataLength();
		
		while (true) {
			sendSegment(s);
			receiveSegment(readingTimeout);
			
			if (receivedSegment.type() == Segment::Type::ACK && receivedSegment.acceptanceNumber() == leastNotAcknowledged) {
				leastNotAcknowledged++;
				break;
			}
		}
		
		d += s.dataLength();
		s.setSequenceNumber(leastNotAcknowledged);
	}
}

int Socket::read(void *buffer, int length) {
	/*if (readBufferIndex + unreadCount == readBuffer.size()) {
		memcpy(buffer, readBuffer.data() + readBufferIndex, unreadCount);
		length = unreadCount;
		readBufferIndex = unreadCount = 0;
		
		return length;
	}
	
	if (unreadCount >= length) {
		memcpy(buffer, readBuffer.data(), length);
		unreadCount -= length;
		readBufferIndex += length;
		
		return length;
	}*/
	
	while (true) {
		receiveSegment();
		
		if (receivedSegment.type() == Segment::Type::DATA && lastAcknowledged + 1 == receivedSegment.sequenceNumber()) {
			sendSegment(Segment::makeACK(receivedSegment.sequenceNumber()));
			lastAcknowledged++;
			break;
		} else if (receivedSegment.type() == Segment::Type::DATA && lastAcknowledged == receivedSegment.sequenceNumber()) {
			sendSegment(Segment::makeACK(receivedSegment.sequenceNumber()));
		}
	}
	
	
	
	return receivedSegment.copyData(buffer, length);
}

void Socket::sendSegment(const Segment &segment) {
	cout << "sending: " << segment.length() << " " << segment.calculateChecksum() << endl;
	::sendto(fileDescriptor, (const void *)&segment, segment.length(), 0, (sockaddr *)&address, addressLength);
}

void Socket::receiveSegment() {
	do {
		::recvfrom(fileDescriptor, &receivedSegment, SegmentMaxLength + SegmentHeaderLength, MSG_WAITALL, (sockaddr *)&address, &addressLength);
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

Segment::Type Socket::peakSegmentType() {
	pollfd pollFD = pollfd {
		.fd = fileDescriptor,
		.events = POLLIN,
	};
	
	poll(&pollFD, 1, 1000);
	
	::recvfrom(fileDescriptor, &peakBuffer, SegmentHeaderLength, MSG_PEEK, (sockaddr *)&address, &addressLength);
	
	return ((Segment *)peakBuffer)->type();
}
