#include "Segment.hpp"

Segment Segment::makeHeaderOnly(Type type) {
	Segment s;
	s.setType(type);
	s.setChecksum();
	
	return s;
}

Segment Segment::makeSYN() {
	return Segment::makeHeaderOnly(Type::SYN);
}

Segment Segment::makeSYNACK() {
	return Segment::makeHeaderOnly(Type::SYNACK);
}

Segment Segment::makeACK(uint32_t acceptanceNumber) {
	Segment s;
	s.setType(Type::ACK);
	s.setAcceptanceNumber(acceptanceNumber);
	s.setChecksum();
	
	return s;
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

uint32_t Segment::sequenceNumber() {
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

int Segment::copyData(void *buffer, uint16_t length) {
	length = min(length, *SegmentDataLength);
	memcpy(buffer, SegmentData, length);
	
	return length;
}

size_t Segment::length() const {
	return SegmentHeaderLength + *SegmentDataLength;
}
