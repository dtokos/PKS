#include <iostream>
#include "./Parsers/ConfigParser.hpp"
#include "./App/App.hpp"

int main(int argc, const char * argv[]) {
	if (argc < 2) {
		cerr << "No file specified" << endl;
		return 0;
	}
	
	ConfigParser configParser;
	try {
		PcapParser::ContextConfig config = configParser.parse("config.cfg");
		PcapParser parser(config);
		App app(parser);
		
		app.run(argv[1]);
	} catch (ConfigParser::ParsingError &error) {
		cerr << error.what() << endl;
	} catch (PcapParser::ParsingError &error) {
		cerr << error.what() << endl;
	}
	
	return 0;
}
