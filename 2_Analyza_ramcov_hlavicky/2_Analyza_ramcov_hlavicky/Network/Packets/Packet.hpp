#ifndef Packet_hpp
#define Packet_hpp

#include <cstdlib>

using namespace std;

class Packet {
protected:
	template<typename Type>
	constexpr Type getField(uint32_t offset) {
		return *reinterpret_cast<Type *>(raw + offset);
	}
public:
	enum Type {IPv4, ARP, Other};
	
	Packet(uint8_t *raw) : raw(raw) {}
	virtual ~Packet() {};
	
	virtual Type packetType() = 0;
	virtual string name() = 0;
	
private:
	uint8_t *raw;
};

#endif
