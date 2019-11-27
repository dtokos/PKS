#ifndef OtherPacket_hpp
#define OtherPacket_hpp

#include "./Packet.hpp"

class OtherPacket : public Packet {
public:
	OtherPacket(uint8_t *raw, string name) : _name(name), Packet(raw) {}
	
	Type packetType() {return Packet::Other;}
	string name() {return _name;}
	
private:
	string _name;
};

#endif
