#include "ConfigParser.hpp"

map<int, string> ConfigParser::parse(const string &fileName) {
	ifstream file(fileName);
	if (!file.is_open())
		throw ParsingError("File " + fileName + " could not be opened");
	
	return parse(file);
}

map<int, string> ConfigParser::parse(istream &data) {
	map<int, string> config;
	
	string line;
	while (getline(data, line)) {
		stringstream lineStream(line);
		int number;
		string name;
		
		if (!(lineStream >> number >> name))
			continue;
		
		config[number] = name;
	}
	
	return config;
}
