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
#include <unistd.h>
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
	
	class SocketDisconnectedError : public SocketError {
	public:
		SocketDisconnectedError() : SocketError("Cant read/write on disconnected socket") {}
	};
	
	static const int InitialReadingTimeout = 200;
	static const int MaxReadingTimeout = 5000;
	
	Socket(int fileDescriptor, sockaddr_in address, size_t maxSegmentSize, int readingTimeout = InitialReadingTimeout, State state = DISCONNECTED);
	Socket(Socket const& other);
	~Socket();
	void write(const void *data, size_t length);
	int read(void *buffer, int count);
	void disconnect();
	
protected:
	int fileDescriptor, readingTimeout;
	size_t maxSegmentSize;
	sockaddr_in address;
	socklen_t addressLength;
	State state = State::DISCONNECTED;
	Segment receivedSegment, peakedSegment;
	uint32_t lastAcknowledged = 0, leastNotAcknowledged = 1;
	ReadBuffer readBuffer;
	thread readingThread, pingThread;
	condition_variable writingCV, readingCV, pingCV, pingSleepCV, disconnectCV;
	mutable mutex writingMutex, readingMutex, pingMutex, pingSleepMutex, disconnectMutex;
	const int maxRetries = 10;
	bool didReceivePingACK = true;
	bool didJoinRead = false, didJoinPing = false, didStartRead = false, didStartPing = false;
	
	void sendSegment(const Segment &segment);
	void receiveSegment();
	bool receiveSegment(int timeout);
	bool peakSegment(int timeout);
	void increaseReadingTimeout();
	void setState(State newState);
	void startThreads();
	virtual void stopThreads();
	void readingLoop();
	void pingLoop();
	void sendFINAndReceiveFINACK();
	void sendACKFIN();
	
	enum SenderDisconnectState {
		SendingFIN,
		ReceivedFINACK,
		ReceivedACKFIN,
	};
	
	SenderDisconnectState disconnectState = SendingFIN;
};

#endif
