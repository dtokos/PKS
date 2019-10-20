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

//#define ReadBufferSize (SegmentMaxLength * 8)

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
	
	Socket(int fileDescriptor, sockaddr_in address);
	void write(const void *data, size_t length);
	int read(void *buffer, int count);
	
protected:
	
	int fileDescriptor, readingTimeout;
	sockaddr_in address;
	socklen_t addressLength;
	Segment receivedSegment;
	char peakBuffer[SegmentHeaderLength];
	uint32_t lastAcknowledged = 0, leastNotAcknowledged = 1;
	array<char, SegmentMaxLength * 8> readBuffer = {0};
	uint16_t unreadCount = 0, readBufferIndex = 0;
	
	void sendSegment(const Segment &segment);
	void receiveSegment();
	bool receiveSegment(int timeout);
	Segment::Type peakSegmentType();
};

#endif
