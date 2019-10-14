#include "Socket.hpp"

Socket::Socket(int fileDescriptor) : fileDescriptor(fileDescriptor) {}

void Socket::write(void *data, unsigned length) {
	send(fileDescriptor, data, length, 0);
}

int Socket::read(void *data, unsigned maxLength) {
	return ::recv(fileDescriptor, data, maxLength, 0);
}
