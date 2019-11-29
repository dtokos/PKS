#ifndef Packet_hpp
#define Packet_hpp

#include <cstdlib>
#include "../Segments/Segment.hpp"

using namespace std;

class Packet {
public:
	enum Type {IPv4, ARP, Other};
	
	Packet(uint8_t *raw, string name) : raw(raw), _name(name) {}
	virtual ~Packet() {if (segment != NULL) delete segment;};
	
	virtual Type packetType() {return Other;}
	virtual string name() {return _name;}
	
	Segment *segment = NULL;
	
protected:
	uint8_t *raw;
	string _name;
	
	template<typename Type>
	constexpr Type getField(uint32_t offset) {
		return *reinterpret_cast<Type *>(raw + offset);
	}
};

#endif
