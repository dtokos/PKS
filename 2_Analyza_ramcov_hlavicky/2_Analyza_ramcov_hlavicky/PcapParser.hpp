#ifndef PcapParser_hpp
#define PcapParser_hpp

#include <string>
#include <map>
#include <pcap.h>

using namespace std;
using Config = map<int, string>;

class PcapParser {
public:
	class ParsingError : public exception {
	public:
		string message;
		
		ParsingError(string message) : message(message) {}
		const char * what () const throw () {
			return message.c_str();
		}
	};
	
	PcapParser(Config l2Config);
	void parse(const string &fileName);
	
private:
	Config l2Config;
};

#endif
