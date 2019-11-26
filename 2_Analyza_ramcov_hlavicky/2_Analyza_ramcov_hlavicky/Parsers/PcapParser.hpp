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
	
	PcapParser(ContextConfig config);
	vector<Frame *> parse(const string &fileName);
	
private:
	ContextConfig config;
	pcap_pkthdr *parsingHeader;
	const u_char *parsingDataBuffer;
	unsigned serialNumber;
	uint8_t *parsingData;
	
	
	pcap_t *openPcapFile(const string &fileName);
	Frame *parseFrame();
	uint16_t parseFrameLength();
	Frame *parseIeee802_3Frame();
	uint8_t parseDSAP();
	
	template<typename Type>
	constexpr Type getField(uint32_t offset) {
		return *reinterpret_cast<Type *>(parsingData + offset);
	}
};

#endif
