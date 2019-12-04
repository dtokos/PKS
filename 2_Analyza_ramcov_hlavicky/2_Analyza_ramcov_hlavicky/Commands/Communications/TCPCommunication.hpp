#ifndef TCPCommunication_hpp
#define TCPCommunication_hpp

#include "./Communication.hpp"

using Flags = TCPSegment::Flags;

class TCPCommunication : Communication {
public:
	TCPCommunication(Frame *frame) :
		frames({frame}),
		packet((IPv4Packet *)frame->packet),
		segment((TCPSegment *)packet->segment),
		message(segment->message) {};
	
	bool add(Frame *frame) {
		if (!belongsToThisCommunication(frame))
			return false;
		
		TCPSegment *segment = (TCPSegment *)frame->segment;
		frames.push_back(frame);
		
		switch (currentState) {
			case Opened:
				handleOpened(segment);
				break;
				
			case SenderFIN:
				handleSenderFIN(segment);
				break;
				
			case ReceiverACK:
				handleReceiverACK(segment);
				break;
				
			case ReceiverFIN:
				handleReceiverFIN(segment);
				break;
				
			case Closed:
				break;
		}
		
		return true;
	}
	
	bool isComplete() {return currentState == Closed;}
	bool usesConnection() {return true;}
	
	void print(int length) {
		printHeader();
		printFrames(length);
	}
private:
	enum State {Opened, SenderFIN, ReceiverACK, ReceiverFIN, Closed};
	vector<Frame *> frames;
	State currentState = Opened;
	IPv4Packet *packet;
	TCPSegment *segment;
	Message *message;
	
	bool belongsToThisCommunication(Frame *frame) {
		if (frame->segment == NULL || frame->segment->segmentType() != Segment::TCP)
			return false;
		
		return packet->sameEndpoints((IPv4Packet *)frame->packet) && segment->sameEndpoints((TCPSegment *)frame->segment);
	}
	
	void handleOpened(TCPSegment *segment) {
		if (segment->flags().is(Flags::RST))
			currentState = Closed;
		else if (segment->flags().is(Flags::FIN))
			currentState = SenderFIN;
	}
	
	void handleSenderFIN(TCPSegment *segment) {
		if (segment->flags().is(Flags::RST))
			currentState = Closed;
		else if (segment->flags().is(Flags::FIN) && segment->flags().is(Flags::ACK))
			currentState = ReceiverFIN;
		else if (segment->flags().is(Flags::ACK))
			currentState = ReceiverACK;
	}
	
	void handleReceiverACK(TCPSegment *segment) {
		if (segment->flags().is(Flags::RST))
			currentState = Closed;
		else if (segment->flags().is(Flags::FIN))
			currentState = ReceiverFIN;
	}
	
	void handleReceiverFIN(TCPSegment *segment) {
		if (segment->flags().is(Flags::RST) || segment->flags().is(Flags::ACK))
			currentState = Closed;
	}
	
	void printHeader() {
		cout << segment->name();
		if (message)
			cout << " - " << message->name();
		cout << " - " << (isComplete() ? "kompletná" : "nekompletná") << endl;
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
