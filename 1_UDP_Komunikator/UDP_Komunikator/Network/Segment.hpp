#ifndef Segment_hpp
#define Segment_hpp

#include <iostream>

#define SegmentHeaderLength (sizeof(Segment::Type) + sizeof(uint16_t) * 2 + sizeof(uint32_t) * 2)
#define SegmentMaxLength (1472 - SegmentHeaderLength)
#define SegmentType ((Type *)data)
#define SegmentChecksum ((uint16_t *)(data + sizeof(Type)))
#define SegmentSequenceNumber ((uint32_t *)(data + sizeof(Type) + sizeof(uint16_t)))
#define SegmentAcceptanceNumber ((uint32_t *)(data + sizeof(Type) + sizeof(uint16_t) + sizeof(uint32_t)))
#define SegmentDataLength ((uint16_t *)(data + sizeof(Type) + sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint32_t)))
#define SegmentData ((char *)(data + sizeof(Type) + sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint16_t)))

using namespace std;

struct Segment {
public:
	enum Type : uint16_t {
		SYN = 0,
		SYNACK,
		ACK,
		DATA,
	};
	
	static Segment makeHeaderOnly(Type type);
	static Segment makeSYN();
	static Segment makeSYNACK();
	static Segment makeACK(uint32_t acceptanceNumber = 0);
	
	Type type();
	void setType(Type type);
	
	bool isValid();
	uint16_t checksum();
	void setChecksum();
	uint16_t calculateChecksum() const;
	
	uint32_t sequenceNumber();
	void setSequenceNumber(uint32_t sequenceNumber);
	
	uint32_t acceptanceNumber();
	void setAcceptanceNumber(uint32_t acceptanceNumber);
	
	uint16_t dataLength() const;
	void setDataLength(uint16_t dataLength);
	void setData(const void *newData, uint16_t length);
	int copyData(void *buffer, uint16_t length);
	
	size_t length() const;
	
private:
	char data[SegmentMaxLength] = {0};
};

#endif
