#ifndef Message_hpp
#define Message_hpp

#include <cstdlib>

using namespace std;

class Message {
public:
	enum Type {TFTP = 0, Other = 1};
	
	Message(Type type, uint8_t *raw, string name) : _type(type), raw(raw), _name(name) {}
	virtual ~Message() {};
	
	virtual Type messageType() {return _type;}
	virtual string name() {return _name;}
	
protected:
	uint8_t *raw;
	string _name;
	Type _type;
	
	template<typename Type>
	constexpr Type getField(uint32_t offset) {
		return *reinterpret_cast<Type *>(raw + offset);
	}
};

#endif
