#ifndef ICMPSegment_hpp
#define ICMPSegment_hpp

#include "./Segment.hpp"

class ICMPSegment : public Segment {
public:
	enum ICMPType {
		EchoReply = 0,
		DestinationUnreachable = 3,
		SourceQuench = 4,
		Redirect = 5,
		EchoRequest = 8,
		RouterAdvertisement = 9,
		RouterSelection = 10,
		TimeExceeded = 11,
		ParameterProblem = 12,
		Timestamp = 13,
		TimestampReply = 14,
		InformationRequest = 15,
		InformationReply = 16,
		AddressMaskRequest = 17,
		AddressMaskReply = 18,
		Traceroute = 30,
	};
	ICMPSegment(uint8_t *raw, string name) : Segment(raw, name), ipHeader(new IPv4Packet(raw + 8, "")) {}
	~ICMPSegment() {delete ipHeader;}
	
	Type segmentType() {return Segment::ICMP;}
	string name() {return _name;}
	
	uint8_t type() {return getField<uint8_t>(0);}
	uint8_t code() {return getField<uint8_t>(1);}
	
	IPv4Packet *ipHeader;
};

#endif
