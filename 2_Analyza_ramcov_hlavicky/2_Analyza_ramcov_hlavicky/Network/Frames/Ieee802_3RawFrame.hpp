#ifndef Ieee802_3RawFrame_hpp
#define Ieee802_3RawFrame_hpp

#include "./Frame.hpp"

class Ieee802_3RawFrame : public Frame {
public:
	Ieee802_3RawFrame(unsigned serialNumber, uint32_t pcapLength, uint8_t *data, uint32_t dataLength) : Frame(serialNumber, pcapLength, data, dataLength) {};
	
	string name() {return "IEEE 802.3 – Raw";}
	uint8_t *data() {return raw + 17;}
};

#endif
