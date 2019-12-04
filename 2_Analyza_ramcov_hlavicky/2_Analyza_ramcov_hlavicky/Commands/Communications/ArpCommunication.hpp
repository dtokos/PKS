#ifndef ArpCommunication_hpp
#define ArpCommunication_hpp

#include "./Communication.hpp"

class ArpCommunication : public Communication {
public:
	ArpCommunication(Frame *frame) :
		frames({frame}),
		packet((ArpPacket *)frame->packet) {}
	
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
	ArpPacket *packet;
	
	bool belongsToThisCommunication(Frame *frame) {
		if (frame->packet == NULL || frame->packet->packetType() != Packet::ARP)
			return false;
		
		ArpPacket *packet = (ArpPacket *)frame->packet;
		
		if (packet->opcode() == ArpPacket::Request || (this->packet->opcode() == ArpPacket::Response && this->packet->opcode() == packet->opcode()))
			return this->packet->senderMACAddress() == packet->senderMACAddress() &&
				this->packet->senderIPAddress() == packet->senderIPAddress() &&
				this->packet->targetIPAddress() == packet->targetIPAddress();
		else if (packet->opcode() == ArpPacket::Response) {
			return this->packet->senderMACAddress() == packet->targetMACAddress() &&
				this->packet->senderIPAddress() == packet->targetIPAddress() &&
				this->packet->targetIPAddress() == packet->senderIPAddress();
		}
		return false;
	}
	
	void printHeader() {
		cout << packet->name() << endl;
		Print::arpPacket(packet);
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
