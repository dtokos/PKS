#include <iostream>
#include "./ConfigParser.hpp"
#include "./PcapParser.hpp"

int main(int argc, const char * argv[]) {
	if (argc < 2) {
		cerr << "No file specified" << endl;
		return 0;
	}
	
	ConfigParser configParser;
	try {
		Config l2config = configParser.parse("l2.cfg");
		cout << "L2CFG: " << l2config[123] << " " << l2config[421] << endl;
		
		PcapParser parser(l2config);
		parser.parse(argv[1]);
	} catch (ConfigParser::ParsingError &error) {
		cerr << error.what() << endl;
	} catch (PcapParser::ParsingError &error) {
		cerr << error.what() << endl;
	}
	return 0;
}
