#ifndef Message_hpp
#define Message_hpp

#include <cstdlib>

using namespace std;

class Message {
public:
	enum Type {TCP, UDP, ICMP, Other};
	
	Message(uint8_t *raw, string name) : raw(raw), _name(name) {}
	virtual ~Message() {};
	
	virtual Type messageType() {return Other;}
	virtual string name() {return _name;}
	
protected:
	uint8_t *raw;
	string _name;
	
	template<typename Type>
	constexpr Type getField(uint32_t offset) {
		return *reinterpret_cast<Type *>(raw + offset);
	}
};

#endif
