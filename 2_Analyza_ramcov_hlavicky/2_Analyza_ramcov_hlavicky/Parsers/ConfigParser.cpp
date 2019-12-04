#include "ConfigParser.hpp"

const map<string, PcapParser::Config::Context> ConfigParser::convertMap = {
	{"Ethernet", PcapParser::Config::Ethernet},
	{"LSAP", PcapParser::Config::LSAP},
	{"IP", PcapParser::Config::IP},
	{"TCP", PcapParser::Config::TCP},
	{"UDP", PcapParser::Config::UDP},
};

PcapParser::Config ConfigParser::parse(const string &fileName) {
	ifstream file(fileName);
	if (!file.is_open())
		throw ParsingError("File " + fileName + " could not be opened");
	
	return parse(file);
}

PcapParser::Config ConfigParser::parse(istream &data) {
	PcapParser::Config config;
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

bool ConfigParser::parseLine(PcapParser::Config &config, const string &line) {
	return (
		parseWhiteSpace(config, line) ||
		parseContext(config, line) ||
		(hasContext && parseConfig(config, line))
	);
}

bool ConfigParser::parseWhiteSpace(PcapParser::Config &config, const string &line) {
	return regex_match(line, whitespacePattern);
}

bool ConfigParser::parseContext(PcapParser::Config &config, const string &line) {
	smatch match;
	
	if (!regex_match(line, match, contextPattern))
		return false;
		
	currentContext = convertMap.at(match.str(1));
	hasContext = true;
	
	return true;
}

bool ConfigParser::parseConfig(PcapParser::Config &config, const string &line) {
	smatch match;
	
	if (!regex_match(line, match, configPattern))
		return false;
	
	config.add(currentContext, parseConfigNumber(match.str(1)), match.str(2));
	
	return true;
}

int ConfigParser::parseConfigNumber(const string &strNumber) {
	size_t hexIndex = strNumber.rfind("0x", 0);
	if (hexIndex == 0)
		return stoi(strNumber.substr(2), 0, 16);
	
	return stoi(strNumber);
}

