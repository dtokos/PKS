#import <XCTest/XCTest.h>
#include "PcapParser.hpp"

@interface TestConfig : XCTestCase

@end

@implementation TestConfig

- (void)testEmptyConfig {
	PcapParser::Config config;
	XCTAssertFalse(config.has(PcapParser::Config::Ethernet, 123));
	XCTAssertFalse(config.has(PcapParser::Config::Ethernet, "foo"));
}

- (void)testAdd {
	PcapParser::Config config;
	config.add(PcapParser::Config::Ethernet, 123, "foo");
	XCTAssertTrue(config.has(PcapParser::Config::Ethernet, 123));
	XCTAssertTrue(config.has(PcapParser::Config::Ethernet, "foo"));
	XCTAssertTrue(config.get(PcapParser::Config::Ethernet, 123) == "foo");
	XCTAssertEqual(config.get(PcapParser::Config::Ethernet, "foo"), 123);
}

- (void)testCaseInsensitive {
	PcapParser::Config config;
	config.add(PcapParser::Config::Ethernet, 123, "FoO");
	XCTAssertTrue(config.has(PcapParser::Config::Ethernet, 123));
	XCTAssertTrue(config.has(PcapParser::Config::Ethernet, "foo"));
	XCTAssertTrue(config.get(PcapParser::Config::Ethernet, 123) == "FoO");
	XCTAssertEqual(config.get(PcapParser::Config::Ethernet, "foo"), 123);
}

@end
