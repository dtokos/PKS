#import <XCTest/XCTest.h>
#import "ConfigParser.hpp"

@interface TestConfigParser : XCTestCase

@end

@implementation TestConfigParser

- (void)testNotFoundFileShouldThrow {
	ConfigParser parser;
	XCTAssertThrows(parser.parse("gfdkjgnfd.cfg"));
}

- (void)testParseString {
	string configString = "#Ethernet\n123 foo\n456 bar\n#LSAP\n789 baz";
	istringstream configStream(configString);
	ConfigParser parser;
	PcapParser::ContextConfig config = parser.parse(configStream);
	XCTAssertEqual(config.size(), 2);
	XCTAssertEqual(config[PcapParser::Ethernet].size(), 2);
	XCTAssertEqual(config[PcapParser::Ethernet][123], "foo");
	XCTAssertEqual(config[PcapParser::Ethernet][456], "bar");
	XCTAssertEqual(config[PcapParser::LSAP][789], "baz");
}

- (void)testParseStringWithWhitespaces {
	string configString = "\n\t #\t Ethernet \t\n \t123 \t foo \t\n \t456\t bar";
	istringstream configStream(configString);
	ConfigParser parser;
	PcapParser::ContextConfig config = parser.parse(configStream);
	XCTAssertEqual(config.size(), 1);
	XCTAssertEqual(config[PcapParser::Ethernet].size(), 2);
	XCTAssertEqual(config[PcapParser::Ethernet][123], "foo");
	XCTAssertEqual(config[PcapParser::Ethernet][456], "bar");
}

- (void)testParseEmptyString {
	string configString = "\n\t ";
	istringstream configStream(configString);
	ConfigParser parser;
	XCTAssertNoThrow(parser.parse(configStream));
}

- (void)testParseEmptyInvalidString {
	string configString = "#Etherngfdet\n123 foo\n456 bar\n#LSdsfAP\n789 baz";
	istringstream configStream(configString);
	ConfigParser parser;
	XCTAssertThrows(parser.parse(configStream));
}

- (void)testParseHexString {
	string configString = "#Ethernet\n0xFF foo\n0x3E bar";
	istringstream configStream(configString);
	ConfigParser parser;
	PcapParser::ContextConfig config = parser.parse(configStream);
	XCTAssertEqual(config.size(), 1);
	XCTAssertEqual(config[PcapParser::Ethernet].size(), 2);
	XCTAssertEqual(config[PcapParser::Ethernet][0xFF], "foo");
	XCTAssertEqual(config[PcapParser::Ethernet][0x3E], "bar");
}

@end
