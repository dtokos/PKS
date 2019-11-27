#ifndef EthernetIIFrame_hpp
#define EthernetIIFrame_hpp

#include "./Frame.hpp"

class EthernetIIFrame : public Frame {
public:
	EthernetIIFrame(unsigned serialNumber, uint32_t pcapLength, uint8_t *data, uint32_t dataLength) :
		Frame(serialNumber, pcapLength, data, dataLength) {};
	
	uint16_t type() {return ntohs(getField<uint16_t>(12));}
	Type frameType() {return Frame::EthernetII;}
	string name() {return "Ethernet II";}
	uint8_t *data() {return raw + 14;}
};

#endif
