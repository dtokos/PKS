#include "ConfigParser.hpp"

const map<string, PcapParser::Context> ConfigParser::convertMap = {
	{"Ethernet", PcapParser::Ethernet},
	{"LSAP", PcapParser::LSAP},
	{"IP", PcapParser::IP},
	{"TCP", PcapParser::TCP},
	{"UDP", PcapParser::UDP},
};

PcapParser::ContextConfig ConfigParser::parse(const string &fileName) {
	ifstream file(fileName);
	if (!file.is_open())
		throw ParsingError("File " + fileName + " could not be opened");
	
	return parse(file);
}

PcapParser::ContextConfig ConfigParser::parse(istream &data) {
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

bool ConfigParser::parseLine(PcapParser::ContextConfig &config, const string &line) {
	return (
		parseWhiteSpace(config, line) ||
		parseContext(config, line) ||
		(hasContext && parseConfig(config, line))
	);
}

bool ConfigParser::parseWhiteSpace(PcapParser::ContextConfig &config, const string &line) {
	return regex_match(line, whitespacePattern);
}

bool ConfigParser::parseContext(PcapParser::ContextConfig &config, const string &line) {
	smatch match;
	
	if (!regex_match(line, match, contextPattern))
		return false;
		
	currentContext = convertMap.at(match.str(1));
	hasContext = true;
	
	return true;
}

bool ConfigParser::parseConfig(PcapParser::ContextConfig &config, const string &line) {
	smatch match;
	
	if (!regex_match(line, match, configPattern))
		return false;
	
	config[currentContext][parseConfigNumber(match.str(1))] = match.str(2);
	
	return true;
}

int ConfigParser::parseConfigNumber(const string &strNumber) {
	size_t hexIndex = strNumber.rfind("0x", 0);
	if (hexIndex == 0)
		return stoi(strNumber.substr(2), 0, 16);
	
	return stoi(strNumber);
}
