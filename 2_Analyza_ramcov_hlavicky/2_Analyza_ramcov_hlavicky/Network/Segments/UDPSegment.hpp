#ifndef UDPSegment_hpp
#define UDPSegment_hpp

#include "./Segment.hpp"

class UDPSegment : public Segment {
public:
	UDPSegment(uint8_t *raw, string name) : Segment(raw, name) {}
	
	Type segmentType() {return Segment::UDP;}
	string name() {return _name;}
	
	uint16_t sourcePort() {return ntohs(getField<uint16_t>(0));}
	uint16_t destinationPort() {return ntohs(getField<uint16_t>(2));}
	
	bool sameEndpoints(UDPSegment *segment) {
		return (sourcePort() == segment->sourcePort() && destinationPort() == segment->destinationPort()) ||
			(sourcePort() == segment->destinationPort() && destinationPort() == segment->sourcePort());
	}
};

#endif
