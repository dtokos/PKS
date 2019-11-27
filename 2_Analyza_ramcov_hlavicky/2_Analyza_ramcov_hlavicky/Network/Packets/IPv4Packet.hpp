#ifndef IPv4Packet_hpp
#define IPv4Packet_hpp

#include "./Packet.hpp"

class IPv4Packet : public Packet {
public:
	struct IPAddress {
		uint8_t octet1;
		uint8_t octet2;
		uint8_t octet3;
		uint8_t octet4;
		
		string asString() {
			stringstream stream;
			stream << static_cast<int>(octet1) << '.'
				<< static_cast<int>(octet2) << '.'
				<< static_cast<int>(octet3) << '.'
				<< static_cast<int>(octet4);
			
			return stream.str();
		}
		
		uint32_t asInt() {return *(uint32_t *)this;}
	};
	
	IPv4Packet(uint8_t *raw, string name) : _name(name), Packet(raw) {}
	
	Type packetType() {return Packet::IPv4;}
	string name() {return _name;}
	
	uint8_t version() {return (getField<uint8_t>(0) & 0xF0) >> 4;}
	uint8_t ihl() {return getField<uint8_t>(0) & 0x0F;}
	uint8_t protocol() {return getField<uint8_t>(9);}
	IPAddress sourceAddress() {return getField<IPAddress>(12);}
	IPAddress destinationAddress() {return getField<IPAddress>(16);}
	uint8_t *data() {return raw + ihl() * 4;}
	
private:
	string _name;
};

#endif
