#include "Port.hpp"

Port::Port(int number) : number(number) {}

Port Port::fromNumber(int number) {
	if (number < 1024 || number > 65535)
		throw PortOutOfRange(number);
	
	return Port(number);
}

unsigned short Port::getNumber() {
	return number;
}
