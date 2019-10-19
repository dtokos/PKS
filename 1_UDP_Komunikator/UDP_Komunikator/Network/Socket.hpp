#ifndef Socket_hpp
#define Socket_hpp

#include <iostream>
#include <array>
#include <exception>
#include <string>
#include <sys/socket.h>
#include <sys/poll.h>
#include <arpa/inet.h>

#define SegmentHeaderLength (sizeof(Segment::Type) + sizeof(uint16_t) * 2 + sizeof(uint32_t) * 2)
#define SegmentMaxLength (1472 - SegmentHeaderLength)
#define SegmentType ((Type *)data)
#define SegmentChecksum ((uint16_t *)(data + sizeof(Type)))
#define SegmentSequenceNumber ((uint32_t *)(data + sizeof(Type) + sizeof(uint16_t)))
#define SegmentAcceptanceNumber ((uint32_t *)(data + sizeof(Type) + sizeof(uint16_t) + sizeof(uint32_t)))
#define SegmentDataLength ((uint16_t *)(data + sizeof(Type) + sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint32_t)))
#define SegmentData ((char *)(data + sizeof(Type) + sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint16_t)))

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
	struct Segment {
		enum Type : uint16_t {
			SYN = 0,
			SYNACK,
			ACK,
			DATA,
		};
		
		static Segment makeHeaderOnly(Type type) {
			Segment s;
			s.setType(type);
			s.setChecksum();
			
			return s;
		}
		
		static Segment makeSYN() { return Segment::makeHeaderOnly(Type::SYN); }
		static Segment makeSYNACK() { return Segment::makeHeaderOnly(Type::SYNACK); }
		static Segment makeACK(uint32_t acceptanceNumber = 0) {
			Segment s;
			s.setType(Type::ACK);
			s.setAcceptanceNumber(acceptanceNumber);
			s.setChecksum();
			
			return s;
		}
		
		Type type() { return *SegmentType; }
		void setType(Type type) { *SegmentType = type; }
		
		bool isValid() {
			cout << "validating: " << *SegmentChecksum << " " << calculateChecksum() << endl;
			return *SegmentChecksum == calculateChecksum();
		}
		uint16_t checksum() { return *SegmentChecksum; }
		void setChecksum() { *SegmentChecksum = calculateChecksum(); }
		uint16_t calculateChecksum() const {
			uint64_t sum = 0;
			uint16_t dataLength = *SegmentDataLength;
			char *dataPtr = SegmentData;
			
			sum += *SegmentType;
			sum += (*SegmentSequenceNumber & 0xFFFF) + ((*SegmentSequenceNumber >> 16) & 0xFFFF);
			sum += (*SegmentAcceptanceNumber & 0xFFFF) + ((*SegmentAcceptanceNumber >> 16) & 0xFFFF);
			sum += *SegmentDataLength;
			
			while(dataLength >= 2) {
				sum += ((*dataPtr << 8) & 0xFF00) + (*(dataPtr + 1) & 0xFF);
				
				dataLength -= 2;
				dataPtr += 2;
			}
			
			if (dataLength == 1)
				sum += ((*dataPtr << 8) & 0xFF00);
			
			while (sum >> 16)
				sum = (sum & 0xFFFF) + (sum >> 16);
				
			return (uint16_t)(~sum);
		}
		
		uint32_t sequenceNumber() { return *SegmentSequenceNumber; }
		void setSequenceNumber(uint32_t sequenceNumber) {*SegmentSequenceNumber = sequenceNumber; }
		
		uint32_t acceptanceNumber() { return *SegmentAcceptanceNumber; }
		void setAcceptanceNumber(uint32_t acceptanceNumber) { *SegmentAcceptanceNumber = acceptanceNumber; }
		
		uint16_t dataLength() const { return *SegmentDataLength; }
		void setDataLength(uint16_t dataLength) { *SegmentDataLength = dataLength; }
		
		void setData(const void *newData, uint16_t length) {
			setDataLength(length);
			memcpy(SegmentData, newData, length);
		}
		
		int copyData(void *buffer, uint16_t length) {
			length = min(length, *SegmentDataLength);
			memcpy(buffer, SegmentData, length);
			
			return length;
		}
		
		size_t length() const { return SegmentHeaderLength + *SegmentDataLength; }
	private:
		char data[SegmentMaxLength] = {0};
	};
	
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
