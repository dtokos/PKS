#ifndef Ieee802_3LlcSnapFrame_hpp
#define Ieee802_3LlcSnapFrame_hpp

#include "./Frame.hpp"

class Ieee802_3LlcSnapFrame : public Frame {
public:
	Ieee802_3LlcSnapFrame(unsigned serialNumber, uint32_t pcapLength, uint8_t *data, uint32_t dataLength) : Frame(serialNumber, pcapLength, data, dataLength) {};
	
	string name() {return "IEEE 802.3 LLC + SNAP";}
	uint8_t *data() {return raw + 22;}
};

#endif
