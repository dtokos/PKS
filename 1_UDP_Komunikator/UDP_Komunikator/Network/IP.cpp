#include "IP.hpp"

IP::IP(in_addr address) : address(address) {}

IP IP::fromString(string addressString) {
	in_addr address{};
	
	if (inet_aton(addressString.c_str(), &address) != 1)
		throw InvalidIP(addressString);
	
	return IP(address);
}

in_addr IP::getAddress() {
	return address;
}
