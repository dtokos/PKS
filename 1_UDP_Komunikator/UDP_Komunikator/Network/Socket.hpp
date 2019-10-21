#ifndef Socket_hpp
#define Socket_hpp

#include <iostream>
#include <array>
#include <exception>
#include <string>
#include <sys/socket.h>
#include <sys/poll.h>
#include <arpa/inet.h>
#include "./Segment.hpp"
#include "./ReadBuffer.hpp"

using namespace std;

class Socket {
public:
	class SocketError : public exception {
	public:
		string message;
		
		SocketError(string message) : message(message) {}
		const char * what () const throw () {
			return message.c_str();
		}
	};
	
	class SocketCreateError : public SocketError {
	public:
		SocketCreateError(string message) : SocketError(message) {}
	};
	
	class SocketWriteError : public SocketError {
	public:
		SocketWriteError(string message) : SocketError(message) {}
	};
	
	static const int InitialReadingTimeout = 200;
	
	Socket(int fileDescriptor, sockaddr_in address, int readingTimeout = InitialReadingTimeout);
	void write(const void *data, size_t length);
	int read(void *buffer, int count);
	
protected:
	int fileDescriptor, readingTimeout;
	sockaddr_in address;
	socklen_t addressLength;
	Segment receivedSegment;
	char peakBuffer[Segment::HeaderLength];
	Segment *peakedSegment;
	uint32_t lastAcknowledged = 0, leastNotAcknowledged = 1;
	ReadBuffer readBuffer;
	
	void sendSegment(const Segment &segment);
	void receiveSegment();
	bool receiveSegment(int timeout);
	bool peakSegment(int timeout);
	void increaseReadingTimeout();
};

#endif
