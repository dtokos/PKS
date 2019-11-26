#ifndef PcapParser_hpp
#define PcapParser_hpp

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <pcap.h>
#include "../Network/Network.hpp"

using namespace std;

class PcapParser {
public:
	enum Context {Ethernet, LSAP, IP, TCP, UDP};
	
	using Config = map<int, string>;
	using ContextConfig = map<Context, Config>;
	
	class ParsingError : public exception {
	public:
		string message;
		
		ParsingError(string message) : message(message) {}
		const char * what () const throw () {
			return message.c_str();
		}
	};
	
	PcapParser(Config l2Config);
	vector<Frame *> parse(const string &fileName);
	
private:
	Config l2Config;
	pcap_pkthdr *parsingHeader;
	const u_char *parsingDataBuffer;
	unsigned serialNumber;
	uint8_t *parsingData;
	
	
	pcap_t *openPcapFile(const string &fileName);
	Frame *parseFrame();
	uint16_t parseFrameLength();
	Frame *parseIeee802_3Frame();
	uint8_t parseDSAP();
};

#endif
