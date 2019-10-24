#include "Socket.hpp"

const int Socket::MinReadingTimeout;
const int Socket::MaxReadingTimeout;

Socket::Socket(int fileDescriptor, sockaddr_in address, size_t maxSegmentSize, int readingTimeout, State state) :
	fileDescriptor(fileDescriptor),
	readingTimeout(readingTimeout),
	maxSegmentSize(maxSegmentSize),
	address(address),
	addressLength(sizeof(address)) {
		setState(state);
	}

Socket::Socket(Socket const& other) {
	lock_guard<mutex> lock(other.writingMutex);
	
	fileDescriptor = other.fileDescriptor;
	readingTimeout = other.readingTimeout;
	address = other.address;
	addressLength = other.addressLength;
	maxSegmentSize = other.maxSegmentSize;
	receivedSegment = other.receivedSegment;
	peakedSegment = other.peakedSegment;
	lastAcknowledged = other.lastAcknowledged;
	leastNotAcknowledged = other.leastNotAcknowledged;
	readBuffer = other.readBuffer;
	didReceivePingACK = other.didReceivePingACK;
	setState(other.state);
};

Socket::~Socket() {
	stopThreads();
	close(fileDescriptor);
}

void Socket::write(const void *data, size_t length) {
	if (state != ESTABLISHED)
		throw SocketDisconnectedError();
	
	Segment s;
	s.setType(Segment::Type::DATA);
	char *d = (char *)data;
	int retries = 0;
	
	while (length > 0 && state == ESTABLISHED) {
		s.setSequenceNumber(leastNotAcknowledged);
		s.setData(d, min(maxSegmentSize, length));
		s.setChecksum();
		length -= s.dataLength();
		
		while (++retries < maxRetries && state == ESTABLISHED) {
			cout << "Sending segment with data size: " << s.dataLength() << " bytes" << endl;
			s.scramble(shouldScrambleSegment());
			sendSegment(s);
			
			unique_lock<mutex> lock(writingMutex);
			didReceiveNAK = false;
			bool didReceiveACK = writingCV.wait_for(lock, chrono::milliseconds(readingTimeout), [this, s] {
				return leastNotAcknowledged > s.sequenceNumber() || didReceiveNAK;
			});
			
			lock.unlock();
			
			if (didReceiveACK && !didReceiveNAK)
				break;
			
			increaseReadingTimeout();
		}
		
		if (retries == maxRetries)
			throw SocketWriteError("Could not send data");
		
		d += s.dataLength();
		s.setSequenceNumber(leastNotAcknowledged);
		retries = 0;
	}
}

int Socket::read(void *buffer, int length) {
	if (state != ESTABLISHED)
		throw SocketDisconnectedError();
	
	unique_lock<mutex> lock(readingMutex);
	bool hasData = readingCV.wait_for(lock, chrono::milliseconds(1000), [this] {
		return readBuffer.hasData();
	});
	
	if (!hasData) {
		lock.unlock();
		return 0;
	}
	
	int bytesRed = readBuffer.read((char *)buffer, length);
	lock.unlock();
	
	return bytesRed;
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
				if (receivedSegment.type() == Segment::Type::DATA)
					sendSegment(Segment(Segment::Type::NAK, receivedSegment.sequenceNumber()));
				
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
	readingTimeout += readingTimeout / 2;
	readingTimeout = min(readingTimeout, MaxReadingTimeout);
}

void Socket::decreaseReadingTimeout() {
	readingTimeout -= readingTimeout / 4;
	readingTimeout = max(readingTimeout, MinReadingTimeout);
}

void Socket::setState(State newState) {
	State oldState = state;
	{
		lock_guard<mutex> lock(pingSleepMutex);
		state = newState;
	}
	pingSleepCV.notify_all();
	
	switch (newState) {
		case ESTABLISHED:
			if (oldState != ESTABLISHED)
				disconnectState = SendingFIN;
				startThreads();
			break;
			
		case DISCONNECTED:
			if (oldState == ESTABLISHED)
				stopThreads();
			break;
	}
}

void Socket::startThreads() {
	didStartRead = true;
	readingThread = thread(&Socket::readingLoop, this);
	didStartPing = true;
	pingThread = thread(&Socket::pingLoop, this);
}

void Socket::stopThreads() {
	if (!didJoinRead && didStartRead) {
		didJoinRead = true;
		readingThread.join();
	}
	
	if (!didJoinPing && didStartPing) {
		didJoinPing = true;
		pingThread.join();
	}
}

void Socket::readingLoop() {
	while (state == ESTABLISHED) {
		 if (!peakSegment(100))
			 continue;
		
		if (!peakedSegment.isValid()) {
			receiveSegment(1);
			continue;
		}
		
		decreaseReadingTimeout();
		
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
			
			continue;
		}
		
		if (peakedSegment.type() == Segment::Type::NAK) {
			receiveSegment();
			if (receivedSegment.acceptanceNumber() == leastNotAcknowledged) {
				{
					lock_guard<mutex> lock(writingMutex);
					didReceiveNAK = true;
				}
				writingCV.notify_all();
			}
			
			continue;
		}
		
		if (peakedSegment.type() == Segment::Type::PING) {
			receiveSegment();
			sendSegment(Segment(Segment::Type::PONG));
			
			continue;
		}
		
		if (peakedSegment.type() == Segment::Type::PONG) {
			receiveSegment();
			{
				lock_guard<mutex> lock(pingMutex);
				didReceivePingACK = true;
			}
			pingCV.notify_all();
			
			continue;
		}
		
		if (peakedSegment.type() == Segment::Type::FIN) {
			receiveSegment();
			sendSegment(Segment(Segment::Type::FINACK));
			
			continue;
		}
		
		if (peakedSegment.type() == Segment::Type::FINACK) {
			receiveSegment();
			{
				lock_guard<mutex> lock(disconnectMutex);
				disconnectState = ReceivedFINACK;
			}
			disconnectCV.notify_all();
			
			continue;
		}
		
		if (peakedSegment.type() == Segment::Type::ACKFIN) {
			receiveSegment();
			state = DISCONNECTED;
			
			continue;
		}
	}
}

void Socket::pingLoop() {
	Segment ping(Segment::Type::PING);
	int retries = 0;
	
	while (state == ESTABLISHED && ++retries < maxRetries) {
		if (didReceivePingACK) {
			unique_lock<mutex> sleepLock(pingSleepMutex);
			didReceivePingACK = false;
			bool didChangeState = pingSleepCV.wait_for(sleepLock, chrono::seconds(10), [this] {
				return state != ESTABLISHED;
			});
			sleepLock.unlock();
			retries = 0;
			
			if (didChangeState)
				break;
		}
		
		sendSegment(ping);
			
		unique_lock<mutex> lock(pingMutex);
		bool didReceiveAck = pingCV.wait_for(lock, chrono::milliseconds(readingTimeout), [this] {
			return didReceivePingACK;
		});
		lock.unlock();
		
		if (!didReceiveAck)
			increaseReadingTimeout();
	}
	
	if (state == ESTABLISHED && retries == maxRetries)
		state = DISCONNECTED;
}

void Socket::disconnect() {
	sendFINAndReceiveFINACK();
	if (state == DISCONNECTED)
		return;
	
	sendACKFIN();
	if (state == DISCONNECTED)
		return;
	
	setState(State::DISCONNECTED);
}

void Socket::sendFINAndReceiveFINACK() {
	int retries = 0;
	
	while (++retries < maxRetries && disconnectState == SendingFIN && state == ESTABLISHED) {
		sendSegment(Segment(Segment::Type::FIN));
		
		unique_lock<mutex> lock(disconnectMutex);
		bool didReceiveACK = disconnectCV.wait_for(lock, chrono::milliseconds(readingTimeout), [this] {
			return disconnectState == ReceivedFINACK || state == DISCONNECTED;
		});
		
		if (didReceiveACK)
			return;
		
		increaseReadingTimeout();
	}
	
	if (retries == maxRetries)
		setState(DISCONNECTED);
}

void Socket::sendACKFIN() {
	int retries = 0;
	
	while (++retries < maxRetries && disconnectState == ReceivedFINACK && state == ESTABLISHED) {
		sendSegment(Segment(Segment::Type::ACKFIN));
		
		unique_lock<mutex> lock(disconnectMutex);
		disconnectState = ReceivedACKFIN;
		bool didReceiveFINACK = disconnectCV.wait_for(lock, chrono::milliseconds(readingTimeout), [this] {
			return disconnectState == ReceivedFINACK && state == ESTABLISHED;
		});
		
		if (!didReceiveFINACK)
			return;
		
		increaseReadingTimeout();
	}
	
	if (retries == maxRetries)
		setState(DISCONNECTED);
}

bool Socket::shouldScrambleSegment() {
	return (rand() % 100) < 5;
}
