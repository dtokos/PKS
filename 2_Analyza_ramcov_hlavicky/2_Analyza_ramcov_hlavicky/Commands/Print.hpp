#ifndef Print_hpp
#define Print_hpp

#include "../Network/Frames/Frame.hpp"

class Print {
public:
	static void frame(Frame *frame) {
		prepare(frame);
		frameHeader();
		if (_packet)
			packet();
		if (_segment)
			segment();
		if (_message)
			message();
		allFrameData();
	}
	
	static void ipv4Packet(IPv4Packet *packet) {
		cout << "zdrojová IP adresa: " << packet->sourceAddress().asString() << endl
			<< "cieľová IP adresa:  " << packet->destinationAddress().asString() << endl;
	}
	
	static void arpPacket(ArpPacket *packet) {
		if (packet->opcode() == ArpPacket::Request)
			cout << "Request" << endl;
		else if (packet->opcode() == ArpPacket::Response)
			cout << "Response" << endl;
		
		cout << "zdrojová IP adresa: " << packet->senderIPAddress().asString() << endl
		<< "cieľová IP adresa:  " << packet->targetIPAddress().asString() << endl;
	}
	
	static void segment(TCPSegment *segment) {
		ports(segment->sourcePort(), segment->destinationPort());
	}
	
	static void segment(UDPSegment *segment) {
		ports(segment->sourcePort(), segment->destinationPort());
	}
	
	static void segment(ICMPSegment *segment) {
		cout << icmpType(segment) << endl;
	}
	
private:
	static Frame *_frame;
	static Packet *_packet;
	static Segment *_segment;
	static Message *_message;
	
	static void prepare(Frame *frame) {
		_frame = frame;
		_packet = _frame->packet;
		_segment = _packet == NULL ? NULL : _packet->segment;
		_message = _segment == NULL ? NULL : _segment->message;
	}
	
	static void frameHeader() {
		cout << dec
			<< "rámec " << _frame->serialNumber << endl
			<< "dĺžka rámca poskytnutá pcap API  – " << _frame->pcapLength << " B" << endl
			<< "dĺžka rámca prenášaného po médiu – " << _frame->wireLength() << " B" << endl
			<< _frame->name() << endl
			<< "Zdrojová MAC adresa: " << _frame->source().asString(' ') << endl
			<< "Cieľová MAC adresa:  " << _frame->destination().asString(' ') << endl;
	}
	
	static void allFrameData() {
		uint8_t *byte = _frame->raw;
		int i;
		
		for (i = 0; i < _frame->capturedLength; i++, byte++) {
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
	
	static void packet() {
		cout << _packet->name() << endl;
		
		if (_packet->packetType() == Packet::IPv4)
			ipv4Packet((IPv4Packet *)_packet);
		else if (_packet->packetType() == Packet::ARP)
			arpPacket((ArpPacket *)_packet);
	}
	
	static void segment() {
		cout << _segment->name() << endl;
		
		Segment::Type type = _segment->segmentType();
		
		if (type == Segment::TCP)
			segment((TCPSegment *)_segment);
		else if (type == Segment::UDP)
			segment((UDPSegment *)_segment);
		else if (type == Segment::ICMP)
			segment((ICMPSegment *)_segment);
	}
	
	static void ports(int source, int destination) {
		cout << "zdrojový port: " << source << endl
			<< "cieľový port:  " << destination << endl;
	}
	
	static string icmpType(ICMPSegment *segment) {
		switch (segment->type()) {
			case ICMPSegment::EchoReply:
				return "Echo Reply";
			case ICMPSegment::DestinationUnreachable:
				return "Destination Unreachable";
			case ICMPSegment::SourceQuench:
				return "Source Quench";
			case ICMPSegment::Redirect:
				return "Redirect";
			case ICMPSegment::EchoRequest:
				return "Echo Request";
			case ICMPSegment::RouterAdvertisement:
				return "Router Advertisement";
			case ICMPSegment::RouterSelection:
				return "Router Selection";
			case ICMPSegment::TimeExceeded:
				return "Time Exceeded";
			case ICMPSegment::ParameterProblem:
				return "Parameter Problem";
			case ICMPSegment::Timestamp:
				return "Timestamp";
			case ICMPSegment::TimestampReply:
				return "Timestamp Reply";
			case ICMPSegment::InformationRequest:
				return "Information Request";
			case ICMPSegment::InformationReply:
				return "Information Reply";
			case ICMPSegment::AddressMaskRequest:
				return "Address Mask Request";
			case ICMPSegment::AddressMaskReply:
				return "Address Mask Reply";
			case ICMPSegment::Traceroute:
				return "Traceroute";
			default:
				return "Unknown";
		}
	}
	
	static void message() {
		cout << _message->name() << endl;
	}
	
};

Frame *Print::_frame;
Packet *Print::_packet;
Segment *Print::_segment;
Message *Print::_message;

#endif
