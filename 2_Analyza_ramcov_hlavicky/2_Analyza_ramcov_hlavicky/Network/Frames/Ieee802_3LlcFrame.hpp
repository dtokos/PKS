#ifndef Ieee802_3LlcFrame_hpp
#define Ieee802_3LlcFrame_hpp

#include "./Frame.hpp"

class Ieee802_3LlcFrame : public Frame {
public:
	Ieee802_3LlcFrame(unsigned serialNumber, uint32_t pcapLength, uint8_t *data, uint32_t dataLength) :
		Frame(serialNumber, pcapLength, data, dataLength) {};
	
	Type frameType() {return Frame::Ieee802_3Llc;}
	string name() {return "IEEE 802.3 LLC";}
	uint8_t *data() {return raw + 17;}
};

#endif
