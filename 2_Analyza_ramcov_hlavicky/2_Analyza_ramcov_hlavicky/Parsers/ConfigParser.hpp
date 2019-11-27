#ifndef ConfigParser_hpp
#define ConfigParser_hpp

#include <exception>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <map>
#include "./PcapParser.hpp"

using namespace std;

class ConfigParser {
public:
	class ParsingError : public exception {
	public:
		string message;
		
		ParsingError(string message) : message(message) {}
		const char * what () const throw () {
			return message.c_str();
		}
	};
	
	PcapParser::Config parse(const string &fileName);
	PcapParser::Config parse(istream &data);
private:
	bool hasContext;
	PcapParser::Config::Context currentContext;
	regex whitespacePattern = regex(R"(^\s*$)");
	regex contextPattern = regex(R"(^\s*#\s*(Ethernet|LSAP|IP|TCP|UDP)\s*$)");
	regex configPattern = regex(R"(^\s*(0x[a-fA-F0-9]+|\d+)\s+(.*[^\s])\s*$)");
	static const map<string, PcapParser::Config::Context> convertMap;
	static const map<PcapParser::Config::Context, vector<string>> requiredProtocols;
	
	bool parseLine(PcapParser::Config &config, const string &line);
	bool parseWhiteSpace(PcapParser::Config &config, const string &line);
	bool parseContext(PcapParser::Config &config, const string &line);
	bool parseConfig(PcapParser::Config &config, const string &line);
	int parseConfigNumber(const string &strNumber);
	bool verifyRequiredProtocols(PcapParser::Config &config);
};

#endif
