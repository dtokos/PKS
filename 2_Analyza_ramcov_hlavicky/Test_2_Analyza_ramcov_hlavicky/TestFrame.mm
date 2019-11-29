#import <XCTest/XCTest.h>
#include "Frames.hpp"

class MockFrame : public Frame {
public:
	MockFrame(unsigned serialNumber, uint32_t pcapLength, uint8_t *data, uint32_t dataLength) :
		Frame(serialNumber, pcapLength, data, dataLength) {}
	Type frameType() {return Frame::EthernetII;}
	string name() {return "MockFrame";}
	uint8_t *data() {return raw + 14;}
};

@interface TestFrame : XCTestCase

@end

@implementation TestFrame

- (void)testFrame {
	uint8_t rawFrame[] = {
		0x00, 0x04, 0x76, 0xA4, 0xE4, 0x8C, 0x00, 0x00,	0xC0, 0xD7, 0x80, 0xC2, 0x08, 0x00, 0x45, 0x00,
		0x00, 0x28, 0x0C, 0x36, 0x40, 0x00, 0x80, 0x06,	0x2B, 0x5A, 0x93, 0xAF, 0x62, 0xEE, 0x45, 0x38,
		0x87, 0x6A, 0x04, 0x70, 0x00, 0x50, 0x7E, 0x6C,	0x06, 0x32, 0x56, 0x7D, 0x30, 0xA8, 0x50, 0x10,
		0x44, 0x70, 0x97, 0xA0, 0x00, 0x00,
	};
	MockFrame frame(1, 54, rawFrame, 54);
	XCTAssertEqual(frame.serialNumber, 1);
	XCTAssertEqual(frame.pcapLength, 54);
	XCTAssertTrue(frame.source().asString() == "00:00:c0:d7:80:c2");
	XCTAssertEqual(frame.source().asInt(), 0x0000C0D780C2);
	XCTAssertTrue(frame.destination().asString() == "00:04:76:a4:e4:8c");
	XCTAssertEqual(frame.destination().asInt(), 0x000476A4E48C);
	XCTAssertEqual(frame.length(), 0x0800);
	XCTAssertEqual(frame.wireLength(), 64);
}

- (void)testFrameWireLength {
	uint8_t rawFrame[70] = {0};
	MockFrame frame1(1, 10, rawFrame, 10);
	MockFrame frame2(1, 70, rawFrame, 70);
	XCTAssertEqual(frame1.wireLength(), 64);
	XCTAssertEqual(frame2.wireLength(), 74);
}

- (void)testEthernetFrame {
	uint8_t rawFrame = 0;
	EthernetIIFrame frame(1, 1, &rawFrame, 1);
	XCTAssertTrue(frame.name() == "Ethernet II");
	XCTAssertTrue(frame.data() == frame.raw + 14);
}

- (void)testIeee802_3RawFrame {
	uint8_t rawFrame = 0;
	Ieee802_3RawFrame frame(1, 1, &rawFrame, 1);
	XCTAssertTrue(frame.name() == "IEEE 802.3 – Raw");
	XCTAssertTrue(frame.data() == frame.raw + 17);
}

- (void)testIeee802_3LlcFrame {
	uint8_t rawFrame = 0;
	Ieee802_3LlcFrame frame(1, 1, &rawFrame, 1);
	XCTAssertTrue(frame.name() == "IEEE 802.3 LLC");
	XCTAssertTrue(frame.data() == frame.raw + 17);
}

@end
