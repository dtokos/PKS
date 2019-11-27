#ifndef ArpPacket_hpp
#define ArpPacket_hpp

class ArpPacket : public Packet {
public:
	ArpPacket(uint8_t *raw, string name) : _name(name), Packet(raw) {}
	
	Type packetType() {return Packet::ARP;}
	string name() {return _name;}
	
private:
	string _name;
};

#endif
