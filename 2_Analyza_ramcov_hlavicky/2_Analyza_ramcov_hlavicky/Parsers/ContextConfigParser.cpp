#include "ContextConfigParser.hpp"

const map<string, PcapParser::Context> ContextConfigParser::convertMap = {
	{"Ethernet", PcapParser::Ethernet},
	{"LSAP", PcapParser::LSAP},
	{"IP", PcapParser::IP},
	{"TCP", PcapParser::TCP},
	{"UDP", PcapParser::UDP},
};

PcapParser::ContextConfig ContextConfigParser::parse(const string &fileName) {
	ifstream file(fileName);
	if (!file.is_open())
		throw ParsingError("File " + fileName + " could not be opened");
	
	return parse(file);
}

PcapParser::ContextConfig ContextConfigParser::parse(istream &data) {
	PcapParser::ContextConfig config;
	hasContext = false;
	
	string line;
	int lineNumber = 1;
	while (getline(data, line)) {
		if (!parseLine(config, line))
			throw ParsingError("Unexpected data at line: " + to_string(lineNumber));
		
		lineNumber++;
	}
	
	return config;
}

bool ContextConfigParser::parseLine(PcapParser::ContextConfig &config, const string &line) {
	return (
		parseWhiteSpace(config, line) ||
		parseContext(config, line) ||
		(hasContext && parseConfig(config, line))
	);
}

bool ContextConfigParser::parseWhiteSpace(PcapParser::ContextConfig &config, const string &line) {
	return regex_match(line, whitespacePattern);
}

bool ContextConfigParser::parseContext(PcapParser::ContextConfig &config, const string &line) {
	smatch match;
	
	if (!regex_match(line, match, contextPattern))
		return false;
		
	//setContext(match.str(1));
	currentContext = convertMap.at(match.str(1));
	hasContext = true;
	
	return true;
}

void ContextConfigParser::setContext(const string &context) {
	if (context == "Ethernet")
		currentContext = PcapParser::Ethernet;
	else if (context == "LSAP")
		currentContext = PcapParser::LSAP;
	else if (context == "IP")
		currentContext = PcapParser::IP;
	else if (context == "TCP")
		currentContext = PcapParser::TCP;
	else if (context == "UDP")
		currentContext = PcapParser::UDP;
}

bool ContextConfigParser::parseConfig(PcapParser::ContextConfig &config, const string &line) {
	smatch match;
	
	if (!regex_match(line, match, configPattern))
		return false;
	
	config[currentContext][stoi(match.str(1))] = match.str(2);
	
	return true;
}
