#ifndef IPv4Packet_hpp
#define IPv4Packet_hpp

#include "./Packet.hpp"

class IPv4Packet : public Packet {
public:
	IPv4Packet(uint8_t *raw, string name) : _name(name), Packet(raw) {}
	
	Type packetType() {return Packet::IPv4;}
	string name() {return _name;}
	
private:
	string _name;
};

#endif
