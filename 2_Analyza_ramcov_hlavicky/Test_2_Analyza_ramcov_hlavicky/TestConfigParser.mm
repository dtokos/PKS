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
	string configString = "123 foo\ninvalid line\n456 bar";
	istringstream configStream(configString);
	ConfigParser parser;
	map<int, string> config = parser.parse(configStream);
	XCTAssertEqual(config.size(), 2);
	XCTAssertEqual(config[123], "foo");
	XCTAssertEqual(config[456], "bar");
}

@end
