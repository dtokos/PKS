#ifndef Socket_hpp
#define Socket_hpp

#include <iostream>
#include <thread>
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
protected:
	enum State {
		DISCONNECTED,
		ESTABLISHED,
	};
	
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
	
	Socket(int fileDescriptor, sockaddr_in address, int readingTimeout = InitialReadingTimeout, State state = DISCONNECTED);
	Socket(Socket const& other);
	~Socket();
	void write(const void *data, size_t length);
	int read(void *buffer, int count);
	void disconnect();
	
protected:
	int fileDescriptor, readingTimeout;
	sockaddr_in address;
	socklen_t addressLength;
	State state = State::DISCONNECTED;
	Segment receivedSegment, peakedSegment;
	uint32_t lastAcknowledged = 0, leastNotAcknowledged = 1;
	ReadBuffer readBuffer;
	thread readingThread;
	condition_variable writingCV, readingCV;
	mutable mutex writingMutex, readingMutex;
	int maxRetries = 10;
	
	void sendSegment(const Segment &segment);
	void receiveSegment();
	bool receiveSegment(int timeout);
	bool peakSegment(int timeout);
	void increaseReadingTimeout();
	void setState(State newState);
	void startThreads();
	void stopThreads();
	void readingLoop();
};

#endif
