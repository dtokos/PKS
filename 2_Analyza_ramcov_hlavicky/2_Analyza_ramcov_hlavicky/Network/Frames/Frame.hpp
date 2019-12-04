#ifndef Frame_hpp
#define Frame_hpp

#include <arpa/inet.h>
#include <iomanip>
#include <string>
#include <sstream>
#include <cstdlib>
#include <string.h>
#include "../Packets/Packet.hpp"

using namespace std;

class Frame {
protected:
	template<typename Type>
	constexpr Type getField(uint32_t offset) {
		return *reinterpret_cast<Type *>(raw + offset);
	}
	
public:
	enum Type {EthernetII, Ieee802_3Raw, Ieee802_3Llc, Ieee802_3LlcSnap};
	
	struct MACAddress {
		uint8_t octet1;
		uint8_t octet2;
		uint8_t octet3;
		uint8_t octet4;
		uint8_t octet5;
		uint8_t octet6;
		
		string asString(char delimiter = ':') {
			stringstream stream;
			stream << setfill('0') << hex
				<< setw(2) << static_cast<int>(octet1) << delimiter
				<< setw(2) << static_cast<int>(octet2) << delimiter
				<< setw(2) << static_cast<int>(octet3) << delimiter
				<< setw(2) << static_cast<int>(octet4) << delimiter
				<< setw(2) << static_cast<int>(octet5) << delimiter
				<< setw(2) << static_cast<int>(octet6);
			
			return stream.str();
		}
		
		uint64_t asInt() {
			return static_cast<uint64_t>(octet1) << 40 |
				static_cast<uint64_t>(octet2) << 32 |
				static_cast<uint64_t>(octet3) << 24 |
				static_cast<uint64_t>(octet4) << 16 |
				static_cast<uint64_t>(octet5) << 8 |
				static_cast<uint64_t>(octet6);
		}
	};
	Frame(unsigned serialNumber, uint32_t pcapLength, uint8_t *data, uint32_t capturedLength) :
		serialNumber(serialNumber),
		pcapLength(pcapLength),
		capturedLength(capturedLength),
		raw(new uint8_t[capturedLength])
		{memcpy(raw, data, capturedLength);}
	virtual ~Frame() {
		delete[] raw;
		if (packet != NULL)
			delete packet;
	}
	
	MACAddress destination() {return getField<MACAddress>(0);}
	MACAddress source() {return getField<Frame::MACAddress>(6);}
	uint16_t length() {return ntohs(getField<uint16_t>(12));}
	
	uint32_t wireLength() {return max(static_cast<uint32_t>(64), pcapLength + 4);}
	virtual Type frameType() = 0;
	virtual string name() = 0;
	virtual uint8_t *data() = 0;
	
	unsigned serialNumber;
	uint32_t pcapLength, capturedLength;
	uint8_t *raw;
	
	Packet *packet = NULL;
	Segment *segment = NULL;
	Message *message = NULL;
};

#endif
