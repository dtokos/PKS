#import <XCTest/XCTest.h>
#import "ConfigParser.hpp"

@interface TestConfigParser : XCTestCase

@end

@implementation TestConfigParser

- (void)testNotFoundFileShouldThrow {
	ConfigParser parser;
	XCTAssertThrows(parser.parse("gfdkjgnfd.cfg"));
}

- (void)testParseWithoutRequiredProtocolsShouldThrow {
	string configString = "#Ethernet\n123 foo\n456 bar\n#LSAP\n789 baz";
	istringstream configStream(configString);
	ConfigParser parser;
	XCTAssertThrows(parser.parse(configStream));
}

- (void)testParseString {
	string configString = "# Ethernet\n"
	"0x0800 IPv4\n"
	"0x0806 ARP\n"
	"0x08dd IPv6\n"
	
	"#LSAP\n"
	"0x42 STP\n"
	"0xaa SNAP\n"
	"0xe0 IPX\n"
	
	"# IP\n"
	"0x01 ICMP\n"
	"0x06 TCP\n"
	"0x11 UDP\n"
	
	"# TCP\n"
	"80 HTTP\n"
	"443 HTTPS\n"
	"23 TELNET\n"
	"22 SSH\n"
	"21 FTP\n"
	
	"# UDP\n"
	"53 DNS\n"
	"69 TFTP\n"
	"\t 123 \t foo \t\n";
	
	istringstream configStream(configString);
	ConfigParser parser;
	PcapParser::Config config;
	XCTAssertNoThrow((config = parser.parse(configStream)));
	XCTAssertTrue(config.has(PcapParser::Config::Ethernet, 0x0800));
	XCTAssertTrue(config.has(PcapParser::Config::Ethernet, "ipv4"));
	XCTAssertTrue(config.has(PcapParser::Config::TCP, 80));
	XCTAssertTrue(config.has(PcapParser::Config::TCP, "http"));
	XCTAssertTrue(config.has(PcapParser::Config::UDP, 123));
	XCTAssertTrue(config.has(PcapParser::Config::UDP, "foo"));
}


@end
