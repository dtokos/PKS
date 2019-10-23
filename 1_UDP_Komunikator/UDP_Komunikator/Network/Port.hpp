#ifndef Port_hpp
#define Port_hpp

#include <exception>

using namespace std;

class Port {
public:
	class PortOutOfRange : public exception {
	public:
		int portNumber;
		
		PortOutOfRange(int portNumber) : portNumber(portNumber) {}
		
		const char * what () const throw () {
			return "Given port is out of range";
		}
	};
	
	static const uint16_t MinPortNumber = 1024;
	static const uint16_t MaxPortNumber = 65535;
	
	static Port fromNumber(int number);
	unsigned short getNumber();
	unsigned short getNormalizedNumber();
	
private:
	Port(int number);
	unsigned short number;
};

#endif
