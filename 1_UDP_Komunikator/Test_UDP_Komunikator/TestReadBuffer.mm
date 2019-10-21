#import <XCTest/XCTest.h>
#include "ReadBuffer.hpp"

@interface TestReadBuffer : XCTestCase

@end

@implementation TestReadBuffer

-(void)testSimpleRead {
	Segment s(1, "asdfg", 5);
	ReadBuffer buffer;
	buffer.add(s);
	char destination[5];
	buffer.read(destination, 5);
	
	XCTAssertEqual(destination[0], 'a');
	XCTAssertEqual(destination[1], 's');
	XCTAssertEqual(destination[2], 'd');
	XCTAssertEqual(destination[3], 'f');
	XCTAssertEqual(destination[4], 'g');
}

-(void)testReadFromMultipleSegments {
	char s1d[Segment::MaxDataLength];
	fill(s1d, s1d + Segment::MaxDataLength, '0');
	char s2d[Segment::MaxDataLength];
	fill(s2d, s2d + Segment::MaxDataLength, '1');
	Segment s1(1, s1d, Segment::MaxDataLength);
	Segment s2(1, s2d, Segment::MaxDataLength);
	ReadBuffer buffer;
	buffer.add(s1);
	buffer.add(s2);
	char destination[Segment::MaxDataLength * 2];
	buffer.read(destination, Segment::MaxDataLength * 2);
	
	for (int i = 0; i < Segment::MaxDataLength; i++) {
		XCTAssertEqual(destination[i], '0');
		XCTAssertEqual(destination[i + Segment::MaxDataLength], '1');
	}
}

-(void)testReadMultipleTimes {
	Segment s1(1, "aaa", 3);
	Segment s2(1, "bbb", 3);
	Segment s3(1, "ccc", 3);
	ReadBuffer buffer;
	buffer.add(s1);
	buffer.add(s2);
	buffer.add(s3);
	
	char part1[4];
	buffer.read(part1, 4);
	XCTAssertEqual(part1[0], 'a');
	XCTAssertEqual(part1[1], 'a');
	XCTAssertEqual(part1[2], 'a');
	XCTAssertEqual(part1[3], 'b');
	
	char part2[4];
	buffer.read(part2, 4);
	XCTAssertEqual(part2[0], 'b');
	XCTAssertEqual(part2[1], 'b');
	XCTAssertEqual(part2[2], 'c');
	XCTAssertEqual(part2[3], 'c');
	
	char part3;
	buffer.read(&part3, 1);
	XCTAssertEqual(part3, 'c');
}

-(void)testEmptyBufferShouldNotRead {
	Segment s(1, "asdfg", 5);
	ReadBuffer buffer;
	buffer.add(s);
	char destination[5];
	
	buffer.read(destination, 5);
	XCTAssertEqual(buffer.read(destination, 1), 0);
}

-(void)testWrapAround {
	char s1d[Segment::MaxDataLength];
	fill(s1d, s1d + Segment::MaxDataLength, 'A');
	char s2d[500];
	fill(s2d, s2d + 500, 'X');
	char s3d[Segment::MaxDataLength];
	fill(s3d, s3d + Segment::MaxDataLength, 'Y');
	Segment s1(1, s1d, Segment::MaxDataLength);
	Segment s2(1, s2d, 500);
	Segment s3(1, s3d, Segment::MaxDataLength);
	ReadBuffer buffer;
	char destination[Segment::MaxDataLength] = {0};
	
	for (int i = 0; i < 7; i++)
		buffer.add(s1);
	
	buffer.read(destination, 500);
	buffer.add(s2);
	buffer.add(s3);
	
	for (int i = 0; i < 7; i++)
		buffer.read(destination, Segment::MaxDataLength);
	
	XCTAssertEqual(buffer.read(destination, Segment::MaxDataLength), Segment::MaxDataLength);
	for (int i = 0; i < Segment::MaxDataLength; i++)
		XCTAssertEqual(destination[i], 'Y');
}

@end
