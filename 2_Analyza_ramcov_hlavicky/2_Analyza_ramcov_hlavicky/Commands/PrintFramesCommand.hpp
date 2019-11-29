#ifndef PrintFramesCommand_hpp
#define PrintFramesCommand_hpp

#include "./Command.hpp"

class PrintFramesCommand : public Command {
public:
	~PrintFramesCommand() {}
	
	void process(Frame *frame) {
		Command::process(frame);
		printFrameHeader();
		if (packet)
			printPacket();
		if (segment)
			printSegment();
		if (message)
			printMessage();
		printAllFrameData();
	}
	
	void endProcessing() {}
	
private:
	void printFrameHeader() {
		cout << dec
			<< "rámec " << frame->serialNumber << endl
			<< "dĺžka rámca poskytnutá pcap API  – " << frame->pcapLength << " B" << endl
			<< "dĺžka rámca prenášaného po médiu – " << frame->wireLength() << " B" << endl
			<< frame->name() << endl
			<< "Zdrojová MAC adresa: " << frame->source().asString(' ') << endl
			<< "Cieľová MAC adresa:  " << frame->destination().asString(' ') << endl;
	}
	
	void printAllFrameData() {
		uint8_t *byte = frame->raw;
		int i;
		
		for (i = 0; i < frame->capturedLength; i++, byte++) {
			cout << setfill('0') << setw(2) << hex << static_cast<int>(*byte) << " ";
			
			if (i == 0)
				continue;
			if (i % 16 == 15)
				cout << endl;
			else if (i % 8 == 7)
				cout << "\t";
		}
		
		cout << endl;
		
		if (i % 16 != 0)
			cout << endl;
	}
	
	void printPacket() {
		cout << packet->name() << endl;
		
		if (packet->packetType() == Packet::IPv4)
			printIPv4Packet((IPv4Packet *)packet);
	}
	
	void printIPv4Packet(IPv4Packet *packet) {
		cout << "zdrojová IP adresa: " << packet->sourceAddress().asString() << endl
			<< "cieľová IP adresa:  " << packet->destinationAddress().asString() << endl;
	}
	
	void printSegment() {
		cout << segment->name() << endl;
		
		Segment::Type type = segment->segmentType();
		
		if (type == Segment::TCP)
			printTCPSegment((TCPSegment *)segment);
		else if (type == Segment::UDP)
			printUDPSegment((UDPSegment *)segment);
	}
	
	void printTCPSegment(TCPSegment *segment) {
		printPorts(segment->sourcePort(), segment->destinationPort());
	}
	
	void printUDPSegment(UDPSegment *segment) {
		printPorts(segment->sourcePort(), segment->destinationPort());
	}
	
	void printPorts(int source, int destination) {
		cout << "zdrojový port: " << source << endl
			<< "cieľový port:  " << destination << endl;
	}
	
	void printMessage() {
		cout << message->name() << endl;
	}
};

#endif
