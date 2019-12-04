#ifndef TCPSegment_hpp
#define TCPSegment_hpp

#include "./Segment.hpp"
#include <bitset>

class TCPSegment : public Segment {
public:
	TCPSegment(uint8_t *raw, string name) : Segment(raw, name) {}
	
	struct Flags {
		enum Type {FIN = 1, SYN = 2, RST = 4, PSH = 8, ACK = 16, URG = 32};
		
		bool is(Type type) {return raw & type;}
		
		uint8_t raw;
	};
	
	Type segmentType() {return Segment::TCP;}
	string name() {return _name;}
	
	uint16_t sourcePort() {return ntohs(getField<uint16_t>(0));}
	uint16_t destinationPort() {return ntohs(getField<uint16_t>(2));}
	
	Flags flags() {return getField<Flags>(13);}
	
	bool sameEndpoints(TCPSegment *segment) {
		return (sourcePort() == segment->sourcePort() && destinationPort() == segment->destinationPort()) ||
			(sourcePort() == segment->destinationPort() && destinationPort() == segment->sourcePort());
	}
};

#endif
