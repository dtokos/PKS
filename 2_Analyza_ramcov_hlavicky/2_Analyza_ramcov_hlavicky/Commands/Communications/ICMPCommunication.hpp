#ifndef ICMPCommunication_hpp
#define ICMPCommunication_hpp

#include "./Communication.hpp"

class ICMPCommunication : Communication {
public:
	ICMPCommunication(Frame *frame) :
		frames({frame}),
		packet((IPv4Packet *)frame->packet),
		segment((ICMPSegment *)packet->segment) {}
	
	bool isComplete() {return false;}
	bool usesConnection() {return false;}
	
	bool add(Frame *frame) {
		if (!belongsToThisCommunication(frame))
			return false;
		
		frames.push_back(frame);
		
		return true;
	}
	
	void print(int length) {
		printHeader();
		printFrames(length);
	}
	
private:
	vector<Frame *> frames;
	IPv4Packet *packet;
	ICMPSegment *segment;
	
	bool belongsToThisCommunication(Frame *frame) {
		if (frame->segment == NULL || frame->segment->segmentType() != Segment::ICMP)
			return false;
		
		ICMPSegment *segment = (ICMPSegment *)frame->segment;
		//cout << frame->serialNumber << endl;
		
		// TODO: implement TTL handling
		return packet->sameEndpoints((IPv4Packet *)frame->packet) ||
			(
			 (segment->type() == ICMPSegment::TimeExceeded || segment->type() == ICMPSegment::DestinationUnreachable) &&
			 	packet->sameEndpoints(segment->ipHeader)
			 );
	}
	
	void printHeader() {
		cout << segment->name() << endl;
		Print::ipv4Packet(packet);
		cout << endl;
	}
	
	void printFrames(int length) {
		int i = 0;
		for (; i < frames.size() && i < length / 2; i++)
			Print::frame(frames[i]);
		
		for (i = max(i, (int)frames.size() - length / 2); i < frames.size(); i++)
			Print::frame(frames[i]);
	}
};

#endif
