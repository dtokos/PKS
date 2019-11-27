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
	class Config {
	public:
		enum Context {Ethernet, LSAP, IP, TCP, UDP};
		
		void add(const Context &context, const int &number, const string &name) {
			byNumber[context][number] = name;
			byName[context][lowercase(name)] = number;
		}
		
		bool has(const Context &context, const int &number) {
			return byNumber.count(context) && byNumber.at(context).count(number);
		}
		
		bool has(const Context &context, const string &name) {
			return byName.count(context) && byName.at(context).count(lowercase(name));
		}
		
		string get(const Context &context, const int &number) {return byNumber.at(context).at(number);};
		int get(const Context &context, const string &name) {return byName.at(context).at(name);};
		
	private:
		map<Context, map<int, string>> byNumber;
		map<Context, map<string, int>> byName;
		
		string lowercase(const string& name) {
			string result(name.size(), ' ');
			transform(name.begin(), name.end(), result.begin(), ::tolower);
			
			return result;
		}
	};
	
	class ParsingError : public exception {
	public:
		string message;
		
		ParsingError(string message) : message(message) {}
		const char * what () const throw () {
			return message.c_str();
		}
	};
	
	PcapParser(Config config);
	vector<Frame *> parse(const string &fileName);
	
private:
	Config config;
	pcap_pkthdr *parsingHeader;
	const u_char *parsingDataBuffer;
	unsigned serialNumber;
	uint8_t *parsingData;
	Frame *parsingFrame;
	
	
	pcap_t *openPcapFile(const string &fileName);
	
	Frame   *parseL2Frame();
	uint16_t parseL2FrameLength();
	Frame   *parseL2EthernetIIFrame();
	Frame   *parseL2Ieee802_3Frame();
	uint8_t  parseL2DSAP();
	
	Packet *parseL3Packet(EthernetIIFrame *frame);
	
	Segment *parseL4Segment(IPv4Packet *packet);
	
	Message *parseL5Message(TCPSegment *segment);
	Message *parseL5Message(UDPSegment *segment);
	
	template<typename Type>
	constexpr Type getField(uint32_t offset) {
		return *reinterpret_cast<Type *>(parsingData + offset);
	}
};

#endif
