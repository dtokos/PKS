#ifndef Socket_hpp
#define Socket_hpp

#include <sys/socket.h>

class Socket {
public:
	Socket(int fileDescriptor);
	void write(void *data, unsigned length);
	int read(void *data, unsigned length);
	
private:
	int fileDescriptor;
};

#endif
