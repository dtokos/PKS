#import <XCTest/XCTest.h>
#include "Port.hpp"

@interface TestPort : XCTestCase

@end

@implementation TestPort

- (void)testInvalidPortShouldThrowException {
	XCTAssertThrows(Port::fromNumber(1), "Should throw. Valid range is 1024-65535.");
}

- (void)testValidPortShouldNotThrow {
	XCTAssertNoThrow(Port::fromNumber(1024), "Should not throw. Is within valid range.");
	XCTAssertNoThrow(Port::fromNumber(1234), "Should not throw. Is within valid range.");
	XCTAssertNoThrow(Port::fromNumber(65535), "Should not throw. Is within valid range.");
}

-(void)testPortReturnsValitPortNumber {
	int number = 1234;
	Port p = Port::fromNumber(number);
	
	XCTAssertEqual(number, p.getNumber());
}

@end
