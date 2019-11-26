#ifndef ContextConfigParser_hpp
#define ContextConfigParser_hpp

#include <exception>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <map>
#include "./PcapParser.hpp"

using namespace std;

class ContextConfigParser {
public:
	class ParsingError : public exception {
	public:
		string message;
		
		ParsingError(string message) : message(message) {}
		const char * what () const throw () {
			return message.c_str();
		}
	};
	
	PcapParser::ContextConfig parse(const string &fileName);
	PcapParser::ContextConfig parse(istream &data);
private:
	bool hasContext;
	PcapParser::Context currentContext;
	regex whitespacePattern = regex(R"(^\s*$)");
	regex contextPattern = regex(R"(^\s*#\s*(Ethernet|LSAP|IP|TCP,UDP)\s*$)");
	regex configPattern = regex(R"(^\s*(\d+)\s*(.*[^\s])\s*$)");
	static const map<string, PcapParser::Context> convertMap;
	
	bool parseLine(PcapParser::ContextConfig &config, const string &line);
	bool parseWhiteSpace(PcapParser::ContextConfig &config, const string &line);
	bool parseContext(PcapParser::ContextConfig &config, const string &line);
	void setContext(const string &context);
	bool parseConfig(PcapParser::ContextConfig &config, const string &line);
};

#endif
