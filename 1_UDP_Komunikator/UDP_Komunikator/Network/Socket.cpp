#include "Socket.hpp"

Socket::Socket(int fileDescriptor, sockaddr_in address, int readingTimeout, State state) :
	fileDescriptor(fileDescriptor),
	address(address),
	addressLength(sizeof(address)),
	readingTimeout(readingTimeout) {
		setState(state);
	}

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
			sendSegment(s);
			receiveSegment(readingTimeout);
			
			if (receivedSegment.type() == Segment::Type::ACK && receivedSegment.acceptanceNumber() == leastNotAcknowledged) {
				leastNotAcknowledged++;
				break;
			}
			
			increaseReadingTimeout();
		}
		
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
			::recvfrom(fileDescriptor, &receivedSegment, Segment::MaxLength, MSG_WAITALL, (sockaddr *)&address, &addressLength);
			if (!receivedSegment.isValid()) {
				/* send NAK */
				return false;
			}
			
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
			::recvfrom(fileDescriptor, &peakedSegment, Segment::HeaderLength, MSG_PEEK, (sockaddr *)&address, &addressLength);
			return true;
	}
}

void Socket::increaseReadingTimeout() {
	if (readingTimeout < 5000)
		readingTimeout *= 2;
	else
		readingTimeout += readingTimeout / 2;
}

void Socket::setState(State newState) {
	switch (newState) {
		case ESTABLISHED:
			if (state != ESTABLISHED)
				startThreads();
			break;
			
		case DISCONNECTED:
			if (state == ESTABLISHED)
				stopThreads();
			break;
	}
}

void Socket::startThreads() {
	readingThread = thread(&Socket::readingLoop, this);
	writingThread = thread(&Socket::writingLoop, this);
}

void Socket::stopThreads() {
	readingThread.join();
	writingThread.join();
}

void Socket::readingLoop() {
	while (state == ESTABLISHED) {
		 if (!peakSegment(10))
			 continue;
		
		if (!peakedSegment.isValid()) {
			receiveSegment(1);
			continue;
		}
		
		if (readBuffer.canFitWholeSegment() && peakedSegment.type() == Segment::Type::DATA) {
			receiveSegment();
			
			if (lastAcknowledged + 1 == receivedSegment.sequenceNumber()) {
				sendSegment(Segment(receivedSegment.sequenceNumber()));
				lastAcknowledged++;
				readBuffer.add(receivedSegment);
				break;
			} else if (lastAcknowledged == receivedSegment.sequenceNumber()) {
				sendSegment(Segment(receivedSegment.sequenceNumber()));
			}
			
			continue;
		}
		
		if (peakedSegment.type() == Segment::Type::ACK) {
			receiveSegment();
			
			if (receivedSegment.acceptanceNumber() >= leastNotAcknowledged)
				leastNotAcknowledged = receivedSegment.acceptanceNumber();
		}
		
		// ---- read data into buffer if available space
		// ---- reply with ACKS and mark lastAcked
		// ---- read ACKS and mark leastNotAckaed
	}
}

void Socket::writingLoop() {
	while (state == ESTABLISHED) {
		// write data from buffer
		// wait for ACK
	}
}
