#ifndef Socket_hpp
#define Socket_hpp

#include <exception>
#include <string>
#include <sys/socket.h>
#include <sys/poll.h>
#include <arpa/inet.h>

#define SegmentHeaderLength (sizeof(Segment::Type) + sizeof(uint16_t) * 2 + sizeof(uint32_t) * 2)

using namespace std;

class Socket {
public:
	class SocketError : public exception {
	public:
		string message;
		
		SocketError(string message) : message(message) {};
		const char * what () const throw () {
			return message.c_str();
		}
	};
	
	Socket(int fileDescriptor, sockaddr_in address);
	
protected:
	struct Segment {
		enum Type : uint16_t {
			SYN = 0,
			SYNACK,
			ACK,
			DATA,
		};
		
		Type type;
		uint16_t checksum;
		uint32_t sequenceNumber;
		uint32_t acceptanceNumber;
		uint16_t dataLength;
		
		static Segment makeSYN() {
			return Segment {
				.type = Type::SYN,
			};
		}
		
		static Segment makeSYNACK() {
			return Segment {
				.type = Type::SYNACK,
			};
		}
		
		static Segment makeACK(uint32_t acceptanceNumber = 0) {
			return Segment {
				.type = Type::ACK,
				.acceptanceNumber = acceptanceNumber,
			};
		}
		
		size_t length() const {
			return SegmentHeaderLength + dataLength;
		}
		
		size_t maxLength() {
			return 1500;
		}
	};
	
	int fileDescriptor;
	sockaddr_in address;
	socklen_t addressLength;
	Segment receivedSegment;
	char peakBuffer[SegmentHeaderLength];
	
	void sendSegment(const Segment &segment);
	void receiveSegment();
	bool receiveSegment(int timeout);
	Segment::Type peakSegmentType();
};

#endif
