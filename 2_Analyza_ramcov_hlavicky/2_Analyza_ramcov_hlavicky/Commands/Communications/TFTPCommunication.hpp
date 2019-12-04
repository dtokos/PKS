#ifndef TFTPCommunication_hpp
#define TFTPCommunication_hpp

#include "./UDPCommunication.hpp"

class TFTPCommunication : public UDPCommunication {
public:
	TFTPCommunication(Frame *frame) : UDPCommunication(frame) {}
	
	bool add(Frame *frame) {
		if (!belongsToThisCommunication(frame))
			return false;
		
		if (frames.size() == 1)
			segment = (UDPSegment *)frame->segment;
		
		frames.push_back(frame);
		
		return false;
	}
	
private:
	bool belongsToThisCommunication(Frame *frame) {
		if (frame->segment == NULL || frame->segment->segmentType() != Segment::UDP)
			return false;
		
		UDPSegment *segment = (UDPSegment *)frame->segment;
		
		return packet->sameEndpoints((IPv4Packet *)frame->packet) && (
			(frames.size() == 1 && this->segment->sourcePort() == segment->destinationPort()) ||
			(frames.size() > 1 && this->segment->sameEndpoints(segment))
		);
	}
};

#endif
