#include "ConfigParser.hpp"

const map<string, PcapParser::Config::Context> ConfigParser::convertMap = {
	{"Ethernet", PcapParser::Config::Ethernet},
	{"LSAP", PcapParser::Config::LSAP},
	{"IP", PcapParser::Config::IP},
	{"TCP", PcapParser::Config::TCP},
	{"UDP", PcapParser::Config::UDP},
};

const map<PcapParser::Config::Context, vector<string>> ConfigParser::requiredProtocols {
	{PcapParser::Config::Ethernet, {"ipv4", "arp"}},
	{PcapParser::Config::IP, {"icmp", "tcp", "udp"}},
	{PcapParser::Config::TCP, {"http", "https", "telnet", "ssh", "ftp"}},
	{PcapParser::Config::UDP, {"tftp"}},
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
	
	//if (!verifyRequiredProtocols(config))
		//throw ParsingError("Could not parse all required protocols: IPv4, ARP, HTTP, HTTPS, TELNET, SSH, FTP, TFTP, ICMP");
	
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

bool ConfigParser::verifyRequiredProtocols(PcapParser::Config &config) {
	for (const auto &pair : requiredProtocols)
		for (string protocolName : requiredProtocols.at(pair.first))
			if (!config.has(pair.first, protocolName))
				return false;
	
	return true;
}
