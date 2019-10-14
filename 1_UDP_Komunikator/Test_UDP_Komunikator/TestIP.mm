#import <XCTest/XCTest.h>
#include "IP.hpp"

@interface TestIP : XCTestCase

@end

@implementation TestIP

-(void)testInvalidIpShouldThrow {
	XCTAssertThrows(IP::fromString("gjsfk"));
}

-(void)testOutOfRangeIpShouldThrow {
	XCTAssertThrows(IP::fromString("192.168.0.256"));
}

-(void)testValidIPShouldNotThrow {
	XCTAssertNoThrow(IP::fromString("192.168.0.1"));
}

@end
