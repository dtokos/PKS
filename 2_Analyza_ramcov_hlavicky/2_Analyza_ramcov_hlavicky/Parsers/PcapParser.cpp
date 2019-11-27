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
		parsingFrame = new EthernetIIFrame(serialNumber, parsingHeader->len, parsingData, parsingHeader->caplen);
	else
		parsingFrame = parseL2Ieee802_3Frame();
	
	parsingFrame->packet = parseL3Packet();
	
	return parsingFrame;
}

uint16_t PcapParser::parseL2FrameLength() {
	return ntohs(getField<uint16_t>(12));
}

Frame *PcapParser::parseL2Ieee802_3Frame() {
	int dsap = parseL2DSAP();
	
	switch (dsap) {
		case 0xFF:
			return new Ieee802_3RawFrame(serialNumber, parsingHeader->len, parsingData, parsingHeader->caplen);
			
		case 0xAA:
			return new Ieee802_3LlcSnapFrame(serialNumber, parsingHeader->len, parsingData, parsingHeader->caplen);
			
		default:
			return new Ieee802_3LlcFrame(serialNumber, parsingHeader->len, parsingData, parsingHeader->caplen);
	}
}

uint8_t PcapParser::parseL2DSAP() {
	return getField<uint8_t>(13);
}

Packet *PcapParser::parseL3Packet() {
	int dsap = parseL2DSAP();
	if (parsingFrame->frameType() != Frame::EthernetII && config.has(Config::LSAP, dsap))
		return new OtherPacket(NULL, config.get(Config::LSAP, dsap));
	
	EthernetIIFrame *frame = (EthernetIIFrame *)parsingFrame;
	int ipv4 = config.get(Config::Ethernet, "ipv4");
	int arp = config.get(Config::Ethernet, "arp");
	uint16_t type = frame->type();
	
	if (type == ipv4)
		return new IPv4Packet(frame->data(), config.get(Config::Ethernet, ipv4));
	else if (type == config.get(Config::Ethernet, "arp"))
		return new ArpPacket(frame->data(), config.get(Config::Ethernet, arp));
	else if (config.has(Config::Ethernet, type))
		return new OtherPacket(NULL, config.get(Config::Ethernet, type));
	
	return NULL;
}

