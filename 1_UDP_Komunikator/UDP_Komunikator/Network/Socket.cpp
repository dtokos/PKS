#include "Socket.hpp"

Socket::Socket(int fileDescriptor, sockaddr_in address, int readingTimeout, State state) :
	fileDescriptor(fileDescriptor),
	address(address),
	addressLength(sizeof(address)),
	readingTimeout(readingTimeout) {
		setState(state);
	}

Socket::Socket(Socket const& other) {
	lock_guard<mutex> lock(other.writingMutex);
	
	fileDescriptor = other.fileDescriptor;
	readingTimeout = other.readingTimeout;
	address = other.address;
	addressLength = other.addressLength;
	receivedSegment = other.receivedSegment;
	peakedSegment = other.peakedSegment;
	lastAcknowledged = other.lastAcknowledged;
	leastNotAcknowledged = other.leastNotAcknowledged;
	readBuffer = other.readBuffer;
	setState(other.state);
};

Socket::~Socket() {
	setState(DISCONNECTED);
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
			
			unique_lock<mutex> lock(writingMutex);
			bool didReceiveACK = writingCV.wait_for(lock, chrono::milliseconds(readingTimeout), [this, s] {
				return leastNotAcknowledged > s.sequenceNumber();
			});
			lock.unlock();
			
			if (didReceiveACK)
				break;
			
			increaseReadingTimeout();
		}
		
		if (retries == 10)
			throw SocketWriteError("Could not send data");
		
		d += s.dataLength();
		s.setSequenceNumber(leastNotAcknowledged);
	}
}

int Socket::read(void *buffer, int length) {
	if (!readBuffer.hasData()) {
		unique_lock<mutex> lock(readingMutex);
		readingCV.wait_for(lock, chrono::milliseconds(1000), [this] {
			return readBuffer.hasData();
		});
		lock.unlock();
	}
	
	if (!readBuffer.hasData())
		return 0;
	
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
				/* TODO: send NAK */
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
			::recvfrom(fileDescriptor, &peakedSegment, Segment::MaxLength, MSG_PEEK, (sockaddr *)&address, &addressLength);
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
	State oldState = state;
	state = newState;
	
	switch (newState) {
		case ESTABLISHED:
			if (oldState != ESTABLISHED)
				startThreads();
			break;
			
		case DISCONNECTED:
			if (oldState == ESTABLISHED)
				stopThreads();
			break;
	}
}

void Socket::startThreads() {
	readingThread = thread(&Socket::readingLoop, this);
}

void Socket::stopThreads() {
	readingThread.join();
}

void Socket::readingLoop() {
	while (state == ESTABLISHED) {
		 if (!peakSegment(100))
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
				{
					lock_guard<mutex> lock(readingMutex);
					readBuffer.add(receivedSegment);
				}
				readingCV.notify_all();
			} else if (lastAcknowledged == receivedSegment.sequenceNumber())
				sendSegment(Segment(receivedSegment.sequenceNumber()));
			
			continue;
		}
		
		if (peakedSegment.type() == Segment::Type::ACK) {
			receiveSegment();
			
			if (receivedSegment.acceptanceNumber() >= leastNotAcknowledged) {
				{
					lock_guard<mutex> lock(writingMutex);
					leastNotAcknowledged = receivedSegment.acceptanceNumber() + 1;
				}
				writingCV.notify_all();
			}
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

void Socket::disconnect() {
	setState(State::DISCONNECTED);
}
