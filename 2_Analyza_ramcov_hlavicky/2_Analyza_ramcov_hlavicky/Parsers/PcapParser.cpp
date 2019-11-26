#include "PcapParser.hpp"

PcapParser::PcapParser(ContextConfig config) : config(config) {};

vector<Frame *> PcapParser::parse(const string &fileName) {
	serialNumber = 1;
	pcap_t *file = openPcapFile(fileName);
	
	vector<Frame *> frames;
	int readingResult;
	
	while ((readingResult = pcap_next_ex(file, &parsingHeader, &parsingDataBuffer)) == 1) {
		parsingData = (uint8_t *)(parsingDataBuffer);
		frames.push_back(parseFrame());
		serialNumber++;
	}
	
	if (readingResult == PCAP_ERROR) {
		string readingError(pcap_geterr(file));
		throw ParsingError("Packet could not be red\n" + readingError);
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

Frame *PcapParser::parseFrame() {
	if (parseFrameLength() > 1500)
		return new EthernetIIFrame(serialNumber, parsingHeader->len, parsingData, parsingHeader->caplen);
	else
		return parseIeee802_3Frame();
}

uint16_t PcapParser::parseFrameLength() {
	return ntohs(*(uint16_t *)(parsingDataBuffer + 12));
}

Frame *PcapParser::parseIeee802_3Frame() {
	switch (parseDSAP()) {
		case 0xFF:
			return new Ieee802_3RawFrame(serialNumber, parsingHeader->len, (uint8_t *)parsingDataBuffer, parsingHeader->caplen);
			
		case 0xAA:
			return new Ieee802_3LlcSnapFrame(serialNumber, parsingHeader->len, (uint8_t *)parsingDataBuffer, parsingHeader->caplen);
			
		default:
			return new Ieee802_3LlcFrame(serialNumber, parsingHeader->len, (uint8_t *)parsingDataBuffer, parsingHeader->caplen);
	}
}

uint8_t PcapParser::parseDSAP() {
	return *(parsingDataBuffer + 13);
}
