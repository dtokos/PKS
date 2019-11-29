#ifndef ArpPacket_hpp
#define ArpPacket_hpp

class ArpPacket : public Packet {
public:
	ArpPacket(uint8_t *raw, string name) : Packet(raw, name) {}
	
	Type packetType() {return Packet::ARP;}
	string name() {return _name;}
};

#endif
