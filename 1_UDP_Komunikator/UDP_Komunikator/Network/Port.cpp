#include "Port.hpp"

Port::Port(int number) : number(number) {}

Port Port::fromNumber(int number) {
	if (number < MinPortNumber || number > MaxPortNumber)
		throw PortOutOfRange(number);
	
	return Port(number);
}

unsigned short Port::getNumber() {
	return number;
}

unsigned short Port::getNormalizedNumber() {
	return htons(number);
}
