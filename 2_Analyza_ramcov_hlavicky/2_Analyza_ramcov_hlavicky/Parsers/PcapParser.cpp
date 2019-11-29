#include "PcapParser.hpp"

PcapParser::PcapParser(Config config) : config(config) {};

vector<Frame *> PcapParser::parse(const string &fileName) {
	serialNumber = 1;
	pcap_t *file = openPcapFile(fileName);
	
	vector<Frame *> frames;
	int readingResult;
	
	while ((readingResult = pcap_next_ex(file, &parsingHeader, &parsingDataBuffer)) == 1) {
		parsingData = (uint8_t *)(parsingDataBuffer);
		frames.push_back(parseL2Frame());
		serialNumber++;
	}
	
	if (readingResult == PCAP_ERROR) {
		for (Frame *frame : frames)
			delete frame; 
		
		string readingError(pcap_geterr(file));
		throw ParsingError("Packet could not be read\n" + readingError);
	}
	
	return frames;
}

pcap_t *PcapParser::openPcapFile(const string &fileName) {
	char errors[PCAP_ERRBUF_SIZE];
	pcap_t *file = pcap_open_offline(fileName.c_str(), errors);
	
	if (file == NULL)
		throw ParsingError("File " + fileName + " could not be opened\n" + errors);
	
	return file;
}

Frame *PcapParser::parseL2Frame() {
	int frameLength = parseL2FrameLength();
	
	if (frameLength > 1500)
		return parseL2EthernetIIFrame();
	else
		return parseL2Ieee802_3Frame();
}

uint16_t PcapParser::parseL2FrameLength() {
	return ntohs(getField<uint16_t>(12));
}

Frame *PcapParser::parseL2EthernetIIFrame() {
	EthernetIIFrame *frame = new EthernetIIFrame(serialNumber, parsingHeader->len, parsingData, parsingHeader->caplen);
	frame->packet = parseL3Packet(frame);
	
	return frame;
}

Frame *PcapParser::parseL2Ieee802_3Frame() {
	int lsap = parseL2LSAP();
	Frame *frame;
	
	switch (lsap) {
		case 0xFF:
			frame = new Ieee802_3RawFrame(serialNumber, parsingHeader->len, parsingData, parsingHeader->caplen);
			break;
			
		default:
			frame = new Ieee802_3LlcFrame(serialNumber, parsingHeader->len, parsingData, parsingHeader->caplen);
			break;
	}
	
	if (config.has(Config::LSAP, lsap))
		frame->packet = new Packet(NULL, config.get(Config::LSAP, lsap));
	
	return frame;
}

uint8_t PcapParser::parseL2LSAP() {
	return getField<uint8_t>(14);
}

Packet *PcapParser::parseL3Packet(EthernetIIFrame *frame) {
	uint16_t type = frame->type();
	string typeName;
	int typeNumber;
	
	if (config.get(Config::Ethernet, "ipv4", typeName, typeNumber) && type == typeNumber) {
		IPv4Packet *packet = new IPv4Packet(frame->data(), typeName);
		packet->segment = parseL4Segment(packet);
		
		return packet;
	} else if (config.get(Config::Ethernet, "arp", typeName, typeNumber) && type == typeNumber)
		return new ArpPacket(frame->data(), typeName);
	else if (config.has(Config::Ethernet, type))
		return new Packet(NULL, config.get(Config::Ethernet, type));
	
	return NULL;
}

Segment *PcapParser::parseL4Segment(IPv4Packet *packet) {
	uint8_t protocol = packet->protocol();
	string protocolName;
	int protocolNumber;
	
	if (config.get(Config::IP, "tcp", protocolName, protocolNumber) && protocol == protocolNumber) {
		TCPSegment *segment = new TCPSegment(packet->data(), protocolName);
		segment->message = parseL5Message(segment);
		
		return segment;
	} else if (config.get(Config::IP, "udp", protocolName, protocolNumber) && protocol == protocolNumber) {
		UDPSegment *segment = new UDPSegment(packet->data(), protocolName);
		segment->message = parseL5Message(segment);
		
		return segment;
	} else if (config.get(Config::IP, "icmp", protocolName, protocolNumber) && protocol == protocolNumber)
		return new ICMPSegment(packet->data(), protocolName);
	else if (config.has(Config::IP, protocol))
		return new Segment(NULL, config.get(Config::IP, protocol));
	
	return NULL;
}

Message *PcapParser::parseL5Message(TCPSegment *segment) {
	uint16_t source = segment->sourcePort();
	uint16_t destination = segment->destinationPort();
	
	// TODO: implement segment->data()
	if (config.has(Config::TCP, source))
		return new Message(NULL, config.get(Config::TCP, source));
	else if (config.has(Config::TCP, destination))
		return new Message(NULL, config.get(Config::TCP, destination));
	
	return NULL;
}

Message *PcapParser::parseL5Message(UDPSegment *segment) {
	uint16_t source = segment->sourcePort();
	uint16_t destination = segment->destinationPort();
	
	// TODO: implement segment->data()
	if (config.has(Config::UDP, source))
		return new Message(NULL, config.get(Config::UDP, source));
	else if (config.has(Config::UDP, destination))
		return new Message(NULL, config.get(Config::UDP, destination));
	
	return NULL;
}

