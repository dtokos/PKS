#ifndef Packet_hpp
#define Packet_hpp

#include <cstdlib>

using namespace std;

class Packet {
public:
	enum Type {IPv4, ARP, Other};
	
	Packet(uint8_t *raw) : raw(raw) {}
	
	virtual Type packetType() = 0;
	virtual string name() = 0;
	
private:
	uint8_t *raw;
};

#endif
