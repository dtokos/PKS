#ifndef Segment_hpp
#define Segment_hpp

#include <iostream>

using namespace std;

struct Segment {
public:
	enum Type : uint16_t {
		SYN = 0,
		SYNACK,
		ACKSYN,
		ACK,
		DATA,
		PING,
		PONG,
		FIN,
		FINACK,
		ACKFIN,
	};
	
	static const size_t HeaderLength = (sizeof(Segment::Type) + sizeof(uint16_t) * 2 + sizeof(uint32_t) * 2);
	static const size_t MaxLength = 1472; // 1500 - IP header - UDP header
	static const size_t MaxDataLength = MaxLength - HeaderLength;
	static const size_t MinDataLength = 1;
	
	Segment(Type type);
	Segment(uint32_t acceptanceNumber = 0);
	Segment(uint32_t sequenceNumber, const void *segmentData, uint16_t length);
	
	Type type();
	void setType(Type type);
	
	bool isValid();
	uint16_t checksum();
	void setChecksum();
	uint16_t calculateChecksum() const;
	
	uint32_t sequenceNumber() const;
	void setSequenceNumber(uint32_t sequenceNumber);
	
	uint32_t acceptanceNumber();
	void setAcceptanceNumber(uint32_t acceptanceNumber);
	
	uint16_t dataLength() const;
	void setDataLength(uint16_t dataLength);
	void setData(const void *newData, uint16_t length);
	int copyData(void *buffer, uint16_t length, uint16_t offset = 0);
	char *getData();
	
	size_t length() const;
	
private:
	char data[MaxLength] = {0};
};

#endif
