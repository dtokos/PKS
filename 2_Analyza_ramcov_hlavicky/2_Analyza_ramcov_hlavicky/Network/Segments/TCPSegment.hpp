#ifndef TCPSegment_hpp
#define TCPSegment_hpp

#include "./Segment.hpp"

class TCPSegment : public Segment {
public:
	TCPSegment(uint8_t *raw, string name) : Segment(raw, name) {}
	
	Type segmentType() {return Segment::TCP;}
	string name() {return _name;}
	
	uint16_t sourcePort() {return ntohs(getField<uint16_t>(0));}
	uint16_t destinationPort() {return ntohs(getField<uint16_t>(2));}
};

#endif
