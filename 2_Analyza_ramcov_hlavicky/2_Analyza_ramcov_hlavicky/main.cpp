#include <iostream>
#include "./Parsers/Parsers.hpp"
#include "./Commands/Commands.hpp"
#include "./App/App.hpp"

int printUsage(const string &appName);
int printConfigParsingError(const ConfigParser::ParsingError &error);
int printPcapParsingError(const PcapParser::ParsingError &error, const string &inputFile);

int main(int argc, const char * argv[]) {
	if (argc < 2)
		return printUsage(argv[0]);

	string inputFile(argv[1]);
	
	try {
		ConfigParser configParser;
		PcapParser::Config config = configParser.parse("config.cfg");
		PcapParser parser(config);
		App app(parser, {new PrintFramesCommand(), new TopIPsCommand(), new CommunicationCommand(20)});
		
		app.run(inputFile);
	} catch (ConfigParser::ParsingError &error) {
		return printConfigParsingError(error);
	} catch (PcapParser::ParsingError &error) {
		return printPcapParsingError(error, inputFile);
	}
	
	return EXIT_SUCCESS;
}

int printUsage(const string &appName) {
	cerr << "No pcap file specified" << endl
		<< "Usage: " << appName << " [input file]" << endl;
	
	return EXIT_FAILURE;
}

int printConfigParsingError(const ConfigParser::ParsingError &error) {
	cerr << "Could not parse config" << endl
		<< error.what() << endl;
	
	return EXIT_FAILURE;
}

int printPcapParsingError(const PcapParser::ParsingError &error, const string &inputFile) {
	cerr << "Could not parse pcap file: " << inputFile << endl
		<< error.what() << endl;
	
	return EXIT_FAILURE;
}
