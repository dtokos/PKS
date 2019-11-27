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
	int dsap = parseL2DSAP();
	Frame *frame;
	
	switch (dsap) {
		case 0xFF:
			frame = new Ieee802_3RawFrame(serialNumber, parsingHeader->len, parsingData, parsingHeader->caplen);
			break;
			
		case 0xAA:
			frame = new Ieee802_3LlcSnapFrame(serialNumber, parsingHeader->len, parsingData, parsingHeader->caplen);
			break;
			
		default:
			frame = new Ieee802_3LlcFrame(serialNumber, parsingHeader->len, parsingData, parsingHeader->caplen);
			break;
	}
	
	if (config.has(Config::LSAP, dsap))
		frame->packet = new OtherPacket(NULL, config.get(Config::LSAP, dsap));
	
	return frame;
}

uint8_t PcapParser::parseL2DSAP() {
	return getField<uint8_t>(13);
}

Packet *PcapParser::parseL3Packet(EthernetIIFrame *frame) {
	int ipv4 = config.get(Config::Ethernet, "ipv4");
	int arp = config.get(Config::Ethernet, "arp");
	uint16_t type = frame->type();
	
	if (type == ipv4) {
		IPv4Packet *packet = new IPv4Packet(frame->data(), config.get(Config::Ethernet, ipv4));
		packet->segment = parseL4Segment(packet);
		
		return packet;
	} else if (type == config.get(Config::Ethernet, "arp"))
		return new ArpPacket(frame->data(), config.get(Config::Ethernet, arp));
	else if (config.has(Config::Ethernet, type))
		return new OtherPacket(NULL, config.get(Config::Ethernet, type));
	
	return NULL;
}

Segment *PcapParser::parseL4Segment(IPv4Packet *packet) {
	uint8_t protocol = packet->protocol();
	int tcp = config.get(Config::IP, "tcp");
	int udp = config.get(Config::IP, "udp");
	int icmp = config.get(Config::IP, "icmp");
	
	if (protocol == tcp) {
		TCPSegment *segment = new TCPSegment(packet->data(), config.get(Config::IP, tcp));
		segment->message = parseL5Message(segment);
		
		return segment;
	} else if (protocol == udp) {
		UDPSegment *segment = new UDPSegment(packet->data(), config.get(Config::IP, udp));
		segment->message = parseL5Message(segment);
		
		return segment;
	} else if (protocol == icmp)
		return new ICMPSegment(packet->data(), config.get(Config::IP, icmp));
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

