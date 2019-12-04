#ifndef UDPCommunication_hpp
#define UDPCommunication_hpp

#include "./Communication.hpp"

class UDPCommunication : Communication {
public:
	UDPCommunication(Frame *frame) :
		frames({frame}),
		packet((IPv4Packet *)frame->packet),
		segment((UDPSegment *)packet->segment),
		message(segment->message) {}
	
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
	UDPSegment *segment;
	Message *message;
	
	bool belongsToThisCommunication(Frame *frame) {
		if (frame->segment == NULL || frame->segment->segmentType() != Segment::UDP)
			return false;
		
		return packet->sameEndpoints((IPv4Packet *)frame->packet) &&
			segment->sameEndpoints((UDPSegment *)segment);
	}
	
	void printHeader() {
		cout << segment->name();
		if (message)
			cout << " - " << message->name();
		cout << endl;
		Print::ipv4Packet(packet);
		Print::segment(segment);
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
