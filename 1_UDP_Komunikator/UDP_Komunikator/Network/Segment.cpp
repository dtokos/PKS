#include "Segment.hpp"

#define SegmentType ((Type *)data)
#define SegmentChecksum ((uint16_t *)(data + sizeof(Type)))
#define SegmentSequenceNumber ((uint32_t *)(data + sizeof(Type) + sizeof(uint16_t)))
#define SegmentAcceptanceNumber ((uint32_t *)(data + sizeof(Type) + sizeof(uint16_t) + sizeof(uint32_t)))
#define SegmentDataLength ((uint16_t *)(data + sizeof(Type) + sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint32_t)))
#define SegmentData ((char *)(data + sizeof(Type) + sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint16_t)))

const size_t Segment::HeaderLength;
const size_t Segment::MaxLength;
const size_t Segment::MaxDataLength;

Segment::Segment(Type type)  {
	setType(type);
	setChecksum();
}

Segment::Segment(Type type, uint32_t acceptanceNumber) {
	setType(type);
	setAcceptanceNumber(acceptanceNumber);
	setChecksum();
}

Segment::Segment(uint32_t acceptanceNumber) {
	setType(Type::ACK);
	setAcceptanceNumber(acceptanceNumber);
	setChecksum();
}

Segment::Segment(uint32_t sequenceNumber, const void *segmentData, uint16_t length) {
	setType(Type::DATA);
	setSequenceNumber(sequenceNumber);
	setData(segmentData, length);
	setChecksum();
}

Segment::Type Segment::type() {
	return *SegmentType;
}

void Segment::setType(Type type) {
	*SegmentType = type;
}

bool Segment::isValid() {
	return *SegmentChecksum == calculateChecksum();
}

uint16_t Segment::checksum() {
	return *SegmentChecksum;
}

void Segment::setChecksum() {
	*SegmentChecksum = calculateChecksum();
}

uint16_t Segment::calculateChecksum() const {
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

uint32_t Segment::sequenceNumber() const {
	return *SegmentSequenceNumber;
}

void Segment::setSequenceNumber(uint32_t sequenceNumber) {
	*SegmentSequenceNumber = sequenceNumber;
}

uint32_t Segment::acceptanceNumber() {
	return *SegmentAcceptanceNumber;
}

void Segment::setAcceptanceNumber(uint32_t acceptanceNumber) {
	*SegmentAcceptanceNumber = acceptanceNumber;
}

uint16_t Segment::dataLength() const {
	return *SegmentDataLength;
}

void Segment::setDataLength(uint16_t dataLength) {
	*SegmentDataLength = dataLength;
}

void Segment::setData(const void *newData, uint16_t length) {
	setDataLength(length);
	memcpy(SegmentData, newData, length);
}

int Segment::copyData(void *buffer, uint16_t length, uint16_t offset) {
	uint16_t remainingDataLength = *SegmentDataLength - offset;
	length = min(length, remainingDataLength);
	memcpy(buffer, SegmentData + offset, length);
	
	return length;
}

char* Segment::getData() {
	return SegmentData;
}

size_t Segment::length() const {
	return HeaderLength + *SegmentDataLength;
}

void Segment::scramble(bool shoudScramble) {
	if ((isSrambled && !shoudScramble) || (!isSrambled && shoudScramble)) {
		*SegmentChecksum = *SegmentChecksum ^ 0x1;
		isSrambled = !isSrambled;
	}
}
