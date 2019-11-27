#ifndef Packet_hpp
#define Packet_hpp

#include <cstdlib>
#include "../Segments/Segment.hpp"

using namespace std;

class Packet {
public:
	enum Type {IPv4, ARP, Other};
	
	Packet(uint8_t *raw) : raw(raw) {}
	virtual ~Packet() {if (segment != NULL) delete segment;};
	
	virtual Type packetType() = 0;
	virtual string name() = 0;
	
	Segment *segment = NULL;
	
protected:
	uint8_t *raw;
	
	template<typename Type>
	constexpr Type getField(uint32_t offset) {
		return *reinterpret_cast<Type *>(raw + offset);
	}
};

#endif
