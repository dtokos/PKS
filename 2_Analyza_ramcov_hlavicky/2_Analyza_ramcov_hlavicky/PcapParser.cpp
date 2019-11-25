#include "PcapParser.hpp"

PcapParser::PcapParser(Config l2Config) : l2Config(l2Config) {};

void PcapParser::parse(const string &fileName) {
	char errors[PCAP_ERRBUF_SIZE];
	pcap_t* file = pcap_open_offline(fileName.c_str(), errors);
	
	if (file == NULL)
		throw ParsingError("File " + fileName + " could not be opened");
}
