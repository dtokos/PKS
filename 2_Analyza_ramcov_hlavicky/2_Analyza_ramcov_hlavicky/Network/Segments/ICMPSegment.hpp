#ifndef ICMPSegment_hpp
#define ICMPSegment_hpp

#include "./Segment.hpp"

class ICMPSegment : public Segment {
public:
	ICMPSegment(uint8_t *raw, string name) : Segment(raw, name) {}
	
	Type segmentType() {return Segment::ICMP;}
	string name() {return _name;}
};

#endif
