#ifndef Segment_hpp
#define Segment_hpp

#include <cstdlib>
#include "../Messages/Message.hpp"

using namespace std;

class Segment {
public:
	enum Type {TCP, UDP, ICMP, Other};
	
	Segment(uint8_t *raw, string name) : raw(raw), _name(name) {}
	virtual ~Segment() {if (message != NULL) delete message;};
	
	virtual Type segmentType() {return Other;}
	virtual string name() {return _name;}
	
	Message *message = NULL;
	
protected:
	uint8_t *raw;
	string _name;
	
	template<typename Type>
	constexpr Type getField(uint32_t offset) {
		return *reinterpret_cast<Type *>(raw + offset);
	}
};

#endif
