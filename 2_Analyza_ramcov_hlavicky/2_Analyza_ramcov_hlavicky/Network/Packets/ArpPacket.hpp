#ifndef ArpPacket_hpp
#define ArpPacket_hpp

#include "./IPv4Packet.hpp"
#include "../Frames/Frame.hpp"

using MACAddress = Frame::MACAddress;
using IPAddress = IPv4Packet::IPAddress;

class ArpPacket : public Packet {
public:
	enum OpcodeType {
		Request = 1,
		Response = 2,
	};
	
	ArpPacket(uint8_t *raw, string name) : Packet(raw, name) {}
	
	Type packetType() {return Packet::ARP;}
	string name() {return _name;}
	
	uint16_t opcode() {return ntohs(getField<uint16_t>(6));}
	
	MACAddress senderMACAddress() {return getField<MACAddress>(8);}
	IPAddress senderIPAddress() {return getField<IPAddress>(14);}
	
	MACAddress targetMACAddress() {return getField<MACAddress>(18);}
	IPAddress targetIPAddress() {return getField<IPAddress>(24);}
	
};

#endif
